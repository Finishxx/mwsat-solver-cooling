#ifndef MAXWSATINSTANCE_H
#define MAXWSATINSTANCE_H
#include <cmath>
#include <cstdint>
#include <vector>

class Term {
 private:
  int32_t underlying;

 public:
  /** If the given int is negative, then the term is negated, plain otherwise */
  Term(int32_t underlying) : underlying(underlying) {}
  uint32_t id() const { return std::abs(underlying); };
  bool isNegated() const { return underlying < 0; };
  bool isPlain() const { return underlying > 0; };
};

class Clause {
 private:
  std::vector<Term> disjuncts;

 public:
  const std::vector<Term>& disjunct() const;
  bool isSatisfiable() const;
};

class Variable {
 private:
  uint32_t id_;
  int32_t weight_;
  std::vector<Clause*> occurences_;

 public:
  uint32_t id() const;
  int32_t weight() const;
  const std::vector<Clause*>& occurences() const;
};

/** Owns all Variables and Clauses */
class MaxWSatInstance {
 private:
  std::vector<Variable> variables_;
  std::vector<Clause> clauses_;

 public:
  const std::vector<Variable>& variables() const;
  const std::vector<Clause>& clauses() const;
  MaxWSatInstance(std::vector<std::vector<int32_t>>& clauses, std::vector<int32_t>& weights);
};

#endif  // MAXWSATINSTANCE_H
