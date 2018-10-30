#include <cmath>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "interpreter.h"
#include "logger.h"

using namespace std;

int main(int argc, char* argv[]) {
  LoggerLevel log_level{LoggerLevel::PROD};
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      log_level = LoggerLevel::DEBUG;
    }
  }
  Logger logger{log_level};

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
      "fn fizz 90 90 73 70 4 printstr endfn fn loop if dup 5 % if pop "
      "else fizz pop "
      "endif print dec "
      "loop endif endfn 100 loop",
      logger)
      .interpret();
}
