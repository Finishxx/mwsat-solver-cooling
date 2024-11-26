#pragma once
#include <cooling.h>
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
concept Criteriable = std::totally_ordered<T> && requires(T t1, T t2) {
  { t1.howMuchWorseThan(t2) } -> std::convertible_to<double>;
  { t1.howMuchBetterThan(t2) } -> std::convertible_to<double>;
};

template <typename T, typename Configuration, Criteriable Criteria>
concept Problemable<Configuration, Criteria> =
    requires(T t, Configuration config) {
      { t.getRandomNeighbor() } -> std::convertible_to<Configuration>;
      { t.evaluateConfiguration(config) } -> std::convertible_to<Criteria>;
      { t.applyConfiguration(config) };
    };

template <
    typename Configuration, Criteriable Criteria,
    Problemable<Configuration, Criteria> Problem>
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
