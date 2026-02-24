#include "evaluator.h" // 节点递归运算需要
#include "conditional_expr.h"
#include "number_expr.h"

// 三元条件表达式
ConditionalExpr::ConditionalExpr(std::unique_ptr<Expr> c, std::unique_ptr<Expr> t, std::unique_ptr<Expr> f)
        : cond(std::move(c)), true_expr(std::move(t)), false_expr(std::move(f)) {
    }

std::string ConditionalExpr::to_string() const {
    return cond->to_string() + " ? " + true_expr->to_string() + " : " + false_expr->to_string();
}


Value ConditionalExpr::evaluate(Evaluator& eval) const {
    Value cond_val = cond->evaluate(eval);
    if (cond_val.is_symbol()) {
        throw std::runtime_error("Cannot evaluate conditional with undefined variables");
    }
    return (std::abs(cond_val.num) > 1e-9) ? true_expr->evaluate(eval) : false_expr->evaluate(eval);
}


std::unique_ptr<Expr> ConditionalExpr::simplify() const {
    auto new_cond = cond->simplify();
    auto new_true = true_expr->simplify();
    auto new_false = false_expr->simplify();

    if (auto num_cond = dynamic_cast<const NumberExpr*>(new_cond.get())) {
        if (std::abs(num_cond->val) > 1e-9) {
            return std::move(new_true);
        }
        else {
            return std::move(new_false);
        }
    }

    return std::make_unique<ConditionalExpr>(std::move(new_cond), std::move(new_true), std::move(new_false));
}