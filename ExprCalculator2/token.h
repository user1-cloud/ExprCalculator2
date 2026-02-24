#pragma once

#include <string>

#include "token_type.h"

struct Token {
    TokenType type = TokenType::ERROR;
    std::string lexeme;
    double number_value = 0.0;
};