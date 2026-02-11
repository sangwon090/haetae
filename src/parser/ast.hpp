#pragma once

#include "expr/expr.hpp"

#include <vector>

class AST {
private:
    std::vector<Expr> exprs;
};