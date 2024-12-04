#pragma once

#include <fstream>
#include <memory>
#include <optional>

#include "xoshiro256plus.h"

/**
 * Wrapper for xoshiro256plus generator
 *
 * Uses the factory pattern for named construction
 */
class Rng {
 public:
  static uint64_t next();
  static double nextDouble();
  /** @return true on success */
  static bool writeStateToFile(std::ofstream& ofs);
  static void initWithSeed(rng_seed_t seed);
  static void initWithState(rng_state_t state);
};
