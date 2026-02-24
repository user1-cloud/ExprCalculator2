#pragma once

#include <vector>
#include <memory>

#include "token.h"
#include "exprs.h"

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;
    Token current() const;
    void consume();

    int getPrecedence(TokenType op) const;
    bool isBinaryOp(TokenType t) const;

    std::unique_ptr<Expr> parseExpression(int min_precedence = 0);
    std::unique_ptr<Expr> parsePrimary();
public:
    Parser(const std::string& input);
    std::unique_ptr<Expr> parse();
};