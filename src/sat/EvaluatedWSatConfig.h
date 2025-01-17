#ifndef EVALUATEDWSATCONFIG_H
#define EVALUATEDWSATCONFIG_H
#include <cstdint>

#include "MaxWSatInstance.h"
#include "SatConfig.h"

struct EvaluatedWSatConfig {
  EvaluatedWSatConfig() = default;
  /** Calculates the initial weight itself */
  EvaluatedWSatConfig(
      MaxWSatInstance& instance,
      SatConfig&& configuration,
      uint32_t satisfied_count
  );

  /** Calculates weight based on  */
  [[nodiscard]] int32_t calculateWeight() const;

  MaxWSatInstance* instance;
  SatConfig configuration;
  int32_t weight;
  [[nodiscard]] bool isSatisfied() const;
  uint32_t satisfiedCount;
};

#endif  // EVALUATEDWSATCONFIG_H
