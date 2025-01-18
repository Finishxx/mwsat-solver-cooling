#include "SatConfig.h"

#include <utility>

bool SatConfig::byId(uint32_t id) const { return underlying[id - 1]; }
std::_Bit_reference SatConfig::byId(uint32_t id) { return underlying[id - 1]; }
SatConfig::SatConfig(WSatInstance* instance, std::vector<bool>&& underlying)
    : instance(instance), underlying(std::move(underlying)) {}