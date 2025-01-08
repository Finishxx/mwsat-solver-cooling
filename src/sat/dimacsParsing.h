#pragma once
#include <Sat.h>

#include <istream>

struct ParsedDimacsFile {
  ParsedDimacsFile(
      const uint32_t var_count,
      std::vector<std::vector<int32_t>> clauses,
      std::vector<int32_t> weights
  )
      : varCount(var_count), clauses(std::move(clauses)), weights(std::move(weights)) {}
  uint32_t varCount;
  std::vector<std::vector<int32_t>> clauses;
  std::vector<int32_t> weights;
};

ParsedDimacsFile parseDimacsFile(std::istream& input);
