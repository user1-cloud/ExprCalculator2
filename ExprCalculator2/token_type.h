#pragma once

enum class TokenType {
    NUMBER, IDENTIFIER, PLUS, MINUS, STAR, SLASH, MOD, POW,
    LPAREN, RPAREN, ASSIGN,
    GT, LT, GE, LE, EQ, NE,
    LOG_AND, LOG_OR, LOG_NOT,
    QUESTION, COLON,
    COMMA,
    END, ERROR
};