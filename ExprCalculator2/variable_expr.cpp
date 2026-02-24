#include "evaluator.h" // 节点递归运算需要
#include "variable_expr.h"

VariableExpr::VariableExpr(std::string n) : name(std::move(n)) {}

std::string VariableExpr::to_string() const {
    return name;
}


Value VariableExpr::evaluate(Evaluator& eval) const {
    auto it = eval.variables.find(name);
    if (it != eval.variables.end()) {
        return Value(it->second);
    }
    else {
        return Value(name); // 未定义变量返回符号值
    }
}


std::unique_ptr<Expr> VariableExpr::simplify() const {
    return std::make_unique<VariableExpr>(name);
}