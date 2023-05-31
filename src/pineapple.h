// CMakeProject1.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <map>
#include <tuple>
#include <regex>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <optional>
#include <iostream>


// ------------------- Defintion ---------------------

using Variable = std::tuple<std::size_t, std::string>;


struct Statement {
public:
	Statement() = default;
	Statement(std::size_t line_num, const std::string& name, Variable variable, std::size_t statement_type = 0)
		: line_num(line_num), name(name), statement_type(statement_type), variable(std::move(variable)) {}

	std::size_t id() const { return statement_type; }

	friend auto operator<=>(const Statement&, const Statement&) = default;

	std::size_t statement_type{ 0 }; // 0 -> Assignment, 1 -> Print;
	std::size_t line_num{1};
	std::string name{""};
	Variable variable{0, ""};
};


class SourceCode {
public:
	SourceCode() = default;
	SourceCode(std::size_t line_num, std::vector<Statement>& statements)
		: line_num(line_num), statements(statements) {}

	void set_line_num(std::size_t _line_num) { line_num = _line_num; }
	std::size_t get_line_num() const { return line_num; }

	void add_statement(Statement statement) {
		statements.emplace_back(std::move(statement));
	}

	std::vector<Statement> get_statements() {
		return statements;
	}

private:
	std::size_t line_num{ 0 };
	std::vector<Statement> statements;
};

// ------------------- Lexer ---------------------

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
std::ostream& operator<<(std::ostream& os, TokenType token_type) {
	switch (token_type)
	{
	case TokenType::TOKEN_EOF:
		return os << "TOKEN_EOF";
	case TokenType::TOKEN_VAR_PREFIX:
		return os << "TOKEN_VAR_PREFIX";
	case TokenType::TOKEN_LEFT_PAREN:
		return os << "TOKEN_LEFT_PAREN";
	case TokenType::TOKEN_RIGHT_PAREN:
		return os << "TOKEN_RIGHT_PAREN";
	case TokenType::TOKEN_EQUAL:
		return os << "TOKEN_EQUAL";
	case TokenType::TOKEN_QUOTE:
		return os << "TOKEN_QUOTE";
	case TokenType::TOKEN_DUOQUOTE:
		return os << "TOKEN_DUOQUOTE";
	case TokenType::TOKEN_NAME:
		return os << "TOKEN_NAME";
	case TokenType::TOKEN_PRINT:
		return os << "TOKEN_PRINT";
	case TokenType::TOKEN_IGNORED:
		return os << "TOKEN_IGNORED";
	default:
		return os << static_cast<std::uint16_t>(token_type);
	}
}

std::map<std::string, TokenType> KEYWORDS {{"print", TokenType::TOKEN_PRINT} };


using TokenInfo = std::tuple<std::size_t, TokenType, std::string>;



class Lexer {
public:
	Lexer() = default;
	Lexer(const std::string& source_code)
		: source_code(source_code) {}

	bool next_source_code_is(const std::string& prefix);
	std::size_t get_line_num() const { return line_num; }
	bool finished() { return current >= source_code.length(); }

	std::string scan_pattern(const std::regex pattern);
	std::string scan_name();
	std::string scan_ignored();

	std::string scan_before_token(const std::string& token);
	void process_new_line(const std::string& ignored);
	TokenInfo get_next_token();
	TokenInfo next_token_is(TokenType guess);
	TokenType look_ahead();


private:
	std::string source_code;
	std::size_t current{0};
	std::size_t line_num {1};
	std::optional<TokenInfo> next_token_info { std::nullopt };
};

bool Lexer::next_source_code_is(const std::string& prefix) {
	auto str = source_code.substr(current);
	return str.starts_with(prefix);
}

std::string Lexer::scan_pattern(const std::regex pattern) {
	std::smatch base_match;
	auto str = source_code.substr(current);
	if (std::regex_search(str, base_match, pattern))
		if (base_match.length() >= 1)
			return base_match[0];
	std::cerr << "Regex Error\n";
	exit(1);
}

std::string Lexer::scan_name() {
	const auto reg = std::regex("^[_a-zA-Z][_a-zA-Z0-9]*");
	return scan_pattern(reg);
}

std::string Lexer::scan_ignored() {
	const auto reg = std::regex("^^[\t\n\v\f\r ]+");
	//const auto reg = std::regex("\s+");
	return scan_pattern(reg);
}

std::string Lexer::scan_before_token(const std::string& token) {
	auto str = source_code.substr(current);
	auto found = str.find(token);
	if (found == std::string::npos) {
		std::cerr << "Scan Token Error: Not Found\n";
		exit(1);
	}
	auto res = str.substr(0, found);
	current += res.length();
	process_new_line(res);
	return res;
}

