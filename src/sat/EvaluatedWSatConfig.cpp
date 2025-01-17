#include "EvaluatedWSatConfig.h"

EvaluatedWSatConfig::EvaluatedWSatConfig(
    MaxWSatInstance& instance,
    SatConfig&& configuration,
    uint32_t weight,
    uint32_t satisfied_count
)
    : instance(&instance),
      configuration(std::move(configuration)),
      weight(weight),
      satisfiedCount(satisfied_count) {}
bool EvaluatedWSatConfig::isSatisfied() const {
  return satisfiedCount == instance->variables().size();
}