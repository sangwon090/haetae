#pragma once

#include "../parser/ast.hpp"

class IRGen {
private:
    AST ast;    

public:
    IRGen(AST ast) : ast(std::move(ast)) {}
    void generate_ir();
};