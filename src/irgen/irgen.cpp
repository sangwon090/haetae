#include <haetae/irgen/irgen.hpp>

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

using namespace mlir;

void IRGen::generate_ir() {
    MLIRContext ctx;

    ctx.loadDialect<
        func::FuncDialect,
        arith::ArithDialect,
        tensor::TensorDialect,
        linalg::LinalgDialect,
        scf::SCFDialect
    >();

    OpBuilder builder(&ctx);

    auto module = ModuleOp::create(builder.getUnknownLoc());
}