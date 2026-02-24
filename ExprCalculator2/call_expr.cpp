#include "evaluator.h" // 节点递归运算需要
#include "call_expr.h"

// 函数调用
CallExpr::CallExpr(std::string name, std::vector<std::unique_ptr<Expr>> a)
        : func_name(std::move(name)), args(std::move(a)) {}

std::string CallExpr::to_string() const {
    std::string s = func_name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) s += ", ";
        s += args[i]->to_string();
    }
    s += ")";
    return s;
}


Value CallExpr::evaluate(Evaluator& eval) const {
    auto it = eval.builtin_funcs.find(func_name);
    if (it == eval.builtin_funcs.end())
        throw std::runtime_error("Undefined function: " + func_name);

    if (args.size() != 1)
        throw std::runtime_error("Function " + func_name + " expects 1 argument");

    Value arg = args[0]->evaluate(eval);
    if (arg.is_symbol()) {
        throw std::runtime_error("Cannot evaluate function with undefined variables");
    }
    return Value(it->second(arg.num));
}


std::unique_ptr<Expr> CallExpr::simplify() const {
    std::vector<std::unique_ptr<Expr>> new_args;
    for (auto& arg : args) {
        new_args.push_back(arg->simplify());
    }
    return std::make_unique<CallExpr>(func_name, std::move(new_args));
}