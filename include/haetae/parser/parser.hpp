#pragma once

#include "../lexer/token.hpp"
#include "ast.hpp"
#include "op.hpp"
#include "err.hpp"
#include "expr/expr.hpp"

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

    std::optional<Token> next(int n);
    std::expected<Expr, ParserError> parse();
    
    // Data Type
    std::expected<DataType, ParserError> parse_dtype(bool is_shape = false);

    // Pratt Parser
    std::expected<Expr, ParserError> parse_prefix();
    std::expected<Expr, ParserError> parse_expr(Precedence prec = Precedence::Lowest);
    std::expected<Expr, ParserError> parse_nud();
    std::expected<Expr, ParserError> parse_led(Expr left);
    std::expected<Expr, ParserError> parse_fncall(Expr left);

    // Fn
    std::expected<Expr, ParserError> parse_fn();

public:
    Parser(std::vector<Token>&& tokens) : tokens(std::move(tokens)), pos(0) {}

    std::expected<AST, ParserError> get_ast();
};