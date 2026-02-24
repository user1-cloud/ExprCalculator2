#include "lexer.h"

// 词法分析器
Lexer::Lexer(const std::string& src) : source(src) {}

Token Lexer::nextToken() {
    while (pos < source.size() && std::isspace(source[pos])) pos++;
    if (pos >= source.size()) return { TokenType::END, "" };

    char c = source[pos];
    // 处理数字（含小数）
    if (std::isdigit(c) || c == '.') {
        size_t start = pos;
        bool has_dot = false;
        while (pos < source.size() && (std::isdigit(source[pos]) || source[pos] == '.')) {
            if (source[pos] == '.') {
                if (has_dot) break;
                has_dot = true;
            }
            pos++;
        }
        std::string num_str = source.substr(start, pos - start);
        try {
            double val = std::stod(num_str);
            return { TokenType::NUMBER, num_str, val };
        }
        catch (...) {
            return { TokenType::ERROR, "Invalid number" };
        }
    }
    // 标识符/关键字
    if (std::isalpha(c) || c == '_') {
        size_t start = pos;
        while (pos < source.size() && (std::isalnum(source[pos]) || source[pos] == '_')) pos++;
        std::string id = source.substr(start, pos - start);
        return { TokenType::IDENTIFIER, id };
    }
    // 运算符（按长度优先匹配）
    pos++;
    switch (c) {
    case '+': return { TokenType::PLUS, "+" };
    case '-': return { TokenType::MINUS, "-" };
    case '*':
        if (pos < source.size() && source[pos] == '*') { pos++; return { TokenType::POW, "**" }; }
        return { TokenType::STAR, "*" };
    case '/': return { TokenType::SLASH, "/" };
    case '%': return { TokenType::MOD, "%" };
    case '^': return { TokenType::POW, "^" };
    case '(': return { TokenType::LPAREN, "(" };
    case ')': return { TokenType::RPAREN, ")" };
    case '=':
        if (pos < source.size() && source[pos] == '=') { pos++; return { TokenType::EQ, "==" }; }
        return { TokenType::ASSIGN, "=" };
    case '>':
        if (pos < source.size() && source[pos] == '=') { pos++; return { TokenType::GE, ">=" }; }
        return { TokenType::GT, ">" };
    case '<':
        if (pos < source.size() && source[pos] == '=') { pos++; return { TokenType::LE, "<=" }; }
        return { TokenType::LT, "<" };
    case '!':
        if (pos < source.size() && source[pos] == '=') { pos++; return { TokenType::NE, "!=" }; }
        return { TokenType::LOG_NOT, "!" };
    case '&':
        if (pos < source.size() && source[pos] == '&') { pos++; return { TokenType::LOG_AND, "&&" }; }
        return { TokenType::ERROR, "&" };
    case '|':
        if (pos < source.size() && source[pos] == '|') { pos++; return { TokenType::LOG_OR, "||" }; }
        return { TokenType::ERROR, "|" };
    case '?': return { TokenType::QUESTION, "?" };
    case ':': return { TokenType::COLON, ":" };
    case ',': return { TokenType::COMMA, "," };
    default: return { TokenType::ERROR, std::string("Unexpected char: ") + c };
    }
}