#pragma once 

#include <string>
#include <ostream>

struct SemaError {
    std::string msg;

public:
    SemaError(std::string msg) : msg(msg) { }
    std::string toString() const { return this->msg; }
};

inline std::ostream& operator<<(std::ostream& os, const SemaError& err) {
    return os << "SemaError: " << err.msg;
}