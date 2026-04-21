#pragma once

#include <haetae/irgen/err.hpp>
#include <haetae/parser/ast.hpp>

#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/SCF/IR/SCF.h>
#include <mlir/Dialect/Affine/IR/AffineOps.h>
#include <mlir/Dialect/MemRef/IR/MemRef.h>

#include <expected>
#include <optional>

class IRGen {
private:
    AST ast;
    mlir::MLIRContext ctx;
    mlir::OpBuilder builder;
    std::optional<mlir::ModuleOp> module;

public:
    IRGen(AST ast) : ast(std::move(ast)), builder(&ctx) {}
    
    mlir::Type convert_type(const DataType &dtype);

    std::expected<void, IRGenError> convert_expr(Expr &expr);
    std::expected<void, IRGenError> convert_fndef(FnDefExpr &fndef);
    std::expected<void, IRGenError> convert_all();
    std::string generate_ir();
};
