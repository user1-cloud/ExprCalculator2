#include "evaluator.h" // 节点递归运算需要
#include "unary_expr.h"
#include "number_expr.h"

// 一元运算
UnaryExpr::UnaryExpr(TokenType o, std::unique_ptr<Expr> expr) : op(o), operand(std::move(expr)) {}

std::string UnaryExpr::to_string() const {
    std::string op_str = (op == TokenType::MINUS) ? "-" : "!";
    return op_str + operand->to_string();
}


Value UnaryExpr::evaluate(Evaluator& eval) const {
    Value v = operand->evaluate(eval);
    if (v.is_symbol()) {
        throw std::runtime_error("Cannot evaluate expression with undefined variables");
    }
    switch (op) {
    case TokenType::MINUS: return Value(-v.num);
    case TokenType::LOG_NOT: return Value(std::abs(v.num) < 1e-9 ? 1.0 : 0.0);
    default: throw std::runtime_error("Unhandled unary operator");
    }
}


std::unique_ptr<Expr> UnaryExpr::simplify() const {
    auto simplified_operand = operand->simplify();
    auto is_constant = [](const Expr* expr) {
        return dynamic_cast<const NumberExpr*>(expr) != nullptr;
        };

    // 常数折叠（纯常数表达式直接计算）
    if (is_constant(simplified_operand.get())) {
        double r = dynamic_cast<const NumberExpr*>(simplified_operand.get())->val;
        double result = 0.0;

        switch (op) {
        case TokenType::MINUS: result = -r; break;
        case TokenType::LOG_NOT: result = r ? 1.0 : 0.0; break;
        default:
            // 其他运算符（如ASSIGN）不折叠
            goto end_constant_folding;
        }
        return std::make_unique<NumberExpr>(result);
    }
end_constant_folding:

    if (op == TokenType::MINUS) {
        if (auto unary = dynamic_cast<const UnaryExpr*>(simplified_operand.get())) {
            if (unary->op == TokenType::MINUS) {
                return unary->operand->simplify();
            }
        }
    }
    else if (op == TokenType::LOG_NOT) {
        if (auto unary = dynamic_cast<const UnaryExpr*>(simplified_operand.get())) {
            if (unary->op == TokenType::LOG_NOT) {
                return unary->operand->simplify();
            }
        }
    }

    return std::make_unique<UnaryExpr>(op, std::move(simplified_operand));
}