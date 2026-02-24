#pragma once

#include "expr.h"
#include "token_type.h"

// 一元运算
class UnaryExpr : public Expr {
    std::unique_ptr<Expr> operand;
    TokenType op;
public:
    UnaryExpr(TokenType o, std::unique_ptr<Expr> expr);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};