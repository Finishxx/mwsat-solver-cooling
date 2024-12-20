#include "Rng.h"

#include "xoshiro256plus.h"

std::array<uint64_t, 4> Rng::getState() const {
  rng_state_t* state = nullptr;
  rng_get_state(state);
  return std::
      array<uint64_t, 4>{state->s[0], state->s[1], state->s[2], state->s[3]};
}
uint64_t Rng::next() { return rng_next(); }
double Rng::nextDouble() { return rng_next_double(); }
void Rng::initWithSeed(uint64_t seed) { rng_set_seed(seed); }
void Rng::initWithState(std::array<uint64_t, 4> state) {}