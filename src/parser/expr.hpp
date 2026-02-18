#pragma once

#include "../lexer/token.hpp"
#include "../utils/overloaded.hpp"
#include "op.hpp"

#include <memory>
#include <vector>

struct Expr;

struct UnaryExpr {
    Operator op;
    std::unique_ptr<Expr> right;
};

struct InfixExpr {
    Operator op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct FnCallExpr {
    Identifier ident;
    std::vector<std::unique_ptr<Expr>> args;
};

struct IdentExpr {
    std::string value;
};

struct LiteralExpr {
    Literal lit;
};

using ExprVariant = std::variant<
    UnaryExpr,
    InfixExpr,
    FnCallExpr,
    IdentExpr,
    LiteralExpr
>;

struct Expr {
    ExprVariant variant;
};