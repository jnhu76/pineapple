#ifndef LEXER_H_
#define LEXER_H_
#include <string>
#include <map>

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

std::map<std::string, Token> key_words = {{"print", tok_print}};


class Lexer {
public:
    Lexer(const std::string& source_code): source_code(source_code), line_num(1), cur(0) {}
    ~Lexer() {}

    Token lookAhead();

    void lookAheadAndSkip(Token token);

    bool isLetter(char ch) { return std::isalpha(static_cast<unsigned char>(ch)); }
    bool isNewLine(char ch) { return ch == '\r' || ch == '\n'; }
    bool isWhiteSpace(char ch) { return ch =='\t' || ch == '\n' || ch == '\f' || ch == ' '; }
    bool isIgnored();

private:
    size_t line_num;
    std::string source_code;
    size_t cur;
};

#endif 