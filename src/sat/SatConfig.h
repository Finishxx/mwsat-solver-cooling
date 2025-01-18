#ifndef SATCONFIG_H
#define SATCONFIG_H
#include <vector>

#include "WSatInstance.h"

/** Mutable */
struct SatConfig {
  WSatInstance* instance;
  std::vector<bool> underlying;
  SatConfig() = default;
  SatConfig(WSatInstance* instance, std::vector<bool>&& underlying);
};

#endif  // SATCONFIG_H
