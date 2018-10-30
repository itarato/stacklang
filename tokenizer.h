#pragma once

#include <string>
#include <vector>

using namespace std;

struct Tokenizer {
  string source;

  Tokenizer(string &&source) : source(move(source)){};

  vector<string> tokenize() {
    vector<string> out{};

    size_t prev_find{0};

    while (prev_find < source.size()) {
      size_t current_find = source.find(' ', prev_find);
      if (current_find == string::npos) {
        current_find = source.size();
      }

      out.emplace_back(source.substr(prev_find, current_find - prev_find));

      prev_find = current_find + 1;
    }

    return out;
  };
};
