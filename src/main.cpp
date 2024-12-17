#include "pineapple.h"
#include <string_view>

int main(int argc, char **argv) {

  if (argc <= 1) {
    std::cerr << "File not found.\n";
    exit(1);
  }

  PineApple::run(argv[1]);

  return 0;
}
