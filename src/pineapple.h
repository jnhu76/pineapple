#ifndef PINEAPPLE_H_
#define PINEAPPLE_H_

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

namespace PineApple {

// -----------------------------------------------
// ------------------- Utils ---------------------
// -----------------------------------------------

auto read_file(std::string_view path) -> std::string {
  constexpr auto read_size = std::size_t(4096);
  auto stream = std::ifstream(path.data());
  stream.exceptions(std::ios_base::badbit);

  if (not stream) {
    throw std::ios_base::failure("file does not exist");
  }

  auto out = std::string();
  auto buf = std::string(read_size, '\0');
  while (stream.read(&buf[0], read_size)) {
    out.append(buf, 0, stream.gcount());
  }
  out.append(buf, 0, stream.gcount());
  return out;
}

class InterpreterError : public std::runtime_error {
public:
  explicit InterpreterError(const std::string &message)
      : std::runtime_error(message) {}
};

// -----------------------------------------------
// ------------------- Defintion -----------------
// -----------------------------------------------
using Variable = std::tuple<std::size_t, std::string>;

struct Assignment {
  std::size_t line_num;
  std::string name;
  Variable variable;
};

struct Print {
  std::size_t line_num;
  Variable variable;
};

using Statement = std::variant<Assignment, Print>;

struct SourceCode {
  std::size_t line_num{0};
  std::vector<Statement> statements;

  SourceCode() = default;
  explicit SourceCode(std::size_t line_num,
                      std::vector<Statement> statements = {})
      : line_num(line_num), statements(std::move(statements)) {}

  void add_statement(Statement stmt) { statements.push_back(std::move(stmt)); }
};

// -----------------------------------------------
// ------------------- Lexer ---------------------
// -----------------------------------------------

enum class TokenType : std::uint16_t {
  TOKEN_EOF = 0,
  TOKEN_VAR_PREFIX = 1,
  TOKEN_LEFT_PAREN = 2,
  TOKEN_RIGHT_PAREN = 3,
  TOKEN_EQUAL = 4,
  TOKEN_QUOTE = 5,
  TOKEN_DUOQUOTE = 6,
  TOKEN_NAME = 7,
  TOKEN_PRINT = 8,
  TOKEN_IGNORED = 9,
};

// https://stackoverflow.com/a/28999011
namespace token_str {
constexpr const char *TOKEN_EOF = "TOKEN_EOF";
constexpr const char *TOKEN_VAR_PREFIX = "TOKEN_VAR_PREFIX";
constexpr const char *TOKEN_LEFT_PAREN = "TOKEN_LEFT_PAREN";
constexpr const char *TOKEN_RIGHT_PAREN = "TOKEN_RIGHT_PAREN";
constexpr const char *TOKEN_EQUAL = "TOKEN_EQUAL";
constexpr const char *TOKEN_QUOTE = "TOKEN_QUOTE";
constexpr const char *TOKEN_DUOQUOTE = "TOKEN_DUOQUOTE";
constexpr const char *TOKEN_NAME = "TOKEN_NAME";
constexpr const char *TOKEN_PRINT = "TOKEN_PRINT";
constexpr const char *TOKEN_IGNORED = "TOKEN_IGNORED";
} // namespace token_str

std::ostream &operator<<(std::ostream &os, TokenType token_type) {
  switch (token_type) {
  case TokenType::TOKEN_EOF:
    return os << token_str::TOKEN_EOF;
  case TokenType::TOKEN_VAR_PREFIX:
    return os << token_str::TOKEN_VAR_PREFIX;
  case TokenType::TOKEN_LEFT_PAREN:
    return os << token_str::TOKEN_LEFT_PAREN;
  case TokenType::TOKEN_RIGHT_PAREN:
    return os << token_str::TOKEN_RIGHT_PAREN;
  case TokenType::TOKEN_EQUAL:
    return os << token_str::TOKEN_EQUAL;
  case TokenType::TOKEN_QUOTE:
    return os << token_str::TOKEN_QUOTE;
  case TokenType::TOKEN_DUOQUOTE:
    return os << token_str::TOKEN_DUOQUOTE;
  case TokenType::TOKEN_NAME:
    return os << token_str::TOKEN_NAME;
  case TokenType::TOKEN_PRINT:
    return os << token_str::TOKEN_PRINT;
  case TokenType::TOKEN_IGNORED:
    return os << token_str::TOKEN_IGNORED;
  default:
    return os << static_cast<std::uint16_t>(token_type);
  }
}

std::string to_string(TokenType token_type) {
  switch (token_type) {
  case TokenType::TOKEN_EOF:
    return token_str::TOKEN_EOF;
  case TokenType::TOKEN_VAR_PREFIX:
    return token_str::TOKEN_VAR_PREFIX;
  case TokenType::TOKEN_LEFT_PAREN:
    return token_str::TOKEN_LEFT_PAREN;
  case TokenType::TOKEN_RIGHT_PAREN:
    return token_str::TOKEN_RIGHT_PAREN;
  case TokenType::TOKEN_EQUAL:
    return token_str::TOKEN_EQUAL;
  case TokenType::TOKEN_QUOTE:
    return token_str::TOKEN_QUOTE;
  case TokenType::TOKEN_DUOQUOTE:
    return token_str::TOKEN_DUOQUOTE;
  case TokenType::TOKEN_NAME:
    return token_str::TOKEN_NAME;
  case TokenType::TOKEN_PRINT:
    return token_str::TOKEN_PRINT;
  case TokenType::TOKEN_IGNORED:
    return token_str::TOKEN_IGNORED;
  default:
    return std::to_string(static_cast<std::uint16_t>(token_type));
  }
}

std::unordered_map<std::string, TokenType> KEYWORDS{{"print", TokenType::TOKEN_PRINT}};

using TokenInfo = std::tuple<std::size_t, TokenType, std::string>;

class Lexer {
public:
  Lexer() = default;
  Lexer(const std::string &source_code) : source_code(source_code) {}

