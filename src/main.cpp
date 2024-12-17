#include "pineapple.h"
#include <string_view>

int main(int argc, char **argv) {

  if (argc <= 1) {
    std::cerr << "File not found.\n";
    exit(1);
  }

  std::string_view file = argv[1];

  auto data = read_file(file);

  Interpreter i(data);

  i.execute();

  return 0;
}
