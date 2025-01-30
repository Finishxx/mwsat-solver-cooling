#ifndef SATCONFIG_H
#define SATCONFIG_H
#include <vector>

#include "WSatInstance.h"

/** Mutable */
struct SatConfig {
  std::vector<bool> underlying;
  [[nodiscard]] bool byId(uint32_t id) const;
  [[nodiscard]] std::_Bit_reference byId(uint32_t id);
  SatConfig() = default;
  explicit SatConfig(std::vector<bool>&& underlying);
  bool operator==(const SatConfig& other) const = default;
  bool operator!=(const SatConfig& other) const = default;
};

#endif  // SATCONFIG_H
