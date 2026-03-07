#pragma once

#include "haetae/lexer/token.hpp"

#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <string>

class Lexer {
public:
    Lexer(std::string&& source) : source(std::move(source)) {}
    Lexer(std::ifstream&& file) : source(load_file(std::move(file))) {}

    std::vector<Token> get_tokens();

private:
    std::string load_file(std::ifstream&& file);

    Token read_number();
    Token read_string();
    Token read_ident();
    Token read_newline();

    bool is_continuable(Token token);

    std::string source;
    int offset;

    static const std::map<std::string, Keyword> keywords;
    static const std::map<std::string, DataType> dtypes;
};