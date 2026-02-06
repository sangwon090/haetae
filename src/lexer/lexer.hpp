#pragma once

#include "token.hpp"

#include <fstream>
#include <vector>
#include <iostream>

class Lexer {
public:
    Lexer(std::ifstream file);

    std::vector<Token> get_tokens();

private:
    std::string load_file(std::ifstream file);

    Token read_number();
    Token read_string();
    Token read_ident();
    Token read_newline();

    bool is_continuable(Token token);

    std::string source;
    int offset;
};