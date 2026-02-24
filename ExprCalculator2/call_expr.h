#pragma once

#include <vector>

#include "expr.h"

// 函数调用
class CallExpr : public Expr {
    std::string func_name;
    std::vector<std::unique_ptr<Expr>> args;
public:
    CallExpr(std::string name, std::vector<std::unique_ptr<Expr>> a);
    Value evaluate(Evaluator& eval) const override;
    std::unique_ptr<Expr> simplify() const override;
    std::string to_string() const override;
};