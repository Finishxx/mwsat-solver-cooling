#ifndef SATCONFIG_H
#define SATCONFIG_H
#include <vector>

#include "MaxWSatInstance.h"

/** Mutable */
struct SatConfig {
  MaxWSatInstance* instance;
  std::vector<bool> underlying;
  SatConfig(MaxWSatInstance* instance, std::vector<bool>&& underlying);
};

#endif  // SATCONFIG_H
