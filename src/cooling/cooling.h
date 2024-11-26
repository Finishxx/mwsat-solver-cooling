#pragma once
#include <sat.h>

/**
 * Generic simulated cooling solver
 * Requires a Problem, Configuration and Optimization Criteria
 *
 * Problem
 * - Interface with following methods:
 *  - Configuration getRandomNeighbor()
 *  - Criteria evaluateConfiguration(Configuration)
 *  - void applyConfiguration(Configuration)
 * Configuration
 *  - Just output class we know nothing about and is passed around
 * Optimization Criteria
 *  - bool {better|worse}[orEqual](Criteria)
 *  - double howMuch{better|worse}(Criteria)
 */
class Criteria {
 public:
  virtual bool operator==(const Criteria& other);
  virtual bool operator<(const Criteria& other);
  virtual bool operator>(const Criteria& other);
  virtual bool operator>=(const Criteria& other);
  virtual bool operator<=(const Criteria& other);
  virtual double howMuchBetterThan(const Criteria& other);
  virtual double howMuchWorseThan(const Criteria& other);
};
class Configuration {};
class Problem {
  virtual Configuration getRandomNeighbor();
  virtual Criteria evaluateConfiguration(const Configuration& configuration);
  virtual void applyConfiguration(const Configuration& configuration);
};

template <typename T>
concept Criteriable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <typename Problem, typename Configuration, typename Criteria>
class Cooling<Problem, Configuration, Criteria> {
  Problem problem;
  Configuration bestConfiguration;
  Configuration current;
  Criteria bestCriteria;

 public:
  Cooling(
      Problem problem, uint32_t maxTries, double coolingFactor,
      double stopTemperature, double startTemperature
  );
};

// =============================================================================
/**
 * Different approach to generality
 *
 * The Cooling will be interacting with class a class implementing Problem
 * The Problem can be an intermediary class between the SAT and Cooling
 * It will also provide the Criteria value class and calculation
 * It will return the Configurations too
 *
 */
