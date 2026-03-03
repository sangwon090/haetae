#pragma once

#include <haetae/utils/overloaded.hpp>

#include <variant>
#include <string> 
#include <vector>
#include <ostream>
#include <format>

struct Atomic { std::string name; };
struct Integer { int bits; };
struct Floating { int bits; };
struct BrainFloating { int bits; };
struct Boolean { };

using NumericType = std::variant<
    Integer,
    Floating,
    BrainFloating
>;

struct Tensor {
    NumericType dtype;
    std::vector<int> dim;
};

using DataType = std::variant<
    Integer,
    Floating,
    Tensor,
    Boolean
>;


inline std::ostream& operator<<(std::ostream& os, const Atomic& atomic) {
    return os << "Atomic(" << atomic.name << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Integer& integer) {
    return os << "Int" << integer.bits;
}

inline std::ostream& operator<<(std::ostream& os, const Floating& floating) {
    return os << "Float" << floating.bits;
}

inline std::ostream& operator<<(std::ostream& os, const BrainFloating& bfloating) {
    return os << "BFloat" << bfloating.bits;
}

inline std::ostream& operator<<(std::ostream& os, const NumericType& numeric) {
    std::visit([&](const auto& dtype) {
        os << dtype;
    }, numeric);

    return os;
}

inline std::string get_mnemonic(const NumericType& numeric) {
    return std::visit(overloaded {
        [&](const Integer& integer) {
            return std::format("i{0}", integer.bits);
        },
        [&](const Floating& floating) {
            return std::format("f{0}", floating.bits);
        },
        [&](const BrainFloating& bfloating) {
            return std::format("bf{0}", bfloating.bits);
        }
    }, numeric);
}

inline std::ostream& operator<<(std::ostream& os, const Tensor& tensor) {
    os << "Tensor<";

    for(auto size : tensor.dim) {
        os << size << "x";
    }

    return os << get_mnemonic(tensor.dtype) << ">";
}

inline std::ostream& operator<<(std::ostream& os, const Boolean& boolean) {
    return os << "Boolean";
}

inline std::ostream& operator<<(std::ostream& os, const DataType& dtype) {
    std::visit([&](const auto& dtype) {
        os << dtype;
    }, dtype);

    return os;
}