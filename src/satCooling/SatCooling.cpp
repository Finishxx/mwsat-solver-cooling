#include "SatCooling.h"

#include "Rng.h"

SatCriteria::SatCriteria(
    const WSatInstance& instance, uint32_t satisfiedCount, int32_t weights
)
    : instance(&instance), satisfiedCount(satisfiedCount), weights(weights) {}

bool SatCriteria::isSatisfied() const {
  return satisfiedCount == instance->clauses().size();
}

bool SatCriteria::operator<(const SatCriteria& other) const {
  // We are worse
  return howMuchWorseThan(other) > 0;
}
bool SatCriteria::operator>=(const SatCriteria& other) const {
  // We are better (worse in minus)
  return howMuchWorseThan(other) <= 0;
}

// We need to know how worse we are => him - us
double SatCriteria::howMuchWorseThan(const SatCriteria& other) const {
  // Both formulas satisfied => compare weights
  if (this->isSatisfied() and other.isSatisfied())
    return other.weights - this->weights;

  // Neither formulas satisfied => compare satisfied
  if (not this->isSatisfied() && not other.isSatisfied())
    return other.satisfiedCount - this->satisfiedCount;

  // We are satisfied, but he is not => penalize him
  if (this->isSatisfied() && not other.isSatisfied()) {
    double satisfiedRatio =
        static_cast<double>(other.satisfiedCount) / instance->clauses().size();
    return (other.weights * (satisfiedRatio * satisfiedRatio)) - this->weights;
  }

  // We are not satisfied, but he is => penalize ourselves
  if (not this->isSatisfied() && other.isSatisfied()) {
    double satisfiedRatio =
        static_cast<double>(this->satisfiedCount) / instance->clauses().size();
    return other.weights - (this->weights * (satisfiedRatio * satisfiedRatio));
  }
}

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

SatCriteria SatCooling::evaluateConfiguration(const SatConfig& configuration
) const {
  uint32_t satisfiedClauses = 0;
  for (const Clause clause& : instance.clauses) {
    for (const Term disjunct& : clause.disjuncts()) {
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
SatCooling::SatCooling(ParsedDimacsFile parsedInstance)
    : instance(WSatInstance(parsedInstance.clauses, parsedInstance.weights)) {}
