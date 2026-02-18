#pragma once

#include "../lexer/token.hpp"
#include "ast.hpp"
#include "op.hpp"
#include "err.hpp"
#include "expr.hpp"

#include <vector>
#include <optional>
#include <expected>

enum class Precedence {
    Lowest,
    Pipeline,
    // Lambda,
    // IfElse,
    // BooleanOr,
    // BooleanAnd,
    // BooleanNot,
    EqualNotEqual,
    LessGreater,
    BitwiseOr,
    BitwiseXor,
    BitwiseAnd,
    Shift,
    PlusMinus,
    MulDivMod,
    Unary,
    // Exp,
    // Await,
    Paren,
    FnCall,
};

class Parser {
private:
    std::vector<Token> tokens;
    int pos;

    static std::optional<Operator> get_operator(Token tok, bool is_prefix);
    static std::optional<Precedence> get_precedence(Operator op);

    Token* next(int n);
    std::expected<Expr, ParserError> parse_prefix();
    std::expected<Expr, ParserError> parse_expr(Precedence prec);
    std::expected<Expr, ParserError> parse_nud();
    std::expected<Expr, ParserError> parse_led(Expr left);
    std::expected<Expr, ParserError> parse_fncall(Expr left);

public:
    Parser(std::vector<Token>&& tokens) : tokens(std::move(tokens)), pos(0) {}

    std::expected<AST, ParserError> get_ast();
};