#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
  CLI::App app{"App description"};

  std::filesystem::path inputPath;
  app.add_option("-f,--file", inputPath, "A help string")->required();

  std::string seed;
  app.add_option("-s,--seed", seed, "64-bit hex seed")->required();

  double initialTemperature;
  app.add_option("-t,--startTemperature", initialTemperature)->required();

  double finalTemperature;
  app.add_option("-T,--endTemperature", finalTemperature)->required();

  double cooling;
  app.add_option("-c,--cooling", cooling, "Cooling coefficient")->required();

  uint32_t equilibrium;
  app.add_option("-e,--equilibrium", equilibrium)->required();

  std::filesystem::path debugFile;
  app.add_option(
         "-d,--debug",
         debugFile,
         "Where to write <stepNum> <currentSatisfied> <currentWeight> "
         "<maxWeight> on each line per step"
  )
      ->required();

  uint32_t maxIter;
  app.add_option("-i,--maxIter", maxIter, "Iterations before end")->required();

  uint32_t withoutGain;
  app.add_option("-g,--gain", withoutGain, "End after steps without gain")
      ->required();

  uint32_t withoutChange;
  app.add_option(
         "-a,--adjustment",
         withoutChange,
         "End after steps without change"
  )
      ->required();

  CLI11_PARSE(app, argc, argv);

  // std::err << stepTotal << " " << lastChange << " " << wasSatisfied << " " <<
  // maxWeight << std::endl;

  bool success = true;
  if (success)
    return 0;
  else
    return 1;
}
