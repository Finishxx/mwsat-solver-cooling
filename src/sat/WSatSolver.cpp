#include "WSatSolver.h"

#include <algorithm>
#include <cassert>
#include <ranges>

// ===================== LiveTerm =====================

bool WSatSolver::LiveTerm::operator<(const LiveTerm& other) const {
  return this->id() < other.id();
}
WSatSolver::LiveTerm::LiveTerm(Term original, bool isSet)
    : isSet_(isSet), original(original) {
  if ((isSet && original.isPlain()) || (!isSet && original.isNegated())) {
    isSatisfied_ = true;
  } else {
    isSatisfied_ = false;
  }
}
const Term& WSatSolver::LiveTerm::originalTerm() const { return original; }
uint32_t WSatSolver::LiveTerm::id() const { return original.id(); }
bool WSatSolver::LiveTerm::isSatisfied() const { return isSatisfied_; }
bool WSatSolver::LiveTerm::isSet() const { return isSet_; }
bool WSatSolver::LiveTerm::isUnset() const { return !isSet_; }
void WSatSolver::LiveTerm::set() {
  if (!isSet_) {
    isSet_ = true;
    // Satisfied if set and the original was plain
    isSatisfied_ = original.isPlain();
  }
}

void WSatSolver::LiveTerm::unset() {
  if (isSet_) {
    isSet_ = false;
    // Satisfied if unset and the original was negated
    isSatisfied_ = original.isNegated();
  }
}

void WSatSolver::LiveTerm::flip() {
  isSet_ = !isSet_;
  isSatisfied_ = !isSatisfied_;
}
// ===================== End LiveTerm =====================

// ===================== LiveTerm =====================

// Redundancy with const findTerm
WSatSolver::LiveTerm* WSatSolver::LiveClause::findTerm(uint32_t termId) {
  auto term =
      std::ranges::lower_bound(terms_, termId, std::less{}, &LiveTerm::id);

  if (term == terms_.end()) {
    return nullptr;
  }
  return &(*term);
}
WSatSolver::LiveClause::LiveClause(const Clause* clause, SatConfig& config)
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

const Clause& WSatSolver::LiveClause::originalClause() const {
  return *original;
}
const std::vector<WSatSolver::LiveTerm>& WSatSolver::LiveClause::terms() const {
  return terms_;
}
const WSatSolver::LiveTerm* WSatSolver::LiveClause::getTerm(uint32_t id) const {
  std::ranges::borrowed_iterator_t<const std::vector<LiveTerm>&> term =
      std::ranges::lower_bound(terms_, id, std::less{}, &LiveTerm::id);

  if (term == terms_.end()) {
    return nullptr;
  }
  return &(*term);
}
bool WSatSolver::LiveClause::isSatisfied() const { return satisfiedCount > 0; }
void WSatSolver::LiveClause::setVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr || term->isSet()) return;

  term->set();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
void WSatSolver::LiveClause::unsetVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr || term->isUnset()) return;

  term->unset();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
void WSatSolver::LiveClause::flipVariable(uint32_t variableId) {
  LiveTerm* term = findTerm(variableId);
  if (term == nullptr) return;

  term->flip();
  if (term->isSatisfied()) {
    satisfiedCount += 1;
  } else {
    satisfiedCount -= 1;
  }
}
WSatSolver::LiveVariable::LiveVariable(const Variable* variable, bool isSet)
    : original(variable), isSet_(isSet) {}

// ===================== End LiveClause =====================

// ===================== LiveVariable =====================

int32_t WSatSolver::LiveVariable::weight() const { return original->weight(); }
uint32_t WSatSolver::LiveVariable::id() const { return original->id(); }
bool WSatSolver::LiveVariable::isSet() const { return isSet_; }

int32_t WSatSolver::LiveVariable::flip() {
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

const WSatSolver::LiveVariable& WSatSolver::variableById(uint32_t variableId
) const {
  assert(variableId != 0);
  assert(variableId <= variables.size());
  return variables[variableId - 1];
}
WSatSolver::LiveVariable& WSatSolver::variableById(uint32_t variableId) {
  assert(variableId != 0);
  assert(variableId <= variables.size());
  return variables[variableId - 1];
}

std::ranges::subrange<std::vector<WSatSolver::LiveVariable>::iterator>
WSatSolver::legalVariables() {
  return std::ranges::subrange{variables.begin(), variables.end()};
}
void WSatSolver::flipVariable(uint32_t variableId) {
  LiveVariable& variable = variableById(variableId);
  const bool wasSet = variable.isSet();

  int32_t satisfiedCountChange = variable.flip();
  config_.satisfiedCount += satisfiedCountChange;

  if (wasSet)
    config_.weight -= variable.weight();
  else
    config_.weight += variable.weight();
  assert(config_.weight == config_.calculateWeight());
}

void WSatSolver::setConfig(SatConfig& config) {
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
  // More space for dummy element
  variables.reserve(instance_->variables().size() + 1);
  // Insert dummy element
  variables.emplace_back(nullptr, false);
  for (const Variable& variable : instance_->variables()) {
    variables.emplace_back(&variable, config.underlying[variable.id()]);
  }

  // Add variable's occurrences
  for (LiveClause& clause : clauses) {
    for (const LiveTerm& term : clause.terms()) {
      variableById(term.id()).occurrences.push_back(&clause);
    }
  }

  // Create Config
  auto satisfiedCount = std::ranges::count_if(
      clauses, [](const LiveClause& clause) { return clause.isSatisfied(); }
  );
  config_ = EvaluatedWSatConfig(*instance_, std::move(config), satisfiedCount);
}

WSatSolver::WSatSolver(WSatInstance& instance, SatConfig& initialConfig)
    : instance_(&instance) {
  setConfig(initialConfig);
}

const EvaluatedWSatConfig& WSatSolver::currentConfiguration() const {
  return config_;
}

EvaluatedWSatConfig WSatSolver::exportConfiguration() const {
  return EvaluatedWSatConfig(config_);
}

const WSatInstance& WSatSolver::instance() const { return *instance_; }

// ===================== End MaxWSatSolver =====================