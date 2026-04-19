#include <haetae/parser/expr/expr.hpp>
#include <haetae/lexer/datatype.hpp>

Expr::Expr(ExprVariant v) : variant(std::move(v)) {}
Expr::~Expr() = default;

UnaryExpr::UnaryExpr(Operator op, std::unique_ptr<Expr> right)
    : op(op), right(std::move(right)) {}
UnaryExpr::~UnaryExpr() = default;

InfixExpr::InfixExpr(Operator op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
    : op(op), left(std::move(left)), right(std::move(right)) {}
InfixExpr::~InfixExpr() = default;

FnDefExpr::FnDefExpr(Identifier name, std::vector<std::tuple<Identifier, DataType>> args, DataType rtype, std::vector<Expr> body)
    : name(std::move(name)), args(std::move(args)), rtype(std::move(rtype)), body(std::move(body)) {}
FnDefExpr::~FnDefExpr() = default;

FnCallExpr::FnCallExpr(Identifier ident, std::vector<std::unique_ptr<Expr>> args)
    : ident(std::move(ident)), args(std::move(args)) {}
FnCallExpr::~FnCallExpr() = default;

std::ostream& operator<<(std::ostream& os, const Expr& expr) {
    os << "Expr(variant=" << expr.variant << ", sema_info=";
    
    if(expr.sema_info.has_value()) os << *expr.sema_info;
    else os << "null";

    return os << ")";
}