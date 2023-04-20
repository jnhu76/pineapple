#ifndef LEXER_H_
#define LEXER_H_
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

    if (isalpha(LastChar)) {
        IdentifierStr = LastChar;
        while (isalnum(LastChar = getchar())) {
            IdentifierStr += LastChar;
        }
    }
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

#endif 