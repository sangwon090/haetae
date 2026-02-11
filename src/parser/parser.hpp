#pragma once

#include "../lexer/token.hpp"
#include "ast.hpp"

#include <vector>
#include <optional>

class Parser {
private:
    std::vector<Token> tokens;
    int pos;

    std::optional<Token> next(int n);

public:
    Parser(std::vector<Token>&& tokens) : tokens(std::move(tokens)), pos(0) {}

    AST get_ast();
};