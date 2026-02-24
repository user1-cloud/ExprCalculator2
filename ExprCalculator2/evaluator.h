#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <stdexcept>

#include "value.h"
#include "expr.h"
#include "constants.h"


class Evaluator {
    void initConstants();
    void initFunctions();
public:
    std::unordered_map<std::string, double> variables;
    std::unordered_map<std::string, std::function<double(double)>> builtin_funcs;

    Evaluator();

    double getVariable(const std::string& name) const;
    void setVariable(const std::string& name, double value);

    Value evaluate(const Expr* expr);
};