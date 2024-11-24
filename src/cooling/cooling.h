#pragma once

/**
 * Generic simulated cooling solver
 * Requires a Problem, Configuration and Optimization Criterion
 *
 * Problem
 *  - can produce nextRandomConfiguration
 *  - can apply the configuration
 *  - can calculate the optimization criterion of a configuration
 * Configuration
 *  - the output we are looking for, based on which criterion is calculated
 * Optimization criterion // TODO: Maybe could do with just a number?
 *  - can compare itself to other criteria given a configuration and Problem
 */
class Criterion {};

// Abstract base class for all configurations
class Configuration {};

class Problem {
  virtual Configuration nextRandomConfiguration();
  virtual void doConfiguration(const Configuration& configuration);
  virtual Criterion evaluateConfiguration(const Configuration& configuration);
};

template <typename Problem, typename Criterion>
struct Cooling<Problem, Configuration, Criterion> {};