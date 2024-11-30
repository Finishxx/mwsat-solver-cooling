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
 private:
  rng_state_t state;
  Rng() = default;

 public:
  uint64_t next();
  /** @return true on success */
  bool writeStateToFile(std::ofstream& ofs);

  static Rng createWithSeed(rng_seed_t seed);
  static Rng createWithSeedFile(std::ifstream& ifs);
  static Rng createWithState(rng_state_t state);
  static Rng createWithStateFile(std::ifstream& ifs);

  /** Reads the state from file */
  explicit Rng(std::ifstream stateFile);
  explicit Rng(rng_state_t state) : state(state) {}
  /** Never construct with seed with 0 as per instructions */
  explicit Rng(rng_seed_t seed);
};
