#include "Rng.h"

Rng::Rng(rng_seed_t seed) {
  splitmix64_set_seed(seed);
  for (uint64_t& i : state.s) {
    i = splitmix64_next();
  }
}

Rng::Rng(std::ifstream stateFile){rng_seed_read()}

Rng::Rng(std::ifstream)

    uint64_t Rng::next() {}
bool Rng::write(std::ofstream& ofs) {}
