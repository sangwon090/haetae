#include <haetae/parser/expr.hpp>

Expr::Expr(ExprVariant v) : variant(std::move(v)) {}
Expr::~Expr() = default;

UnaryExpr::UnaryExpr(Operator op, std::unique_ptr<Expr> right)
    : op(op), right(std::move(right)) {}
UnaryExpr::~UnaryExpr() = default;

InfixExpr::InfixExpr(Operator op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
    : op(op), left(std::move(left)), right(std::move(right)) {}
InfixExpr::~InfixExpr() = default;

FnCallExpr::FnCallExpr(Identifier ident, std::vector<std::unique_ptr<Expr>> args)
    : ident(std::move(ident)), args(std::move(args)) {}
FnCallExpr::~FnCallExpr() = default;