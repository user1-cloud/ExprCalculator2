#pragma once

#include <string>

struct Value {
    enum class Type {
        NUMBER,    // 数值类型
        COMPLEX,   // 复数类型（预留）
        SYMBOL     // 符号类型（变量名）
    };

    Type type = Type::NUMBER;
    double num = 0.0;
    std::string symbol_name; // 存储符号名

    // 数值构造函数
    explicit Value(double v = 0.0);
    // 符号构造函数
    explicit Value(std::string name);

    operator double() const;

    bool is_number() const { return type == Type::NUMBER; }
    bool is_complex() const { return type == Type::COMPLEX; }
    bool is_symbol() const { return type == Type::SYMBOL; }
};