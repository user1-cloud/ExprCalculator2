#pragma once

#include <cmath>

class SafeDouble {
private:
    double value_;

public:
    // 构造函数：隐式转换为 double，兼容现有代码
    SafeDouble(double val = 0.0);

    // 转换回 double 的运算符，不影响原有运算逻辑
    operator double() const;

    // 重载 == 运算符，使用安全比较逻辑
    bool operator==(const SafeDouble& other) const;

    // 重载 != 运算符（配套）
    bool operator!=(const SafeDouble& other) const;

    //暴露设置自定义阈值的接口（适配特殊场景）
    static bool equals(const SafeDouble& a, const SafeDouble& b, double eps);
};