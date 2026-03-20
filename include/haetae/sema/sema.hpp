#pragma once

#include <haetae/parser/ast.hpp>
#include <haetae/sema/err.hpp>
#include <haetae/sema/symbol.hpp>

#include <expected>
#include <unordered_map>

class Sema {
private:
    AST ast;
    SymbolTable symbols;

public:
    Sema(AST&& ast) : ast(std::move(ast)) { }

    std::expected<AST, SemaError> analyze_all();
};