#include <haetae/irgen/irgen.hpp>
#include <haetae/utils/overloaded.hpp>

#include <variant>

using namespace mlir;

mlir::Type IRGen::convert_type(const DataType &dtype) {
    auto ret = std::visit(overloaded {
        [this](const Atomic &atomic) {
            // TODO: implement atomic
           return Type { this->builder.getNoneType() };
        },
        [this](const Integer &integer) {
            return Type { this->builder.getIntegerType(integer.bits) };
        },
        [this](const Floating &floating) {
            if(floating.bits == 32) return Type { this->builder.getF32Type() };
            else if(floating.bits == 64) return Type { this->builder.getF64Type() };
            else return Type { this->builder.getF64Type() };
        },
        [this](const BrainFloating &bfloating) {
            // TODO: support bfloat type
            if(bfloating.bits == 32) return Type { this->builder.getF32Type() };
            else if(bfloating.bits == 64) return Type { this->builder.getF64Type() };
            else return Type { this->builder.getF64Type() };
        },
        [this](const Tensor &tensor) {
            // TODO: implement tensor
           return Type { this->builder.getNoneType() };
        },
        [this](const Boolean &boolean) {
            return Type { this->builder.getI1Type() };
        }
    }, dtype);

    return ret;
}

std::expected<void, IRGenError> IRGen::convert_expr(Expr &expr) {
    return std::visit(overloaded {
        [&](const FnDefExpr& e) -> std::expected<void, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] nested fn def is undefined behavior."));
        },
        [&](const auto& others) -> std::expected<void, IRGenError> {
            return {};
        }
    }, expr.variant);
}

std::expected<void, IRGenError> IRGen::convert_fndef(FnDefExpr &fndef) {
    llvm::SmallVector<Type> arg_types;

    for(auto &[name, dtype] : fndef.args) {
        arg_types.push_back(convert_type(dtype));
    }

    Type ret_type = convert_type(fndef.rtype);
    auto fn_type = builder.getFunctionType(arg_types, { ret_type });

    builder.setInsertionPointToEnd(module->getBody());
    auto fn = builder.create<func::FuncOp>(
        builder.getUnknownLoc(),
        fndef.name.value,
        fn_type
    );


    return {};
}

std::expected<void, IRGenError> IRGen::convert_all() {
    for(auto &expr : this->ast.exprs) {
        auto result = std::visit(overloaded {
            [&](FnDefExpr& e) -> std::expected<void, IRGenError> {
                return this->convert_fndef(e);
            },
            [&](auto& others) -> std::expected<void, IRGenError> {
                return {};
            }
        }, expr.variant);

        if(!result) return result;
    }
    return {};
}

// TOOD: std::string is a temporary return type.
std::string IRGen::generate_ir() {
    this->ctx.loadDialect<
        func::FuncDialect,
        arith::ArithDialect,
        tensor::TensorDialect,
        linalg::LinalgDialect,
        scf::SCFDialect,
        affine::AffineDialect,
        memref::MemRefDialect
    >();

    this->module = ModuleOp::create(builder.getUnknownLoc());
    this->convert_all();

    std::string output;
    llvm::raw_string_ostream os(output);
    this->module->print(os);
    this->module->erase();

    return output;
}
