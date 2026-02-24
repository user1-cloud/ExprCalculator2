#pragma once

#include "expr.h"

// 字面量
class NumberExpr : public Expr {
public:
    double val;
    NumberExpr(double v);
    Value evaluate(Evaluator&) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};