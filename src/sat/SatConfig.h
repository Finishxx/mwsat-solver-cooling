#ifndef SATCONFIG_H
#define SATCONFIG_H
#include <vector>

#include "WSatInstance.h"

/** Mutable */
struct SatConfig {
  WSatInstance* instance;
  std::vector<bool> underlying;
  [[nodiscard]] bool byId(uint32_t id) const;
  [[nodiscard]] std::_Bit_reference byId(uint32_t id);
  SatConfig() = default;
  SatConfig(WSatInstance* instance, std::vector<bool>&& underlying);
};

#endif  // SATCONFIG_H
