#include "MaxWSatInstance.h"

#include <assert.h>

#include <algorithm>
#include <ranges>

// ===================== Term =====================
Term::Term(int32_t underlying) : underlying(underlying) {}
uint32_t Term::id() const { return std::abs(underlying); }
bool Term::isNegated() const { return underlying < 0; }
bool Term::isPlain() const { return underlying > 0; }
Term Term::flip() const { return Term(-underlying); }
// ===================== EndTerm =====================

// ===================== Clause =====================
Clause::Clause(std::vector<Term>&& disjuncts) {
  std::ranges::sort(disjuncts_, [](const Term& t1, const Term& t2) {
    return t1.id() < t2.id();
  });
  auto duplicates =
      std::ranges::unique(disjuncts_, [](const Term& t1, const Term& t2) {
        return t1.id() == t2.id();
      });
  disjuncts_.erase(duplicates.begin(), duplicates.end());
}
const std::vector<Term>& Clause::disjuncts() const { return disjuncts_; }

// Could be made faster, but will be called only once, so no big deal
bool Clause::isSatisfiable() const {
  for (unsigned outer = 0; outer < disjuncts_.size(); outer++) {
    for (unsigned inner = outer; inner < disjuncts_.size(); inner++) {
      if (disjuncts_[outer].isNegated() && disjuncts_[inner].isPlain() ||
          disjuncts_[outer].isPlain() && disjuncts_[inner].isNegated()) {
        return false;
      }
    }
  }
  return true;
}
bool Clause::containsVariable(uint32_t variableId) const {
  return std::ranges::any_of(disjuncts_, [&variableId](const Term& t) {
    return t.id() == variableId;
  });
}

// ===================== EndClause =====================

// ===================== Variable =====================

uint32_t Variable::id() const { return id_; }
int32_t Variable::weight() const { return weight_; }
const std::vector<const Clause*>& Variable::occurences() const {
  return occurences_;
}

Variable::Variable(
    uint32_t id, int32_t weight, const std::vector<Clause>& allClauses
)
    : id_(id), weight_(weight) {
  for (const Clause& clause : allClauses) {
    if (clause.containsVariable(id)) occurences_.push_back(&clause);
  }
}
// ===================== EndVariable =====================

// ===================== Instance =====================

const std::vector<Variable>& MaxWSatInstance::variables() const {
  return variables_;
}
const std::vector<Clause>& MaxWSatInstance::clauses() const { return clauses_; }

// This work could be extracted into smaller functions for reuse
MaxWSatInstance::MaxWSatInstance(
    std::vector<std::vector<int32_t>>& clauses, std::vector<int32_t>& weights
) {
  assert(!clauses.empty());
  assert(!weights.empty());
  clauses_.reserve(clauses.size());
  // Initialize clauses
  for (const std::vector<int32_t>& clause : clauses) {
    // Initialize each Term in clause
    std::vector<Term> terms = std::vector<Term>();
    terms.reserve(clause.size());
    for (int32_t term : clause) {
      terms.emplace_back(term);
    }
    clauses_.emplace_back(std::move(terms));
  }

  // Initialize Variables
  // Weights: Seq<int32_t> => Seq<Variable>
  variables_.reserve(weights.size());
  for (uint32_t i = 0; i < weights.size(); i++) {
    variables_.emplace_back(i, weights.at(i), clauses_);
  }
}
bool MaxWSatInstance::isSatisfiable() const {
  return std::ranges::all_of(clauses_, [](const Clause& clause) {
    return clause.isSatisfiable();
  });
}

// ===================== EndInstance =====================
