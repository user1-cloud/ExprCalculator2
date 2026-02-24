#pragma once

#include <memory>

#include "value.h"

class Expr {
public:
    virtual ~Expr() = default;
    virtual Value evaluate(class Evaluator& eval) const = 0;
    virtual std::unique_ptr<Expr> simplify() const = 0;
    // 生成符号表达式字符串
    virtual std::string to_string() const = 0;
};