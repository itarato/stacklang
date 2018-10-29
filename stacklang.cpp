#include <cmath>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
using value_t = double;

template <typename T>
void debug_vec(vector<T> v) {
  size_t i{0};
  for (const auto &e : v) {
    cout << '#' << i++ << ": '" << e << "'" << endl;
  }
}

enum IfState {
  Exec,
  Wait,
};

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

value_t print(value_t lhs) {
  cout << "\x1B[107m\x1B[30m\x1B[1m  " << lhs << "  \x1B[0m" << endl;
  return lhs;
}
}  // namespace StackLang

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

struct Interpreter {
  Tokenizer tokenizer;

  Interpreter(string &&source)
      : tokenizer({move(source)}),
        if_states({}),
        if_wait_skip_len(0),
        fn_stack({}) {
    tokens = tokenizer.tokenize();
    tokens_it = tokens.begin();

    cout << "\n\t---\n\n";
  };

  void interpret() {
    const regex num_regex("^[\\-+]?\\d+(\\.\\d+|)$");
    for (optional<string> token_opt = next_token(); token_opt.has_value();
         token_opt = next_token()) {
      const string token = token_opt.value();
      cout << "Token read: '" << token << '\'' << endl;

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
        push(StackLang::print(pop()));
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
          // Just use only the fn_stack and check the top.
          if (current_fn_key.has_value()) {
            fn_stack.push_back({current_fn_key.value(), current_fn_ip});
          }

          current_fn_key = token;
          current_fn_ip = 0;
        } else {
          cerr << "Unknown command." << endl;
          abort();
        }
      }
    }
  };

 private:
  vector<string> tokens;
  vector<string>::iterator tokens_it;
  optional<string> current_fn_key{};
  uint8_t current_fn_ip{0};
  vector<value_t> stack;
  vector<IfState> if_states;
  uint8_t if_wait_skip_len;
  unordered_map<string, vector<string>> functions;
  vector<pair<string, uint8_t>> fn_stack;

  value_t pop() {
    if (stack.empty()) {
      cerr << "Empty stack cannot pop." << endl;
      abort();
    }

    value_t val{stack.back()};
    stack.pop_back();

    cout << "POP <" << val << "> from stack." << endl;

    return val;
  };

  void push(value_t val) {
    stack.emplace_back(val);
    cout << "PUSH <" << val << "> to stack." << endl;
  };

  value_t top() {
    if (stack.empty()) {
      cerr << "Empty stack has no top." << endl;
      abort();
    }

    return stack.back();
  }

  void flush() {
    stack.clear();
    cout << "FLUSH" << endl;
  };

  void dup() { push(stack.back()); };
  void inc() { stack.back()++; };
  void dec() { stack.back()--; };
  void swap() {
    value_t first{pop()};
    value_t second(pop());
    push(first);
    push(second);
  };

  void print_string() {
    uint8_t len = static_cast<uint8_t>(pop());
    cout << "\x1B[107m\x1B[30m\x1B[1m  ";
    for (uint8_t i = 0; i < len; i++) {
      char c{static_cast<char>(pop())};
      cout << c;
    }
    cout << "  \x1B[0m\n";
  }

  optional<string> next_token() {
    if (current_fn_key.has_value()) {
      string token = functions[current_fn_key.value()][current_fn_ip];

      current_fn_ip++;
      if (current_fn_ip >= functions[current_fn_key.value()].size()) {
        current_fn_key = {};
      }

      return {token};
    }

    if (tokens_it != tokens.end()) {
      string token = *tokens_it;
      tokens_it++;
      return {token};
    }

    return {};
  }

  void read_function() {
    string name = next_token().value();
    vector<string> tokens{};
    for (optional<string> token_opt = next_token();
         token_opt.has_value() && token_opt.value() != "endfn";
         token_opt = next_token()) {
      tokens.emplace_back(token_opt.value());
    }
    functions[name] = tokens;
  }
};

int main() {
  // Interpreter("1 print").interpret();
  // Interpreter("fn add + print endfn 2 5 add").interpret();
  // Interpreter(
  //     "0 if 1 print else 2 print flush 0 if 3 print else 4 print endif print
  //     " "endif") .interpret();
  // Interpreter("10 dec dec print").interpret();
  // Interpreter("fn loop if dec 100 + print 100 swap - loop endif endfn 10
  // loop")
  //     .interpret();
  // Interpreter("10 dup pop pop pop").interpret();
  Interpreter(
      "fn fizz 90 90 73 70 4 printstr endfn fn loop if dup 5 swap % if pop "
      "else fizz pop "
      "endif print dec "
      "loop endif endfn 100 loop")
      .interpret();
}
