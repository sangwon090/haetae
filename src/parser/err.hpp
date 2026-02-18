#pragma once 

#include <string>

struct ParserError {
    std::string msg;

public:
    ParserError(std::string msg) : msg(msg) { }
};