#include "evaluator.h" // 节点递归运算需要
#include "assign_expr.h"

// 赋值表达式
AssignExpr::AssignExpr(std::string name, std::unique_ptr<Expr> val)
        : var_name(std::move(name)), value(std::move(val)) {
    }

std::string AssignExpr::to_string() const {
    return var_name + " = " + value->to_string();
}


Value AssignExpr::evaluate(Evaluator& eval) const {
    Value val = value->evaluate(eval);
    if (val.is_symbol()) {
        throw std::runtime_error("Cannot assign undefined variable");
    }
    eval.setVariable(var_name, val.num);
    return val;
}


std::unique_ptr<Expr> AssignExpr::simplify() const {
    auto new_value = value->simplify();
    return std::make_unique<AssignExpr>(var_name, std::move(new_value));
}