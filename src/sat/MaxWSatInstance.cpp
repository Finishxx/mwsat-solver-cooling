#include "MaxWSatInstance.h"

#include <bits/ranges_algo.h>

Clause::Clause(std::vector<Term>&& disjuncts) {
  std::ranges::sort(disjuncts_, [](const Term& t1, const Term& t2) {
    return t1.id() < t2.id();
  });
  std::ranges::unique(disjuncts_, [](const Term& t1, const Term& t2) {
    return t1.id() == t2.id();
  });
}
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