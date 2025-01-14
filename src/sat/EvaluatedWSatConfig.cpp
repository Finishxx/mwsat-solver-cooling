#include "EvaluatedWSatConfig.h"

EvaluatedWSatConfig::EvaluatedWSatConfig(
    MaxWSatInstance& instance,
    SatConfig&& configuration,
    uint32_t weight,
    bool is_satisfied,
    uint32_t satisfied_count
)
    : instance(&instance),
      configuration(std::move(configuration)),
      weight(weight),
      isSatisfied(is_satisfied),
      satisfiedCount(satisfied_count) {}