#pragma once

#include "expr.h"

// 变量引用
class VariableExpr : public Expr {
public:
    std::string name;
    VariableExpr(std::string n);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};