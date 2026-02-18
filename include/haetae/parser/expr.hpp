#pragma once

#include "../lexer/token.hpp"
#include "../utils/overloaded.hpp"
#include "op.hpp"

#include <memory>
#include <vector>
#include <variant>
#include <ostream>

struct Expr;

std::ostream& operator<<(std::ostream& os, const Expr& expr);

struct UnaryExpr {
    Operator op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Operator op, std::unique_ptr<Expr> right);
    ~UnaryExpr();

    UnaryExpr(const UnaryExpr&) = delete;
    UnaryExpr& operator=(const UnaryExpr&) = delete;
    UnaryExpr(UnaryExpr&&) noexcept = default;
    UnaryExpr& operator=(UnaryExpr&&) noexcept = default;
};

struct InfixExpr {
    Operator op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    InfixExpr(Operator op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    ~InfixExpr();

    InfixExpr(const InfixExpr&) = delete;
    InfixExpr& operator=(const InfixExpr&) = delete;
    InfixExpr(InfixExpr&&) noexcept = default;
    InfixExpr& operator=(InfixExpr&&) noexcept = default;
};

struct FnCallExpr {
    Identifier ident;
    std::vector<std::unique_ptr<Expr>> args;

    FnCallExpr(Identifier ident, std::vector<std::unique_ptr<Expr>> args);
    ~FnCallExpr();

    FnCallExpr(const FnCallExpr&) = delete;
    FnCallExpr& operator=(const FnCallExpr&) = delete;
    FnCallExpr(FnCallExpr&&) noexcept = default;
    FnCallExpr& operator=(FnCallExpr&&) noexcept = default;
};

struct IdentExpr {
    Identifier value;
};

struct LiteralExpr {
    Literal value;
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

    explicit Expr(ExprVariant v);
    ~Expr();

    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;
    Expr(Expr&&) noexcept = default;
    Expr& operator=(Expr&&) noexcept = default;
};

inline std::ostream& operator<<(std::ostream& os, const UnaryExpr& expr) {
    return os << "UnaryExpr(op=" << expr.op << ", right=" << *expr.right << ")";
}

inline std::ostream& operator<<(std::ostream& os, const InfixExpr& expr) {
    return os << "InfixExpr(op=" << expr.op << ", left=" << *expr.left << ", right=" << *expr.right << ")";
}

inline std::ostream& operator<<(std::ostream& os, const FnCallExpr& expr) {
    os << "FnCallExpr(ident=" << expr.ident;

    if(!expr.args.empty()) {
        size_t size = expr.args.size();
        for(size_t i=0; i<size; i++) {
            os << *expr.args[i];
            if(i != size - 1) os << ", ";
        }
    }

    return os << ")";
}

inline std::ostream& operator<<(std::ostream& os, const IdentExpr& expr) {
    return os << expr.value;
}

inline std::ostream& operator<<(std::ostream& os, const LiteralExpr& expr) {
    return os << expr.value;
}

inline std::ostream& operator<<(std::ostream& os, const ExprVariant& variant) {
    std::visit([&](const auto& e) { os << e; }, variant);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Expr& expr) {
    return os << expr.variant;
}
