#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include <iostream>


int main(int argc, char** argv) {
    std::cout << "hello world!\n";
    auto data = read_file("1.txt");
    std::cout << data << std::endl;
    return 0;
}
