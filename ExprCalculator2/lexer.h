#pragma once

#include <iostream>

#include "token.h"

// 词法分析器
class Lexer {
    std::string source;
    size_t pos = 0;
public:
    Lexer(const std::string& src);
    Token nextToken();
};