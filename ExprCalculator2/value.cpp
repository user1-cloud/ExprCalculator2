#include <stdexcept>

#include "value.h"

Value::Value(double v) : type(Type::NUMBER), num(v) {}
    
// 符号构造函数
Value::Value(std::string name) : type(Type::SYMBOL), symbol_name(std::move(name)) {}

Value::operator double() const {
    if (type != Type::NUMBER) throw std::runtime_error("Not a number");
    return num;
}