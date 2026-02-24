#include "evaluator.h" // 节点递归运算需要
#include "binary_expr.h"
#include "number_expr.h"
#include "variable_expr.h"
#include "unary_expr.h"

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r)
        : lhs(std::move(l)), op(o), rhs(std::move(r)) {}

std::string BinaryExpr::to_string() const {
    std::string op_str;
    switch (op) {
    case TokenType::PLUS: op_str = "+"; break;
    case TokenType::MINUS: op_str = "-"; break;
    case TokenType::STAR: op_str = "*"; break;
    case TokenType::SLASH: op_str = "/"; break;
    case TokenType::MOD: op_str = "%"; break;
    case TokenType::POW: op_str = "^"; break;
    default: op_str = "?";
    }
    return "(" + lhs->to_string() + " " + op_str + " " + rhs->to_string() + ")";
}

// 辅助方法：获取操作数和运算符
Expr* BinaryExpr::get_lhs() const { return lhs.get(); }
Expr* BinaryExpr::get_rhs() const { return rhs.get(); }
TokenType BinaryExpr::get_op() const { return op; }


Value BinaryExpr::evaluate(Evaluator& eval) const {
    Value l = lhs->evaluate(eval);
    Value r = rhs->evaluate(eval);
    const double EPS = 1e-9;

    // 如果任意一方是符号类型，直接返回错误（或扩展支持符号运算）
    if (l.is_symbol() || r.is_symbol()) {
        throw std::runtime_error("Cannot evaluate expression with undefined variables");
    }

    switch (op) {
    case TokenType::PLUS: return Value(l.num + r.num);
    case TokenType::MINUS: return Value(l.num - r.num);
    case TokenType::STAR: return Value(l.num * r.num);
    case TokenType::SLASH:
        if (std::abs(r.num) < EPS) throw std::runtime_error("Division by zero");
        return Value(l.num / r.num);
    case TokenType::MOD:
        if (std::abs(r.num) < EPS) throw std::runtime_error("Modulo by zero");
        return Value(std::fmod(l.num, r.num));
    case TokenType::POW: return Value(std::pow(l.num, r.num));
    case TokenType::GT: return Value(l.num > r.num + EPS ? 1.0 : 0.0);
    case TokenType::LT: return Value(l.num < r.num - EPS ? 1.0 : 0.0);
    case TokenType::GE: return Value(l.num >= r.num - EPS ? 1.0 : 0.0);
    case TokenType::LE: return Value(l.num <= r.num + EPS ? 1.0 : 0.0);
    case TokenType::EQ: return Value(std::abs(l.num - r.num) < EPS ? 1.0 : 0.0);
    case TokenType::NE: return Value(std::abs(l.num - r.num) >= EPS ? 1.0 : 0.0);
    case TokenType::LOG_AND: return Value((std::abs(l.num) > EPS && std::abs(r.num) > EPS) ? 1.0 : 0.0);
    case TokenType::LOG_OR: return Value((std::abs(l.num) > EPS || std::abs(r.num) > EPS) ? 1.0 : 0.0);
    default: throw std::runtime_error("Unhandled binary operator");
    }
}


