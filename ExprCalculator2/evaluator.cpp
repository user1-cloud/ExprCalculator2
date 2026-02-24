#include "evaluator.h"

void Evaluator::initConstants() {
    variables["pi"] = M_PI;
    variables["e"] = M_E;
}

void Evaluator::initFunctions() {
    builtin_funcs["sin"] = [](double x) { return std::sin(x); };
    builtin_funcs["cos"] = [](double x) { return std::cos(x); };
    builtin_funcs["tan"] = [](double x) { return std::tan(x); };
    builtin_funcs["sqrt"] = [](double x) { return std::sqrt(x); };
    builtin_funcs["abs"] = [](double x) { return std::abs(x); };
    builtin_funcs["log"] = [](double x) { return std::log10(x); };
    builtin_funcs["ln"] = [](double x) { return std::log(x); };
    builtin_funcs["exp"] = [](double x) { return std::exp(x); };
}

Evaluator::Evaluator() {
    initConstants();
    initFunctions();
}

double Evaluator::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) throw std::runtime_error("Undefined variable: " + name);
    return it->second;
}

void Evaluator::setVariable(const std::string& name, double value) {
    variables[name] = value;
}

Value Evaluator::evaluate(const Expr* expr) {
    return expr->evaluate(*this);
}