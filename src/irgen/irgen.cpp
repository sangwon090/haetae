#include <haetae/irgen/irgen.hpp>

#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Verifier.h>

#include <mlir/InitAllDialects.h>

using namespace mlir;

// TOOD: std::string is a temporary return type.
std::string IRGen::generate_ir() {
    DialectRegistry registry;
    registerAllDialects(registry);

    MLIRContext ctx(registry);

    OpBuilder builder(&ctx);

    auto module = ModuleOp::create(builder.getUnknownLoc());

    std::string output;
    llvm::raw_string_ostream os(output);
    module.print(os);
    module.erase();

    return output;
}