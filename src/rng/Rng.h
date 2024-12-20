#pragma once

#include <fstream>
#include <memory>
#include <optional>

/** Facade for xoshiro256plus generator */
class Rng {
 public:
  std::array<uint64_t, 4> getState() const;

  static uint64_t next();
  static double nextDouble();
  /** @return true on success */
  static void initWithSeed(uint64_t seed);
  static void initWithState(std::array<uint64_t, 4> state);
};