std::unique_ptr<Expr> BinaryExpr::simplify() const {
    auto new_lhs = lhs->simplify();
    auto new_rhs = rhs->simplify();

    // 辅助函数：判断是否为常数
    auto is_constant = [](const Expr* expr) {
        return dynamic_cast<const NumberExpr*>(expr) != nullptr;
        };

    // 常数折叠（纯常数表达式直接计算）
    if (is_constant(new_lhs.get()) && is_constant(new_rhs.get())) {
        double l = dynamic_cast<const NumberExpr*>(new_lhs.get())->val;
        double r = dynamic_cast<const NumberExpr*>(new_rhs.get())->val;
        double result = 0.0;

        switch (op) {
        case TokenType::PLUS:  result = l + r; break;
        case TokenType::MINUS: result = l - r; break;
        case TokenType::STAR:  result = l * r; break;
        case TokenType::SLASH:
            if (r != 0) result = l / r;
            else throw std::runtime_error("Division by zero");
            break;
        case TokenType::MOD:
            if (r != 0) result = std::fmod(l, r);
            else throw std::runtime_error("Modulo by zero");
            break;
        case TokenType::POW:   result = std::pow(l, r); break;
        case TokenType::GT:    result = (l > r) ? 1.0 : 0.0; break;
        case TokenType::LT:    result = (l < r) ? 1.0 : 0.0; break;
        case TokenType::GE:    result = (l >= r) ? 1.0 : 0.0; break;
        case TokenType::LE:    result = (l <= r) ? 1.0 : 0.0; break;
        case TokenType::EQ:    result = (std::abs(l - r) < 1e-9) ? 1.0 : 0.0; break;
        case TokenType::NE:    result = (std::abs(l - r) >= 1e-9) ? 1.0 : 0.0; break;
        case TokenType::LOG_AND: result = (l != 0 && r != 0) ? 1.0 : 0.0; break;
        case TokenType::LOG_OR:  result = (l != 0 || r != 0) ? 1.0 : 0.0; break;
        default:
            // 其他运算符（如ASSIGN）不折叠
            goto end_constant_folding;
        }
        return std::make_unique<NumberExpr>(result);
    }
end_constant_folding:
    // 辅助函数1：提取项信息（用于同类项合并）
    struct TermInfo {
        double coeff = 1.0;
        std::string var_name;
        bool is_valid = false;
    };
    auto get_term_info = [&](const Expr* expr) -> TermInfo {
        TermInfo info;

        // 纯变量（如 x）
        if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
            info.coeff = 1.0;
            info.var_name = var->name;
            info.is_valid = true;
            return info;
        }

        // 常数*变量（如 2*x）
        if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
            if (bin->get_op() == TokenType::STAR) {
                auto num = dynamic_cast<const NumberExpr*>(bin->get_lhs());
                auto var = dynamic_cast<const VariableExpr*>(bin->get_rhs());
                if (num && var) {
                    info.coeff = num->val;
                    info.var_name = var->name;
                    info.is_valid = true;
                    return info;
                }

                // 处理 x*2 这种顺序
                num = dynamic_cast<const NumberExpr*>(bin->get_rhs());
                var = dynamic_cast<const VariableExpr*>(bin->get_lhs());
                if (num && var) {
                    info.coeff = num->val;
                    info.var_name = var->name;
                    info.is_valid = true;
                    return info;
                }
            }
        }

        return info;
        };

    // 辅助函数2：提取幂项信息（用于同底幂合并）
    struct PowerInfo {
        std::string base;   // 底数（变量名）
        double exponent = 1.0; // 指数
        bool is_valid = false;
    };
    auto get_power_info = [&](const Expr* expr) -> PowerInfo {
        PowerInfo info;
        // 情况1：纯变量 d → 底数 d，指数 1
        if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
            info.base = var->name;
            info.exponent = 1.0;
            info.is_valid = true;
            return info;
        }
        // 情况2：幂运算 d^k → 底数 d，指数 k
        if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
            if (bin->get_op() == TokenType::POW) {
                auto base_var = dynamic_cast<const VariableExpr*>(bin->get_lhs());
                auto exp_num = dynamic_cast<const NumberExpr*>(bin->get_rhs());
                if (base_var && exp_num) {
                    info.base = base_var->name;
                    info.exponent = exp_num->val;
                    info.is_valid = true;
                    return info;
                }
            }
        }
        return info;
        };
    
    // 1. 加法化简（核心：同类项合并）
    if (op == TokenType::PLUS) {
        TermInfo lhs_term = get_term_info(new_lhs.get());
        TermInfo rhs_term = get_term_info(new_rhs.get());

        // 情况1: x + x -> 2*x
        if (lhs_term.is_valid && rhs_term.is_valid && lhs_term.var_name == rhs_term.var_name) {
            double new_coeff = lhs_term.coeff + rhs_term.coeff;
            return std::make_unique<BinaryExpr>(
                std::make_unique<NumberExpr>(new_coeff),
                TokenType::STAR,
                std::make_unique<VariableExpr>(lhs_term.var_name)
            );
        }

        // 基础化简：a + 0 = a
        if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::move(new_rhs);
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 0)
            return std::move(new_lhs);
    }

    // 2. 乘法化简（核心：同底幂合并 d*d^2 = d^3）
    else if (op == TokenType::STAR) {
        /*if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::make_unique<NumberExpr>(0.0);
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 0)
            return std::make_unique<NumberExpr>(0.0);
        if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 1)
            return std::move(new_rhs);
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 1)
            return std::move(new_lhs);*/

        // 2. 新增：同底幂合并（保留）
        PowerInfo lhs_pow = get_power_info(new_lhs.get());
        PowerInfo rhs_pow = get_power_info(new_rhs.get());
        if (lhs_pow.is_valid && rhs_pow.is_valid && lhs_pow.base == rhs_pow.base) {
            double new_exp = lhs_pow.exponent + rhs_pow.exponent;
            return std::make_unique<BinaryExpr>(
                std::make_unique<VariableExpr>(lhs_pow.base),
                TokenType::POW,
                std::make_unique<NumberExpr>(new_exp)
            );
        }

        // 3. 按你的思路：合并常数因子（核心修改）
        double const_factor = 1.0;  // 存储所有常数的乘积
        std::unique_ptr<Expr> var_expr; // 存储变量/非const表达式

        // 步骤1：检查左边是否是常数，提取常数因子
        if (is_constant(new_lhs.get())) {
            const_factor *= dynamic_cast<const NumberExpr*>(new_lhs.get())->val;
            var_expr = std::move(new_rhs); // 右边作为变量表达式
        }
        // 步骤2：检查右边是否是常数，提取常数因子
        else if (is_constant(new_rhs.get())) {
            const_factor *= dynamic_cast<const NumberExpr*>(new_rhs.get())->val;
            var_expr = std::move(new_lhs); // 左边作为变量表达式
        }
        // 步骤3：两边都不是常数 → 不合并（如x*y）
        else {
            return std::make_unique<BinaryExpr>(std::move(new_lhs), op, std::move(new_rhs));
        }

        // 步骤4：如果变量表达式本身还是乘法（如x*7*8 → 先处理x*7=7*x，再处理7*x*8）
        // 递归化简变量表达式，确保嵌套的乘法也能合并常数
        auto simplified_var = var_expr->simplify();

        // 步骤5：检查化简后的变量表达式是否还是“常数*变量”，继续提取常数
        if (auto bin = dynamic_cast<BinaryExpr*>(simplified_var.get())) {
            if (bin->get_op() == TokenType::STAR) {
                // 如果是“常数*变量”，提取常数
                if (is_constant(bin->get_lhs())) {
                    const_factor *= dynamic_cast<const NumberExpr*>(bin->get_lhs())->val;
                    simplified_var = std::move(*bin).rhs; // 保留变量部分
                }
                // 如果是“变量*常数”，交换后提取常数
                else if (is_constant(bin->get_rhs())) {
                    const_factor *= dynamic_cast<const NumberExpr*>(bin->get_rhs())->val;
                    simplified_var = std::move(*bin).lhs; // 保留变量部分
                }
            }
        }

        if (const_factor == 0)
            return std::make_unique<NumberExpr>(0.0);
        if (const_factor == 1)
            return std::move(simplified_var);
        if (const_factor == -1)
            return std::make_unique<UnaryExpr>(TokenType::MINUS, std::move(simplified_var));

        // 步骤6：重构表达式（常数放左边，变量放右边）
        return std::make_unique<BinaryExpr>(
            std::make_unique<NumberExpr>(const_factor),
            TokenType::STAR,
            std::move(simplified_var)
        );
    }

    // 3. 其他基础化简规则
    else if (op == TokenType::MINUS) {
        TermInfo lhs_term = get_term_info(new_lhs.get());
        TermInfo rhs_term = get_term_info(new_rhs.get());

        if (lhs_term.is_valid && rhs_term.is_valid && lhs_term.var_name == rhs_term.var_name) {
            double new_coeff = lhs_term.coeff - rhs_term.coeff;
            /*if ((std::abs(new_coeff) < 1e-9) ? 1.0 : 0.0) return std::make_unique<NumberExpr>(0.0);
            if ((std::abs(new_coeff - 1) < 1e-9) ? 1.0 : 0.0) return std::make_unique<VariableExpr>(lhs_term.var_name);
            */
            return BinaryExpr(
                std::make_unique<NumberExpr>(new_coeff),
                TokenType::STAR,
                std::make_unique<VariableExpr>(lhs_term.var_name)
            ).simplify();
            /*return std::make_unique<BinaryExpr>(
                std::make_unique<NumberExpr>(new_coeff),
                TokenType::STAR,
                std::make_unique<VariableExpr>(lhs_term.var_name)
            );*/
        }

        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 0)
            return std::move(new_lhs);
        if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::make_unique<UnaryExpr>(TokenType::MINUS, std::move(new_rhs));
    }
    else if (op == TokenType::SLASH) {
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 1)
            return std::move(new_lhs);
        // 不确定右值是否是零，还真不能这样化简
        /*if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::make_unique<NumberExpr>(0.0);*/
    }
    else if (op == TokenType::POW) {
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 0)
            return std::make_unique<NumberExpr>(1.0);
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 1)
            return std::move(new_lhs);
        if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::make_unique<NumberExpr>(0.0);
    }
    else if (op == TokenType::MOD) {
        if (is_constant(new_rhs.get()) && dynamic_cast<const NumberExpr*>(new_rhs.get())->val == 1)
            return std::make_unique<NumberExpr>(0.0);
        if (is_constant(new_lhs.get()) && dynamic_cast<const NumberExpr*>(new_lhs.get())->val == 0)
            return std::make_unique<NumberExpr>(0.0);
    }
    else if (op == TokenType::GT || op == TokenType::LT ||
        op == TokenType::GE || op == TokenType::LE ||
        op == TokenType::EQ || op == TokenType::NE) {
        if (is_constant(new_lhs.get()) && is_constant(new_rhs.get())) {
            double l = dynamic_cast<const NumberExpr*>(new_lhs.get())->val;
            double r = dynamic_cast<const NumberExpr*>(new_rhs.get())->val;
            double result = 0.0;

            switch (op) {
            case TokenType::GT: result = (l > r) ? 1.0 : 0.0; break;
            case TokenType::LT: result = (l < r) ? 1.0 : 0.0; break;
            case TokenType::GE: result = (l >= r) ? 1.0 : 0.0; break;
            case TokenType::LE: result = (l <= r) ? 1.0 : 0.0; break;
            case TokenType::EQ: result = (std::abs(l - r) < 1e-9) ? 1.0 : 0.0; break;
            case TokenType::NE: result = (std::abs(l - r) >= 1e-9) ? 1.0 : 0.0; break;
            default: break;
            }
            return std::make_unique<NumberExpr>(result);
        }
    }

    return std::make_unique<BinaryExpr>(std::move(new_lhs), op, std::move(new_rhs));
}