  bool next_source_code_is(const std::string &prefix);
  std::size_t get_line_num() const { return line_num; }
  std::size_t get_currnet() const { return current; }
  bool finished() { return current >= source_code.length(); }

  std::string scan_pattern(const std::regex pattern);
  std::string scan_name();
  std::string scan_ignored();

  std::string scan_before_token(const std::string &token);
  void process_new_line(const std::string &ignored);
  TokenInfo get_next_token();
  TokenInfo next_token_is(TokenType guess);
  TokenType look_ahead();

private:
  std::string source_code;
  std::size_t current{0};
  std::size_t line_num{1};
  std::optional<TokenInfo> next_token_info{std::nullopt};
};

bool Lexer::next_source_code_is(const std::string &prefix) {
  auto str = source_code.substr(current);
  return str.starts_with(prefix);
}

std::string Lexer::scan_pattern(const std::regex pattern) {
  std::smatch base_match;
  auto str = source_code.substr(current);
  if (std::regex_search(str, base_match, pattern))
    if (base_match.length() >= 1)
      return base_match[0];

  throw InterpreterError("Regex Error: " + str);
}

std::string Lexer::scan_name() {
  const auto reg = std::regex("^[_a-zA-Z][_a-zA-Z0-9]*");
  return scan_pattern(reg);
}

std::string Lexer::scan_ignored() {
  const auto reg = std::regex("^^[\t\n\v\f\r ]+");
  return scan_pattern(reg);
}

std::string Lexer::scan_before_token(const std::string &token) {
  auto str = source_code.substr(current);
  auto found = str.find(token);
  if (found == std::string::npos) {
    throw InterpreterError("Scan Token Error: " + token + " not found.");
  }
  auto res = str.substr(0, found);
  current += res.length();
  process_new_line(res);
  return res;
}

void Lexer::process_new_line(const std::string &ignored) {
  std::size_t i{0};
  while (i <= ignored.length()) {
    auto tmp_str = ignored.substr(i, 2);
    if (tmp_str == "\r\n" || tmp_str == "\n\r") {
      i += 2;
      line_num++;
    } else if (ignored[i] == '\r' || ignored[i] == '\n') {
      i++;
      line_num++;
    } else {
      i++;
    }
  }
}

TokenInfo Lexer::get_next_token() {
  if (next_token_info.has_value()) {
    auto token_info = next_token_info.value();
    next_token_info = std::nullopt;
    return token_info;
  }

  if (finished()) {
    return std::make_tuple(line_num, TokenType::TOKEN_EOF, "EOF");
  }

  auto next_chr = source_code[current];

  switch (next_chr) {
  case '$':
    current += 1;
    return std::make_tuple(line_num, TokenType::TOKEN_VAR_PREFIX, "$");
  case '(':
    current += 1;
    return std::make_tuple(line_num, TokenType::TOKEN_LEFT_PAREN, "(");
  case ')':
    current += 1;
    return std::make_tuple(line_num, TokenType::TOKEN_RIGHT_PAREN, ")");
  case '=':
    current += 1;
    return std::make_tuple(line_num, TokenType::TOKEN_EQUAL, "=");
  case '"':
    if (next_source_code_is("\"\"")) {
      current += 2;
      return std::make_tuple(line_num, TokenType::TOKEN_DUOQUOTE, "\"\"");
    }
    current += 1;
    return std::make_tuple(line_num, TokenType::TOKEN_QUOTE, "\"");
  }

  if (next_chr == '_' || isalpha(next_chr)) {
    auto name = scan_name();
    if (KEYWORDS.find(name) != KEYWORDS.end()) {
      current += name.length();
      return std::make_tuple(line_num, KEYWORDS[name], name);
    } else {
      current += name.length();
      return std::make_tuple(line_num, TokenType::TOKEN_NAME, name);
    }
  }

  if (isspace(next_chr)) {
    auto ignore_str = scan_ignored();
    current += ignore_str.length();
    process_new_line(ignore_str);
    return std::make_tuple(line_num, TokenType::TOKEN_IGNORED, ignore_str);
  }
  throw InterpreterError("'get_next_token()' unexpected symbol " +
                         std::string(1, next_chr));
}

TokenInfo Lexer::next_token_is(TokenType guess) {
  std::string token;
  TokenType token_type;
  std::size_t line_num;

  auto _next_token_info = get_next_token();
  std::tie(line_num, token_type, token) = _next_token_info;
  if (token_type != guess) {
    throw InterpreterError("'next_token_is()' syntax error near " + token +
                           ", expecting " + to_string(guess) + " but got [" +
                           std::to_string(line_num) + ", " +
                           to_string(token_type) + ", " + token + "].");
  }
  return _next_token_info;
}

TokenType Lexer::look_ahead() {
  if (not next_token_info.has_value())
    next_token_info = get_next_token();
  return std::get<TokenType>(next_token_info.value());
}

// ------------------- Parser ---------------------

void parse_ignored(Lexer &lexer) {
  if (lexer.look_ahead() == TokenType::TOKEN_IGNORED)
    lexer.next_token_is(TokenType::TOKEN_IGNORED);
}

Variable parse_variable(Lexer &lexer) {
  auto next_token_info = lexer.next_token_is(TokenType::TOKEN_VAR_PREFIX);
  std::size_t line_num = std::get<std::size_t>(next_token_info);
  next_token_info = lexer.next_token_is(TokenType::TOKEN_NAME);
  std::string name = std::get<std::string>(next_token_info);

  parse_ignored(lexer);
  return std::make_tuple(line_num, std::move(name));
}

std::string parse_string(Lexer &lexer) {
  if (lexer.look_ahead() == TokenType::TOKEN_DUOQUOTE) {
    lexer.next_token_is(TokenType::TOKEN_DUOQUOTE);
    return "";
  }

  lexer.next_token_is(TokenType::TOKEN_QUOTE);
  auto str = lexer.scan_before_token("\"");
  lexer.next_token_is(TokenType::TOKEN_QUOTE);
  return str;
}

Statement parse_assignment(Lexer &lexer) {
  auto var = parse_variable(lexer);
  parse_ignored(lexer);
  lexer.next_token_is(TokenType::TOKEN_EQUAL);
  parse_ignored(lexer);
  auto str = parse_string(lexer);
  parse_ignored(lexer);

  return Assignment{std::get<0>(var), std::move(str), std::move(var)};
}

Statement parse_print(Lexer &lexer) {
  auto line_num = std::get<0>(lexer.next_token_is(TokenType::TOKEN_PRINT));
  lexer.next_token_is(TokenType::TOKEN_LEFT_PAREN);
  parse_ignored(lexer);
  auto variable = parse_variable(lexer);
  lexer.next_token_is(TokenType::TOKEN_RIGHT_PAREN);
  parse_ignored(lexer);

  return Print{line_num, std::move(variable)};
}

Statement parse_statement(Lexer &lexer) {
  switch (lexer.look_ahead()) {
  case TokenType::TOKEN_PRINT:
    return parse_print(lexer);
  case TokenType::TOKEN_VAR_PREFIX:
    return parse_assignment(lexer);
  default:
    throw InterpreterError("Statement type error.");
  }
}

SourceCode parse(Lexer &lexer) {
  SourceCode sc;

  sc.line_num = lexer.get_line_num();

  auto next_type = lexer.look_ahead();

  while (next_type != TokenType::TOKEN_EOF) {
    if (next_type == TokenType::TOKEN_PRINT ||
        next_type == TokenType::TOKEN_VAR_PREFIX)
      sc.statements.emplace_back(parse_statement(lexer));
    next_type = lexer.look_ahead();
  }
  return sc;
}

// ------------------- Backend ---------------------

// 帮助实现 visitor 模式的工具类
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Interpreter {
public:
  Interpreter() = default;
  Interpreter(const std::string &source_code) : lexer(Lexer(source_code)) {
    ast = parse(lexer);
  }

  void execute() {
    try {
      for (const auto &stmt : ast.statements) {
        std::visit(*this, stmt);
      }
    } catch (const InterpreterError &e) {
      std::cerr << "Interpreter Error: " << e.what() << std::endl;
      std::exit(1);
    } catch (const std::exception &e) {
      std::cerr << "Unexpected error: " << e.what() << std::endl;
      std::exit(1);
    }
  }

  void operator()(const Print &p) {
    auto name = std::get<std::string>(p.variable);
    std::cout << variables[name];
  }

  void operator()(const Assignment &a) {
    auto name = std::get<std::string>(a.variable);
    variables[name] = a.name;
  }

private:
  Lexer lexer;
  SourceCode ast;
  std::unordered_map<std::string, std::string> variables{};
};

inline void run(std::string_view filename) {
  auto data = read_file(filename);
  Interpreter(data).execute();
}

} // namespace PineApple
#endif // !PINEAPPLE_H_