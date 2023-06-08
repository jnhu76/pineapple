#ifndef PINEAPPLE_H_
#define PINEAPPLE_H_


#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <type_traits>
#include <string_view>

// C++元编程之Parser Combinator - netcan的文章 - 知乎
// https://zhuanlan.zhihu.com/p/249005405
// https://compile-time-regular-expressions.readthedocs.io/en/latest/index.html

// Parser a :: String -> Maybe(a, String)
using ParserInput = std::string_view;
template<typename T>
using ParserResult = std::optional<std::pair<T, ParserInput>>;
template<typename T>
using Parser_t = auto(*)(ParserInput) -> ParserResult<T>;  // ?

constexpr auto makeCharParser(char c) {
    // CharParser :: Parser Char
    return [=](ParserInput s) -> ParserResult<char> {
        if (s.empty() || c != s[0]) return std::nullopt;
        return std::make_pair(s[0], ParserInput(s.begin() + 1, s.size() - 1));
    };
};

constexpr auto oneOf(std::string_view chars) {
    // OneOf :: Parser Char
    return [=](ParserInput s) -> ParserResult<char> {
        if (s.empty() || chars.find(s[0]) == std::string::npos) return std::nullopt;
        return std::make_pair(s[0], ParserInput(s.begin() + 1, s.size() - 1));
    };
};

// combine :: Parser a -> Parser b -> (a -> b -> c) -> Parser c
template<typename P1, typename P2, typename F,
    typename R = std::invoke_result_t<F, Parser_t<P1>, Parser_t<P2>>>
constexpr auto combine(P1&& p1, P2&& p2, F&& f) {
    return [=](ParserInput s) -> ParserResult<R> {
        auto r1 = p1(s);
        if !(r1) return std::nullopt;
        auto r2 = p2(r1->second);
        if (!r2) return std::nullopt;
        return std::make_pair(f(r1->first, r2->first), r2->second);
    };
};

// operator> :: Parser a -> Parser b -> Parser a
template<typename P1, typename P2>
constexpr auto operator>(P1&& p1, P2&& p2) {
    return combine(std::forward<P1>(p1),
                   std::forward<P2>(p2),
                   [](auto&& l, auto) { return l; });
};


#endif // PINEAPPLE_H_