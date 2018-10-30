#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "tokenizer.h"

using namespace std;

using value_t = double;

struct Logger;

enum IfState {
  Exec,
  Wait,
};

struct Interpreter {
  Tokenizer tokenizer;

  Interpreter(string &&, Logger &);

  void interpret();

 private:
  vector<string> tokens;
  vector<string>::iterator tokens_it;
  vector<value_t> stack;
  vector<IfState> if_states;
  uint8_t if_wait_skip_len;
  unordered_map<string, vector<string>> functions;
  vector<pair<string, uint8_t>> fn_stack;
  Logger &logger;

  value_t pop();
  void push(value_t);
  value_t top();
  void flush();
  void dup();
  void inc();
  void dec();
  void swap();
  void print_string();
  optional<string> next_token();
  void read_function();
};
