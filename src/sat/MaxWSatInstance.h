#ifndef MAXWSATINSTANCE_H
#define MAXWSATINSTANCE_H
#include <cmath>
#include <cstdint>
#include <vector>

/** Term with given id can be either plain or negated */
class Term {
 private:
  int32_t underlying;

 public:
  /** If the given int is negative, then the term is negated, plain otherwise */
  explicit Term(int32_t underlying);
  [[nodiscard]] uint32_t id() const;
  [[nodiscard]] bool isNegated() const;
  [[nodiscard]] bool isPlain() const;
  Term flip() const;
};

/** Clause in CNF - Terms with disjunction between them */
class Clause {
 private:
  /** Unique and sorted */
  std::vector<Term> disjuncts_;

 public:
  explicit Clause(std::vector<Term>&& disjuncts);
  /** Unique and sorted */
  [[nodiscard]] const std::vector<Term>& disjuncts() const;
  /** Expensive operation - all pairs must be evaluated */
  [[nodiscard]] bool isSatisfiable() const;
  [[nodiscard]] bool containsVariable(uint32_t variableId) const;
};

/** Holds info regarding a variable with given id */
class Variable {
 private:
  uint32_t id_;
  int32_t weight_;
  std::vector<const Clause*> occurences_;

 public:
  [[nodiscard]] uint32_t id() const;
  [[nodiscard]] int32_t weight() const;
  [[nodiscard]] const std::vector<const Clause*>& occurences() const;
  explicit Variable(
      uint32_t id, int32_t weight, const std::vector<Clause>& allClauses
  );
};

/** Immutable Max Weighted SAT instance */
class MaxWSatInstance {
 private:
  std::vector<Variable> variables_;
  std::vector<Clause> clauses_;

 public:
  [[nodiscard]] const std::vector<Variable>& variables() const;
  [[nodiscard]] const std::vector<Clause>& clauses() const;
  MaxWSatInstance(
      std::vector<std::vector<int32_t>>& clauses, std::vector<int32_t>& weights
  );
  /** Slow, because it calls isSatisfiable() on all clauses */
  bool isSatisfiable() const;
};

#endif  // MAXWSATINSTANCE_H
