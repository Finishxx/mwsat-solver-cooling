#ifndef SATCRITERIA_H
#define SATCRITERIA_H
#include <ostream>

#include "WSatInstance.h"

/**
 * Bridge between Cooling and Sat modules
 *
 * Implements the Problemable interface
 */
class SatCriteria {
 private:
  WSatInstance const* instance;
  uint32_t satisfiedCount;
  int32_t weights;

  [[nodiscard]] double satisfiedRatio() const;
  [[nodiscard]] double normalizedWeight() const;

 public:
  friend std::ostream& operator<<(
      std::ostream& os, const SatCriteria& criteria
  );

  SatCriteria() = default;
  SatCriteria(
      const WSatInstance& instance, uint32_t satisfiedCount, int32_t weights
  );
  [[nodiscard]] int32_t weight() const;
  [[nodiscard]] uint32_t satisfied() const;

  [[nodiscard]] bool isValid() const;
  [[nodiscard]] bool isSatisfied() const;
  bool operator<(const SatCriteria& other) const;
  bool operator>=(const SatCriteria& other) const;
  [[nodiscard]] double howMuchWorseThan(const SatCriteria& other) const;
};

#endif  // SATCRITERIA_H
