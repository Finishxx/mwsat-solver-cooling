# RNG generator

The generator uses static state in itself, therefore it doesL not make sense to
wrap it in a singleton in my honest opinion.

## Expected usage

We will need both `rng_state_t` and `rng_seed_t` types.

### State

```c++
void rng_set_state (rng_state_t* pstate);
void rng_get_state (rng_state_t* pstate);
```

### Seeding

```c++
uint64_t splitmix64_next();
void splitmix64_set_seed (uint64_t seed);
```

### Generating

```c++
uint64_t rng_next(void);
double rng_next_double ();
```
