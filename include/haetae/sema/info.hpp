#pragma once

#include <haetae/lexer/datatype.hpp>

#include <ostream>

struct SemaInfo {
    DataType dtype;
};

inline std::ostream& operator<<(std::ostream& os, const SemaInfo &sema_info) {
    return os << "SemaInfo(dtype=" << sema_info.dtype << ")";
}