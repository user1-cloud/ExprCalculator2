#pragma once

#include "expr.h"

// 三元条件表达式
class ConditionalExpr : public Expr {
    std::unique_ptr<Expr> cond, true_expr, false_expr;
public:
    ConditionalExpr(std::unique_ptr<Expr> c, std::unique_ptr<Expr> t, std::unique_ptr<Expr> f);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};