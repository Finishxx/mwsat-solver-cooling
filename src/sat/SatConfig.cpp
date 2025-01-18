#include "SatConfig.h"

#include <utility>

SatConfig::SatConfig(WSatInstance* instance, std::vector<bool>&& underlying)
    : instance(instance), underlying(std::move(underlying)) {}