#pragma once

#include "../lexer/token.hpp"
#include "../utils/overloaded.hpp"
#include "op.hpp"

#include <memory>
#include <vector>

struct Expr;
inline std::ostream& operator<<(std::ostream& os, const Expr& expr);

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
};

inline std::ostream& operator<<(std::ostream& os, const UnaryExpr& expr) {
    return os << "UnaryExpr(op=" << expr.op << ", right=" << *expr.right << ")";
}

inline std::ostream& operator<<(std::ostream& os, const InfixExpr& expr) {
    return os << "UnaryExpr(op=" << expr.op << ", left=" << *expr.left << ", right=" << *expr.right << ")";
}

inline std::ostream& operator<<(std::ostream& os, const FnCallExpr& expr) {
    os << "FnCallExpr(ident=" << expr.ident;

    if(!expr.args.empty()) {
        size_t size = expr.args.size();
        for(int i=0; i<size; i++) {
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
    std::visit([&](auto& expr) {
        os << expr;
    }, variant);
    
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Expr& expr) {
    return os << expr.variant;
}