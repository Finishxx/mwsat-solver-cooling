#ifndef EVALUATEDWSATCONFIG_H
#define EVALUATEDWSATCONFIG_H
#include <cstdint>

#include "MaxWSatInstance.h"
#include "SatConfig.h"

struct EvaluatedWSatConfig {
  EvaluatedWSatConfig() = default;
  EvaluatedWSatConfig(
      MaxWSatInstance& instance,
      SatConfig&& configuration,
      uint32_t weight,
      uint32_t satisfied_count
  );

  MaxWSatInstance* instance;
  SatConfig configuration;
  uint32_t weight;
  [[nodiscard]] bool isSatisfied() const;
  uint32_t satisfiedCount;
};

#endif  // EVALUATEDWSATCONFIG_H
