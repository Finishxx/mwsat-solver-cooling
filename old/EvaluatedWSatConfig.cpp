#include "EvaluatedWSatConfig.h"

EvaluatedWSatConfig::EvaluatedWSatConfig(
    WSatInstance& instance,
    SatConfig&& configuration,
    uint32_t satisfied_count
)
    : instance(&instance),
      configuration(std::move(configuration)),
      satisfiedCount(satisfied_count) {
  weight = calculateWeight();
}

int32_t EvaluatedWSatConfig::calculateWeight() const {
  int32_t total = 0;
  for (uint32_t variableId = 0; variableId < instance->variables().size();
       variableId++) {
    if (configuration.underlying[variableId] == true)
      total += instance->variables()[variableId].weight();
  }
  return total;
}

bool EvaluatedWSatConfig::isSatisfied() const {
  return satisfiedCount == instance->variables().size();
}