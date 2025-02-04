#include "SatCooling.h"

#include "Rng.h"
#include "debug.h"

// SatCooling
SatConfig SatCooling::getRandomConfiguration() const {
  Rng::next();
  std::vector<bool> bools;
  bools.resize(instance.variables().size());

  for (auto val : bools) {
    if (Rng::next() % 2 == 0) val.flip();
  }
  return SatConfig(std::move(bools));
}

SatConfig SatCooling::getRandomNeighbor(const SatConfig& configuration) const {
  /*for (const Clause& clause : instance.clauses()) {
    bool allSatisfied =
        std::ranges::all_of(clause.disjuncts(), [&](const Term& term) {
          bool isSet = configuration.byId(term.id());
          if ((term.isPlain() and isSet) or (term.isNegated() and not isSet)) {
            return true;
          }
          return false;
        });
    if (allSatisfied) {
      DEBUG_PRINT("All satisfied");
      const Term& toFlip =
          clause.disjuncts()[Rng::next() % clause.disjuncts().size()];
      std::vector<bool> copy = configuration.underlying;
      copy[toFlip.id() - 1].flip();
      return SatConfig(std::move(copy));
    }
  }*/
  // Else all satisfied
  std::vector<bool> copy = configuration.underlying;
  copy[Rng::next() % copy.size()].flip();
  return SatConfig(std::move(copy));
}

SatCriteria SatCooling::evaluateConfiguration(
    const SatConfig& configuration
) const {
  uint32_t satisfiedClauses = 0;
  for (const Clause& clause : instance.clauses()) {
    for (const Term& disjunct : clause.disjuncts()) {
      bool isSet = configuration.byId(disjunct.id());
      if ((disjunct.isPlain() && isSet) or (disjunct.isNegated() && !isSet)) {
        satisfiedClauses += 1;
        break;
      }
    }
  }

  int32_t totalWeights = 0;
  for (uint32_t i = 0; i < configuration.underlying.size(); i++) {
    if (configuration.underlying[i] == true) {
      totalWeights += instance.variables().at(i).weight();
    }
  }
  return SatCriteria(instance, satisfiedClauses, totalWeights);
}

SatCooling::SatCooling(
    std::vector<std::vector<int32_t>> clauses, std::vector<int32_t> weights
)
    : instance(WSatInstance(clauses, weights)) {}
