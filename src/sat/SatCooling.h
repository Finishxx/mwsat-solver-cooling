#pragma once
#include <SatConfig.h>
#include <SatCriteria.h>
#include <WSatInstance.h>

class SatCooling {
 private:
  WSatInstance instance;
  static constexpr double p = 0.4;

 public:
  [[nodiscard]] SatConfig getRandomConfiguration() const;
  [[nodiscard]] SatConfig getRandomNeighbor(
      const SatConfig& configuration
  ) const;
  [[nodiscard]] SatCriteria evaluateConfiguration(
      const SatConfig& configuration
  ) const;
  explicit SatCooling(
      std::vector<std::vector<int32_t>> clauses, std::vector<int32_t> weights
  );
};
