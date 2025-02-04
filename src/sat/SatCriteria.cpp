#include "SatCriteria.h"

#include <iostream>

#include "debug.h"

double SatCriteria::satisfiedRatio() const {
  DEBUG_PRINT(
      "Satisfied ratio:"
      << (static_cast<double>(satisfiedCount) /
          static_cast<double>(instance->clauses().size()))
  )
  return static_cast<double>(satisfiedCount) /
      static_cast<double>(instance->clauses().size());
}

SatCriteria::SatCriteria(
    const WSatInstance& instance, uint32_t satisfiedCount, int32_t weights
)
    : instance(&instance), satisfiedCount(satisfiedCount), weights(weights) {}

int32_t SatCriteria::weight() const { return weights; }
double SatCriteria::normalizedWeight() const {
  return static_cast<double>(weights) /
      static_cast<double>(instance->weightTotal());
}
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
    return other.normalizedWeight() - this->normalizedWeight();

  // Neither formulas satisfied => compare satisfied
  if (not this->isSatisfied() && not other.isSatisfied())
    return (other.satisfiedRatio() - this->satisfiedRatio());

  // We are satisfied, but he is not => penalize him
  if (this->isSatisfied() && not other.isSatisfied()) {
    return (other.normalizedWeight() * other.satisfiedRatio()) -
        this->normalizedWeight();
  }

  // We are not satisfied, but he is => penalize ourselves
  // if (not this->isSatisfied() && other.isSatisfied())
  return other.normalizedWeight() -
      (this->normalizedWeight() * this->satisfiedRatio());
}
std::ostream& operator<<(std::ostream& os, const SatCriteria& criteria) {
  os << "SatCriteria(satisfiedCount=" << criteria.satisfied()
     << ", weight=" << criteria.weight()
     << ", isValid=" << (criteria.isValid() ? "true" : "false")
     << ", isSatisfied=" << (criteria.isSatisfied() ? "true" : "false") << ")";
  return os;
}