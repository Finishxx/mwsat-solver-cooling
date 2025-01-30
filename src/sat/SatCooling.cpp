#include "SatCooling.h"

#include "Rng.h"

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
  for (uint32_t i = 0; configuration.underlying.size() < i; i++) {
    if (configuration.underlying[i] == true) {
      totalWeights += instance.variables().at(i + 1).weight();
    }
  }
  return SatCriteria(instance, satisfiedClauses, totalWeights);
}
//       std::vector<std::vector<int32_t>> clauses,
// std::vector<int32_t> weights

SatCooling::SatCooling(
    std::vector<std::vector<int32_t>> clauses, std::vector<int32_t> weights
)
    : instance(WSatInstance(clauses, weights)) {}
