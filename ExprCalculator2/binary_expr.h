#pragma once

#include "expr.h"

#include "token_type.h"

// 二元运算
class BinaryExpr : public Expr {
    std::unique_ptr<Expr> lhs, rhs;
    TokenType op;
public:
    BinaryExpr(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;

    // 辅助方法：获取操作数和运算符
    Expr* get_lhs() const;
    Expr* get_rhs() const;
    TokenType get_op() const;
};