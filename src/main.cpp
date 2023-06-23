#include "pineapple.h"
#include <string_view>

int
main(int argc, char** argv)
{

  // auto s = std::string("$a = \"pen pineapple\n\rapple pen.\"\nprint($a)\r$a =
  // \"another test 1123\" $a = \"overwrite test\n\rwith new line\" print( $a
  // )");

  if (argc <= 1) {
    std::cerr << "File not found.\n";
    exit(1);
  }

  std::string_view file = argv[1];

  auto data = read_file(file);

  Interpreter i(data);
  // Interpreter i(s);
  i.execute();

  return 0;
}
