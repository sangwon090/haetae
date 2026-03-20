#pragma once

#include <haetae/lexer/datatype.hpp>

#include <variant>
#include <unordered_map>
#include <vector>
#include <tuple>

struct FnSymbol {
    std::vector<std::tuple<Identifier, DataType>> args;
    DataType rtype;
};

struct VarSymbol {
    DataType dtype;
};

using Symbol = std::variant<
    FnSymbol,
    VarSymbol
>;

struct SymbolTable {
    std::unordered_map<Identifier, Symbol> symbols;
    SymbolTable* parent;

    SymbolTable() : parent(nullptr) { }
    SymbolTable(SymbolTable* parent) : parent(parent) { }

    Symbol* lookup(const Identifier& ident) {
        if(symbols.count(ident)) return &symbols[ident];
        if(parent) return parent->lookup(ident);
        return nullptr;
    }

    Symbol* find(const Identifier& ident) {
        if(symbols.count(ident)) return &symbols[ident];
        return nullptr;
    }

    void insert(const Identifier& ident, Symbol sym) {
        symbols[ident] = sym;
    }
};