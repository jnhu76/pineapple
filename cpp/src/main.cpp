#include <iostream>
#include <fstream>
#include <string>

// ---------------------- Lexer ---------------------
enum Token {
    tok_eof = -1,           // EOF
    tok_var_prefix = -2,    // $
    tok_left_paren = -3,    // (
    tok_right_paren = -4,   // )
    tok_equal = -5,         // =
    tok_quote = -6,         // "
    tok_duoquote = -7,      // ""
    tok_name = -8,          // Name ::= [_A-Za-z][_0-9A-Za-z]
    tok_print = -9,         // print
    tok_ignored = -10,      // ignored
};

static std::string IdentifierStr;
static double NumVal;


static int gettok() {
    static int LastChar = ' ';

    // Skip any space.
    while (isspace(LastChar))
        LastChar = getchar();
}


class Lexer {
public:
    Lexer(std::string& source_code): source_code(source_code), line_num(0), cur(0) {}
    ~Lexer() {}
private:
    size_t line_num;
    std::string source_code;
    size_t cur;
};


// file to string.
// https://stackoverflow.com/a/116220
auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

int main(int argc, char** argv)
{
    std::cout << "hello world!\n";
    auto data = read_file("1.txt");
    std::cout << data << std::endl;
    return 0;
}
