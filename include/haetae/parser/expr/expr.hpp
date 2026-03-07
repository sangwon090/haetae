#pragma once

#include <haetae/lexer/token.hpp>
#include <haetae/utils/overloaded.hpp>
#include <haetae/parser/op.hpp>
#include <haetae/sema/sema.hpp>

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

struct FnDefExpr {
    Identifier name;
    std::vector<std::tuple<Identifier, DataType>> args;
    DataType rtype;
    std::vector<Expr> body;

    FnDefExpr(Identifier name, std::vector<std::tuple<Identifier, DataType>> args, DataType rtype, std::vector<Expr> body);
    ~FnDefExpr();

    FnDefExpr(const FnDefExpr&) = delete;
    FnDefExpr& operator=(const FnDefExpr&) = delete;
    FnDefExpr(FnDefExpr&&) noexcept = default;
    FnDefExpr& operator=(FnDefExpr&&) noexcept = default;
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
    FnDefExpr,
    FnCallExpr,
    IdentExpr,
    LiteralExpr
>;

struct Expr {
    ExprVariant variant;
    std::optional<SemaInfo> sema_info;

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

inline std::ostream& operator<<(std::ostream& os, const FnDefExpr& expr) {
    os << "FnDefExpr(name=" << expr.name << ", args=[";

    if(!expr.args.empty()) {
        size_t size = expr.args.size();
        for(size_t i=0; i<size; i++) {
            os << "(" << std::get<0>(expr.args[i]) << ": " << std::get<1>(expr.args[i]) << ")";
            if(i != size - 1) os << ", ";
        }
    }

    os << "], body=[";
    if(!expr.body.empty()) {
        size_t size = expr.body.size();
        for(size_t i=0; i<size; i++) {
            os << expr.body[i];
            if(i != size - 1) os << ", ";
        }
    }

    os << "], rtype=" << expr.rtype << ")";

    return os;
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

