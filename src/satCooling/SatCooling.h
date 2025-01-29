#pragma once
#include "../sat/SatConfig.h"
#include "../sat/dimacsParsing.h"
#include "Cooling.h"

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

 public:
  SatCriteria(
      const WSatInstance& instance, uint32_t satisfiedCount, int32_t weights
  );

  [[nodiscard]] bool isSatisfied() const;
  bool operator<(const SatCriteria& other) const;
  [[nodiscard]] double howMuchWorseThan(const SatCriteria& other) const;
};

class SatCooling {
 private:
  WSatInstance instance;

 public:
  [[nodiscard]] SatConfig getRandomConfiguration() const;
  [[nodiscard]] SatConfig getRandomNeighbor(const SatConfig& configuration
  ) const;
  [[nodiscard]] SatCriteria evaluateConfiguration(const SatConfig& configuration
  ) const;
  explicit SatCooling(ParsedDimacsFile parsedInstance);
};
