#pragma once
/**
 * Generic simulated cooling solver
 * Requires a Problem, Configuration and Optimization Criterion
 * If a class extends the provided base classes, then it is suitable for use
 *
 * Problem
 *  - can produce nextRandomConfiguration()
 *  - can applyConfiguration(Configuration)
 * Configuration
 *  - the output we are looking for, based on which criterion is calculated
 *  - Just passed around
 * Optimization criterion
 *  ? Maybe could do with just a number?
 *  - can compare itself along the lines of {better|worse}[orEqual]()
 */
class Criterion {
 public:
  virtual bool operator==(const Criterion& other);
  virtual bool operator<(const Criterion& other);
  virtual bool operator>(const Criterion& other);
  virtual bool operator>=(const Criterion& other);
  virtual bool operator<=(const Criterion& other);
};
class Configuration {};
class Problem {
  virtual Configuration nextRandomConfiguration();
  virtual void doConfiguration(const Configuration& configuration);
  virtual Criterion evaluateConfiguration(const Configuration& configuration);
};



class Cooling {
 private:
  Problem problem;
  Configuration bestConfiguration;
  Configuration current;
  Criterion bestCriterion;

 public:
  Cooling(Problem problem, uint32_t maxTries, double coolingFactor, double stopTemperature, double startTemperature);
};