void Lexer::process_new_line(const std::string& ignored) {
	auto i{ 0 };
	while (i <= ignored.length()) {
		auto tmp_str = ignored.substr(i, 2);
		if (tmp_str == "\r\n" || tmp_str == "\n\r") {
			i += 2;
			line_num++;
		}
		else if (ignored[i] == '\r' || ignored[i] == '\n') {
			i++;
			line_num++;
		}
		else {
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

	switch (next_chr)
	{
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
		}
		else {
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

	std::cerr << "get_next_token(): unexpected symbol " << next_chr << "\n";
	exit(1);
}

TokenInfo Lexer::next_token_is(TokenType guess) {
	std::string token;
	TokenType token_type;
	std::size_t line_num;

	auto _next_token_info = get_next_token();
	std::tie(line_num, token_type, token) = _next_token_info;
	if (token_type != guess) {
		std::cerr << "next_token_is(): syntax error near " << token <<
			", expecting " << guess <<
			" but got [" << line_num <<
			", " << token_type <<
			"," << token << "]\n";
		exit(1);
	}
	return _next_token_info;
}

TokenType Lexer::look_ahead() {
	if (not next_token_info.has_value())
		next_token_info = get_next_token();
	return std::get<TokenType>(next_token_info.value());
}


// ------------------- Parser ---------------------

void parse_ignored(Lexer& lexer) {
	if (lexer.look_ahead() == TokenType::TOKEN_IGNORED)
		lexer.next_token_is(TokenType::TOKEN_IGNORED);
}


Variable parse_variable(Lexer& lexer) {
	auto next_token_info = lexer.next_token_is(TokenType::TOKEN_VAR_PREFIX);
	std::size_t line_num = std::get<std::size_t>(next_token_info);
	next_token_info = lexer.next_token_is(TokenType::TOKEN_NAME);
	std::string name = std::get<std::string>(next_token_info);

	parse_ignored(lexer);
	return std::make_tuple(line_num, name);
}


std::string parse_string(Lexer& lexer) {
	if (lexer.look_ahead() == TokenType::TOKEN_DUOQUOTE) {
		lexer.next_token_is(TokenType::TOKEN_DUOQUOTE);
		return "";
	}

	lexer.next_token_is(TokenType::TOKEN_QUOTE);
	auto str = lexer.scan_before_token("\"");
	lexer.next_token_is(TokenType::TOKEN_QUOTE);
	return str;
}

Statement parse_statement(Lexer& lexer) {
	Statement statement;

	auto is_print = lexer.look_ahead() == TokenType::TOKEN_PRINT ? true : false;
	if (is_print) {
		statement.statement_type = 1;
		statement.line_num = std::get<std::size_t>(lexer.next_token_is(TokenType::TOKEN_PRINT));
		lexer.next_token_is(TokenType::TOKEN_LEFT_PAREN);
		parse_ignored(lexer);
		statement.variable = parse_variable(lexer);
		parse_ignored(lexer);
		lexer.next_token_is(TokenType::TOKEN_RIGHT_PAREN);
		parse_ignored(lexer);
	}
	else {
		statement.statement_type = 0;
		statement.variable = parse_variable(lexer);
		parse_ignored(lexer);
		lexer.next_token_is(TokenType::TOKEN_EQUAL);
		parse_ignored(lexer);
		statement.name = parse_string(lexer);
		parse_ignored(lexer);
		statement.line_num = std::get<std::size_t>(statement.variable);
	}
	return statement;
}

SourceCode parse(Lexer& lexer) {
	SourceCode sc;

	sc.set_line_num(lexer.get_line_num());
	auto next_type = lexer.look_ahead();
	while (next_type != TokenType::TOKEN_EOF) {
		if (next_type == TokenType::TOKEN_PRINT || next_type == TokenType::TOKEN_VAR_PREFIX)
			sc.add_statement(parse_statement(lexer));
		next_type = lexer.look_ahead();
	}
	return sc;
}

// ------------------- Backend ---------------------

class Interpreter {
public:
	Interpreter() = default;
	Interpreter(const std::string& source_code) : lexer(Lexer(source_code)) {
		ast = parse(lexer);
	}

	void resolve_print(Statement& print) {
		auto name = std::get<std::string>(print.variable);
		std::cout << variables[name];
	}

	void resolve_assigment(Statement& assignment) {
		auto name = std::get<std::string>(assignment.variable);
		variables[name] = assignment.name;
	}

	void execute() {
		for (auto& elem : ast.get_statements()) {
			if (elem.id() == 1)
				resolve_print(elem);
			else
				resolve_assigment(elem);
		}
	}

private:
	Lexer lexer;
	SourceCode ast;
	std::map<std::string, std::string> variables{};
};

// ------------------- Utils ---------------------
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
