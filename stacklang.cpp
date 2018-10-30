#include <cmath>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "interpreter.h"
#include "logger.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    cerr << "Missing file name.\n";
  }
  ifstream ifs(argv[1]);

  LoggerLevel log_level{LoggerLevel::PROD};
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      log_level = LoggerLevel::DEBUG;
    }
  }
  Logger logger{log_level};

  Interpreter(istream_iterator<string>(ifs), logger).interpret();
}
