#include "interpreter.h"
#include <cmath>
#include <iostream>
#include <regex>
#include "logger.h"

using namespace std;

namespace StackLang {
value_t add(value_t lhs, value_t rhs) { return lhs + rhs; }
value_t mul(value_t lhs, value_t rhs) { return lhs * rhs; }
value_t div(value_t lhs, value_t rhs) { return lhs / rhs; }
value_t sub(value_t lhs, value_t rhs) { return lhs - rhs; }
value_t pow(value_t lhs, value_t rhs) { return std::pow(lhs, rhs); }
value_t sqrt(value_t v) { return std::sqrt(v); }
value_t mod(value_t lhs, value_t rhs) {
  return static_cast<value_t>(static_cast<int>(lhs) % static_cast<int>(rhs));
}
value_t print(Logger &logger, value_t lhs) {
  logger.info("\x1B[107m\x1B[30m\x1B[1m  %f  \x1B[0m\n", lhs);
  return lhs;
}
}  // namespace StackLang

Interpreter::Interpreter(istream_iterator<string> isit, Logger &logger)
    : isit(isit),
      if_states({}),
      if_wait_skip_len(0),
      fn_stack({}),
      logger(logger){};

void Interpreter::interpret() {
  const regex num_regex("^[\\-+]?\\d+(\\.\\d+|)$");
  for (optional<string> token_opt = next_token(); token_opt.has_value();
       token_opt = next_token()) {
    const string token = token_opt.value();
    logger.debug("Token read: '%s'\n", token.c_str());

    if (!if_states.empty() && if_states.back() == IfState::Wait) {
      if (token == "if") {
        if_wait_skip_len++;
      }

      if (token == "endif") {
        if (if_wait_skip_len == 0) {
          if_states.pop_back();
        } else {
          if_wait_skip_len--;
        }
      }

      if (token == "else" && if_wait_skip_len == 0) {
        if_states.back() = IfState::Exec;
      }

      continue;
    } else {
      if (token == "if") {
        if (top() != 0.0) {
          if_states.emplace_back(IfState::Exec);
        } else {
          if_states.emplace_back(IfState::Wait);
        }
        continue;
      } else if (token == "else") {
        if_states.back() = IfState::Wait;
        continue;
      } else if (token == "endif") {
        if_states.pop_back();
        continue;
      }
    }

    if (regex_match(token, num_regex)) {
      push(stod(token));
    } else if (token == "fn") {
      read_function();
    } else if (token == "+") {
      push(StackLang::add(pop(), pop()));
    } else if (token == "-") {
      push(StackLang::sub(pop(), pop()));
    } else if (token == "*") {
      push(StackLang::mul(pop(), pop()));
    } else if (token == "/") {
      push(StackLang::div(pop(), pop()));
    } else if (token == "^") {
      push(StackLang::pow(pop(), pop()));
    } else if (token == "sqrt") {
      push(StackLang::sqrt(pop()));
    } else if (token == "%") {
      push(StackLang::mod(pop(), pop()));
    } else if (token == "print") {
      push(StackLang::print(logger, pop()));
    } else if (token == "printstr") {
      print_string();
    } else if (token == "flush") {
      flush();
    } else if (token == "dup") {
      dup();
    } else if (token == "nop") {
      // No op.
    } else if (token == "pop") {
      pop();
    } else if (token == "dec") {
      dec();
    } else if (token == "inc") {
      inc();
    } else if (token == "swap") {
      swap();
    } else {
      auto fn_it = functions.find(token);
      if (fn_it != functions.end()) {
        fn_stack.push_back({token, 0});
      } else {
        cerr << "Unknown command." << endl;
        abort();
      }
    }
  }
};

value_t Interpreter::pop() {
  if (stack.empty()) {
    cerr << "Empty stack cannot pop." << endl;
    abort();
  }

  value_t val{stack.back()};
  stack.pop_back();

  logger.debug("POP <%f> from stack.\n", val);

  return val;
};

void Interpreter::push(value_t val) {
  stack.emplace_back(val);
  logger.debug("PUSH <%f> to stack.\n", val);
};

value_t Interpreter::top() {
  if (stack.empty()) {
    cerr << "Empty stack has no top." << endl;
    abort();
  }

  return stack.back();
}

void Interpreter::flush() {
  stack.clear();
  logger.debug("FLUSH%c", '\n');
};

void Interpreter::dup() { push(stack.back()); };
void Interpreter::inc() { stack.back()++; };
void Interpreter::dec() { stack.back()--; };
void Interpreter::swap() {
  value_t first{pop()};
  value_t second(pop());
  push(first);
  push(second);
};

void Interpreter::print_string() {
  uint8_t len = static_cast<uint8_t>(pop());
  logger.info("\x1B[107m\x1B[30m\x1B[1m  %c", '\0');
  for (uint8_t i = 0; i < len; i++) {
    char c{static_cast<char>(pop())};
    logger.info("%c", c);
  }
  logger.info("  \x1B[0m\n%c", '\0');
}

optional<string> Interpreter::next_token() {
  if (!fn_stack.empty()) {
    string token = functions[fn_stack.back().first][fn_stack.back().second];

    fn_stack.back().second++;
    if (fn_stack.back().second >= functions[fn_stack.back().first].size()) {
      fn_stack.pop_back();
    }

    return {token};
  }

  if (isit != istream_iterator<string>()) {
    string token{*isit};
    isit++;
    return {token};
  }

  return {};
}

void Interpreter::read_function() {
  string name = next_token().value();
  vector<string> tokens{};
  for (optional<string> token_opt = next_token();
       token_opt.has_value() && token_opt.value() != "endfn";
       token_opt = next_token()) {
    tokens.emplace_back(token_opt.value());
  }
  functions[name] = tokens;
}
