#pragma once

#include <haetae/parser/expr/expr.hpp>

#include <vector>

struct AST {
public:
    std::vector<Expr> exprs;
};