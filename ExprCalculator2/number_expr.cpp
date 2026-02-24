#include "evaluator.h" // 节点递归运算需要
#include <sstream>

#include "number_expr.h"

NumberExpr::NumberExpr(double v) : val(v) {}

Value NumberExpr::evaluate(Evaluator&) const {
    return Value(val);
}

std::string NumberExpr::to_string() const {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}


std::unique_ptr<Expr> NumberExpr::simplify() const {
    return std::make_unique<NumberExpr>(val);
}