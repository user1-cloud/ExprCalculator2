#include "safe_double.h"

#include "eps.h"

// 构造函数：隐式转换为 double，兼容现有代码
SafeDouble::SafeDouble(double val) : value_(val) {}

// 转换回 double 的运算符，不影响原有运算逻辑
SafeDouble::operator double() const {
        return value_;
}

// 重载 == 运算符，使用安全比较逻辑
bool SafeDouble::operator==(const SafeDouble& other) const {
    if (std::isnan(value_) || std::isnan(other.value_)) return false;
    if (std::isinf(value_) || std::isinf(other.value_)) return value_ == other.value_;
    return std::fabs(value_ - other.value_) < DEFAULT_EPS;
}

// 重载 != 运算符（配套）
bool SafeDouble::operator!=(const SafeDouble& other) const {
    return !(*this == other);
}

// 暴露设置自定义阈值的接口（适配特殊场景）
bool SafeDouble::equals(const SafeDouble& a, const SafeDouble& b, double eps) {
    if (std::isnan(a.value_) || std::isnan(b.value_)) return false;
    if (std::isinf(a.value_) || std::isinf(b.value_)) return a.value_ == b.value_;
    return std::fabs(a.value_ - b.value_) < eps;
}