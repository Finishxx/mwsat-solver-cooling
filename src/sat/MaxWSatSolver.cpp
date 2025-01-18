#include "MaxWSatSolver.h"

#include <algorithm>
#include <cassert>
#include <ranges>

// ===================== LiveTerm =====================

bool MaxWSatSolver::LiveTerm::operator<(const LiveTerm& other) const {
  return this->id() < other.id();
}
MaxWSatSolver::LiveTerm::LiveTerm(Term original, bool isSet)
    : isSet_(isSet), original(original) {
  if ((isSet && original.isPlain()) || (!isSet && original.isNegated())) {
    isSatisfied_ = true;
  } else {
    isSatisfied_ = false;
  }
}
const Term& MaxWSatSolver::LiveTerm::originalTerm() const { return original; }
uint32_t MaxWSatSolver::LiveTerm::id() const { return original.id(); }
bool MaxWSatSolver::LiveTerm::isSatisfied() const { return isSatisfied_; }
bool MaxWSatSolver::LiveTerm::isSet() const { return isSet_; }
bool MaxWSatSolver::LiveTerm::isUnset() const { return !isSet_; }
void MaxWSatSolver::LiveTerm::set() {
  if (!isSet_) {
    isSet_ = true;
    // Satisfied if set and the original was plain
    isSatisfied_ = original.isPlain();
  }
}

void MaxWSatSolver::LiveTerm::unset() {
  if (isSet_) {
    isSet_ = false;
    // Satisfied if unset and the original was negated
    isSatisfied_ = original.isNegated();
  }
}

void MaxWSatSolver::LiveTerm::flip() {
  isSet_ = !isSet_;
  isSatisfied_ = !isSatisfied_;
}
// ===================== End LiveTerm =====================

// ===================== LiveTerm =====================

// Redundancy with const findTerm
MaxWSatSolver::LiveTerm* MaxWSatSolver::LiveClause::findTerm(uint32_t termId) {
  auto term =
      std::ranges::lower_bound(terms_, termId, std::less{}, &LiveTerm::id);

  if (term == terms_.end()) {
    return nullptr;
  }
  return &(*term);
}
MaxWSatSolver::LiveClause::LiveClause(const Clause* clause, SatConfig& config)
    : original(clause), satisfiedCount(0) {
  terms_.reserve(clause->disjuncts().size());
  for (const auto& term : clause->disjuncts()) {
    LiveTerm& ref = terms_.emplace_back(term, config.underlying[term.id()]);
    if (ref.isSatisfied()) satisfiedCount += 1;
  }
  if (!std::ranges::is_sorted(terms_, std::less<>{}, &LiveTerm::id)) {
    std::ranges::sort(terms_, {}, &LiveTerm::id);
  }
}

const Clause& MaxWSatSolver::LiveClause::originalClause() const {
  return *original;
}
const std::vector<MaxWSatSolver::LiveTerm>& MaxWSatSolver::LiveClause::terms(
) const {
  return terms_;
}
const MaxWSatSolver::LiveTerm* MaxWSatSolver::LiveClause::getTerm(uint32_t id
) const {
  std::ranges::borrowed_iterator_t<const std::vector<LiveTerm>&> term =
      std::ranges::lower_bound(terms_, id, std::less{}, &LiveTerm::id);

  if (term == terms_.end()) {
    return nullptr;
  }
  return &(*term);
}
bool MaxWSatSolver::LiveClause::isSatisfied() const {
  return satisfiedCount > 0;
}
void MaxWSatSolver::LiveClause::setVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr || term->isSet()) return;

  term->set();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
void MaxWSatSolver::LiveClause::unsetVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr || term->isUnset()) return;

  term->unset();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
void MaxWSatSolver::LiveClause::flipVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr) return;

  term->flip();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
MaxWSatSolver::LiveVariable::LiveVariable(const Variable* variable, bool isSet)
    : original(variable), isSet_(isSet) {}

// ===================== End LiveClause =====================

// ===================== LiveVariable =====================

int32_t MaxWSatSolver::LiveVariable::weight() const {
  return original->weight();
}
uint32_t MaxWSatSolver::LiveVariable::id() const { return original->id(); }
bool MaxWSatSolver::LiveVariable::isSet() const { return isSet_; }

int32_t MaxWSatSolver::LiveVariable::flip() {
  int32_t satisfiabilityChange = 0;
  for (LiveClause* clause : occurrences) {
    assert(clause->getTerm(id()) != nullptr);

    bool wasSatisfied = clause->isSatisfied();
    clause->flipVariable(id());
    bool satisfiedNow = clause->isSatisfied();

    if (wasSatisfied && !satisfiedNow) satisfiabilityChange -= 1;
    if (!wasSatisfied && satisfiedNow) satisfiabilityChange += 1;
  }
  return satisfiabilityChange;
}

// ===================== End LiveVariable =====================

// ===================== MaxWSatSolver =====================

const MaxWSatSolver::LiveVariable& MaxWSatSolver::variableById(
    uint32_t variableId
) const {
  return variables[variableId - 1];
}
MaxWSatSolver::LiveVariable& MaxWSatSolver::variableById(uint32_t variableId) {
  return variables[variableId - 1];
}

std::ranges::subrange<std::vector<MaxWSatSolver::LiveVariable>::iterator>
MaxWSatSolver::legalVariables() {
  return std::ranges::subrange{variables.begin(), variables.end()};
}
void MaxWSatSolver::flipVariable(uint32_t variableId) {
  LiveVariable& variable = variables[variableId];
  const bool wasSet = variable.isSet();

  int32_t satisfiedCountChange = variable.flip();
  config_.satisfiedCount += satisfiedCountChange;

  if (wasSet)
    config_.weight -= variable.weight();
  else
    config_.weight += variable.weight();
  assert(config_.weight == config_.calculateWeight());
}

void MaxWSatSolver::setConfig(SatConfig& config) {
  assert(config.underlying.size() == instance_->variables().size());
  // Clear previous context                      n
  variables.clear();
  clauses.clear();

  // Init clauses
  clauses.reserve(instance_->clauses().size());
  for (const Clause& clause : instance_->clauses()) {
    LiveClause(&clause, config);
  }

  // Init variables
  variables.reserve(instance_->variables().size());
  for (const Variable& variable : instance_->variables()) {
    variables.emplace_back(&variable, config.underlying[variable.id()]);
  }

  // Add variable's occurrences
  for (LiveClause& clause : clauses) {
    for (const LiveTerm& term : clause.terms()) {
      variables[term.id()].occurrences.push_back(&clause);
    }
  }

  // Create Config
  auto satisfiedCount = std::ranges::count_if(
      clauses, [](const LiveClause& clause) { return clause.isSatisfied(); }
  );
  config_ = EvaluatedWSatConfig(*instance_, std::move(config), satisfiedCount);
}

MaxWSatSolver::MaxWSatSolver(
    MaxWSatInstance& instance, SatConfig& initialConfig
)
    : instance_(&instance) {
  setConfig(initialConfig);
}

const EvaluatedWSatConfig& MaxWSatSolver::currentConfiguration() const {
  return config_;
}

EvaluatedWSatConfig MaxWSatSolver::exportConfiguration() const {
  return EvaluatedWSatConfig(config_);
}

const MaxWSatInstance& MaxWSatSolver::instance() const { return *instance_; }

// ===================== End MaxWSatSolver =====================