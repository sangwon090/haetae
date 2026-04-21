#pragma once 

#include <string>
#include <ostream>

struct IRGenError {
    std::string msg;

public:
    IRGenError(std::string msg) : msg(msg) { }
    std::string toString() const { return this->msg; }
};

inline std::ostream& operator<<(std::ostream& os, const IRGenError& err) {
    return os << "IRGenError: " << err.msg;
}