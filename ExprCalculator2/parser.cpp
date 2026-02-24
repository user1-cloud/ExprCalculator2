#include <stdexcept>

#include "parser.h"
#include "lexer.h"

Token Parser::current() const { return pos < tokens.size() ? tokens[pos] : Token{ TokenType::END, "" }; }
void Parser::consume() { if (pos < tokens.size()) pos++; }

int Parser::getPrecedence(TokenType op) const {
    switch (op) {
    case TokenType::ASSIGN: return 1;
    case TokenType::QUESTION: return 2;
    case TokenType::LOG_OR: return 3;
    case TokenType::LOG_AND: return 4;
    case TokenType::EQ: case TokenType::NE: return 5;
    case TokenType::GT: case TokenType::LT: case TokenType::GE: case TokenType::LE: return 6;
    case TokenType::PLUS: case TokenType::MINUS: return 7;
    case TokenType::STAR: case TokenType::SLASH: case TokenType::MOD: return 8;
    case TokenType::POW: return 9;
    default: return 0;
    }
}

bool Parser::isBinaryOp(TokenType t) const {
    return t == TokenType::PLUS || t == TokenType::MINUS || t == TokenType::STAR ||
        t == TokenType::SLASH || t == TokenType::MOD || t == TokenType::POW ||
        t == TokenType::GT || t == TokenType::LT || t == TokenType::GE || t == TokenType::LE ||
        t == TokenType::EQ || t == TokenType::NE || t == TokenType::LOG_AND || t == TokenType::LOG_OR ||
        t == TokenType::QUESTION ||
        t == TokenType::ASSIGN;
}

std::unique_ptr<Expr> Parser::parseExpression(int min_precedence) {
    auto lhs = parsePrimary();
    while (isBinaryOp(current().type)) {
        auto op = current().type;
        int prec = getPrecedence(op);
        if (prec < min_precedence) break;
        consume();
        if (op == TokenType::QUESTION) {
            auto true_expr = parseExpression();
            if (current().type != TokenType::COLON) throw std::runtime_error("Expected ':' in ternary");
            consume();
            auto false_expr = parseExpression(prec);
            lhs = std::make_unique<ConditionalExpr>(std::move(lhs), std::move(true_expr), std::move(false_expr));
            continue;
        }
        int next_prec = (op == TokenType::ASSIGN || op == TokenType::POW) ? prec : prec + 1;
        auto rhs = parseExpression(next_prec);
        lhs = std::make_unique<BinaryExpr>(std::move(lhs), op, std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    auto tok = current(); consume();
    switch (tok.type) {
    case TokenType::NUMBER:
        return std::make_unique<NumberExpr>(tok.number_value);
    case TokenType::IDENTIFIER: {
        if (current().type == TokenType::LPAREN) {
            consume();
            std::vector<std::unique_ptr<Expr>> args;
            if (current().type != TokenType::RPAREN) {
                args.push_back(parseExpression());
                while (current().type == TokenType::COMMA) {
                    consume();
                    args.push_back(parseExpression());
                }
            }
            if (current().type != TokenType::RPAREN) throw std::runtime_error("Expected ')'");
            consume();
            return std::make_unique<CallExpr>(tok.lexeme, std::move(args));
        }
        if (current().type == TokenType::ASSIGN) {
            consume();
            auto val = parseExpression();
            return std::make_unique<AssignExpr>(tok.lexeme, std::move(val));
        }
        return std::make_unique<VariableExpr>(tok.lexeme);
    }
    case TokenType::LPAREN: {
        auto expr = parseExpression();
        if (current().type != TokenType::RPAREN) throw std::runtime_error("Expected ')'");
        consume();
        return expr;
    }
    case TokenType::MINUS:
        return std::make_unique<UnaryExpr>(TokenType::MINUS, parsePrimary());
    case TokenType::LOG_NOT:
        return std::make_unique<UnaryExpr>(TokenType::LOG_NOT, parsePrimary());
    case TokenType::QUESTION:
        throw std::runtime_error("Ternary handled in binary parsing");
    default:
        throw std::runtime_error("Unexpected token in primary: " + tok.lexeme);
    }
}


Parser::Parser(const std::string& input) {
    Lexer lexer(input);
    Token tok;
    while ((tok = lexer.nextToken()).type != TokenType::END) {
        if (tok.type == TokenType::ERROR) throw std::runtime_error("Lexer error: " + tok.lexeme);
        tokens.push_back(tok);
    }
}

std::unique_ptr<Expr> Parser::parse() {
    if (tokens.empty()) throw std::runtime_error("Empty expression");
    auto expr = parseExpression();
    if (pos < tokens.size()) throw std::runtime_error("Unexpected token after expression");
    return expr;
}