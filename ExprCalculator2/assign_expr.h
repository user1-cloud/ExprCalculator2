#pragma once

#include "expr.h"

// 赋值表达式
class AssignExpr : public Expr {
    std::string var_name;
    std::unique_ptr<Expr> value;
public:
    AssignExpr(std::string name, std::unique_ptr<Expr> val);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};