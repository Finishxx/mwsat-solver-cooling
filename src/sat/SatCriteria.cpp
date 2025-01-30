#include "SatCriteria.h"

SatCriteria::SatCriteria(
    const WSatInstance& instance, uint32_t satisfiedCount, int32_t weights
)
    : instance(&instance), satisfiedCount(satisfiedCount), weights(weights) {}

int32_t SatCriteria::weight() const { return weights; }
uint32_t SatCriteria::satisfied() const { return satisfiedCount; }

bool SatCriteria::isValid() const { return isSatisfied(); }

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
  // if (not this->isSatisfied() && other.isSatisfied()) {
  double satisfiedRatio =
      static_cast<double>(this->satisfiedCount) / instance->clauses().size();
  return other.weights - (this->weights * (satisfiedRatio * satisfiedRatio));
}