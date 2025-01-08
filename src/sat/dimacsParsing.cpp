#include "dimacsParsing.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <sstream>

std::vector<std::string> toWords(std::string const& string) {
  std::vector<std::string> words;
  std::istringstream iss(string);
  std::string word;

  while (std::getline(iss, word, ' ')) words.push_back(word);
  return words;
}

// Inspiration here:
// https://courses.fit.cvut.cz/NI-KOP/tutorials/files/sat-brute-force.html
ParsedDimacsFile parseDimacsFile(std::istream& input) {
  uint32_t varCount = UINT32_MAX;
  uint32_t clauseCount = UINT32_MAX;
  std::vector<int32_t> weights;
  std::vector<std::vector<int32_t>> clauses;

  for (std::string line; std::getline(input, line);) {
    std::vector<std::string> words = toWords(line);
    std::cout << fmt::format("words: {}", words) << std::endl;

    if (line.empty() || words[0] == "c") continue;  // Comment
    if (words[0] == "p") {  // Parsing format and var/clause counts
      if (words[1] != "mwcnf") {
        throw std::invalid_argument("Given format is not mwcnf");
      }
      varCount = std::stoi(words[2]);
      clauseCount = std::stoi(words[3]);
    } else if (words[0] == "w") {  // Parsing weights
      if (words.size() - 2 != varCount) {
        throw std::invalid_argument(
            fmt::format("Expected {} weights, but got {}", varCount, words.size() - 2)
        );
      }
      if (*(words.end() - 1) != "0") throw std::invalid_argument("Weights not ended by 0");

      weights.resize(words.size() - 2);
      std::transform(words.begin() + 1, words.end() - 1, weights.begin(), [](std::string const& s) {
        return std::stoi(s);
      });
    } else {  // Line with clause
      std::vector<int32_t> clause(words.size());
      std::transform(words.begin(), words.end(), clause.begin(), [](std::string const& word) {
        return std::stoi(word);
      });
      clause.pop_back();
      clauses.push_back(clause);
    }
  }

  std::cout << "Here" << std::endl;

  if (clauseCount != clauses.size())
    throw std::invalid_argument(
        fmt::format("Expected {} clauses, but got {}", clauseCount, clauses.size())
    );
  if (weights.empty()) throw std::invalid_argument("Expected any weights");
  if (clauses.empty()) throw std::invalid_argument("Expected any clauses");

  return {varCount, clauses, weights};
}
