#pragma once 

#include <string>

struct ParserError {
    std::string msg;

public:
    ParserError(std::string msg) : msg(msg) { }
    std::string toString() const { return this->msg; }
};

inline std::ostream& operator<<(std::ostream& os, const ParserError& err) {
    return os << "ParserError: " << err.msg << '\n';
}