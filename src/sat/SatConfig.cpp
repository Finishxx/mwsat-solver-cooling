#include "SatConfig.h"

#include <utility>

SatConfig::SatConfig(MaxWSatInstance* instance, std::vector<bool>&& underlying)
    : instance(instance), underlying(std::move(underlying)) {}