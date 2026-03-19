#pragma once

#include <haetae/parser/ast.hpp>
#include <haetae/sema/err.hpp>

#include <expected>
#include <unordered_map>

class Sema {
private:
    AST ast;
    std::unordered_map<std::string, std::string> symbols;

public:
    Sema(AST&& ast) : ast(std::move(ast)) { }

    std::expected<AST, SemaError> analyze();
};