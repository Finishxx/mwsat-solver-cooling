#include "MaxWSatSolver.h"

#include <algorithm>
#include <cassert>

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

// ===================== End LiveClause =====================

// ===================== LiveVariable =====================

uint32_t MaxWSatSolver::LiveVariable::weight() const {
  return original->weight();
}
uint32_t MaxWSatSolver::LiveVariable::id() const { return original->id(); }

// ===================== End LiveVariable =====================

// ===================== MaxWSatSolver =====================

void MaxWSatSolver::setConfig(SatConfig& config) {
  assert(config.underlying.size() == instance_->variables().size());
  // Clear previous context
  variables.clear();
  clauses.clear();

  // Init clauses
  clauses.reserve(instance_->clauses().size());
  for (const Clause& clause : instance_->clauses()) {
    LiveClause(&clause, config);
  }

  // Init variables
  for (const Variable& variable : instance_->variables()) {
    variables.emplace_back(&variable);
  }

  for (LiveClause& clause : clauses) {
    for (const LiveTerm& term : clause.terms()) {
      variables[term.id()].occurrences.push_back(&clause);
    }
  }
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