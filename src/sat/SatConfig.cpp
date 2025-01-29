#include "SatConfig.h"

#include <utility>

bool SatConfig::byId(uint32_t id) const { return underlying[id - 1]; }
std::_Bit_reference SatConfig::byId(uint32_t id) { return underlying[id - 1]; }
SatConfig::SatConfig(std::vector<bool>&& underlying)
    : underlying(std::move(underlying)) {}