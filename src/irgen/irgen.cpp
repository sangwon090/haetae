#include <haetae/irgen/irgen.hpp>
#include <haetae/utils/overloaded.hpp>

#include <variant>


mlir::Type IRGen::convert_type(const DataType &dtype) {
    auto ret = std::visit(overloaded {
        [this](const Atomic &atomic) {
            // TODO: implement atomic, or remove this type from the spec?
            return mlir::Type { this->builder.getNoneType() };
        },
        [this](const Integer &integer) {
            return mlir::Type { this->builder.getIntegerType(integer.bits) };
        },
        [this](const Floating &floating) {
            if(floating.bits == 32) return mlir::Type { this->builder.getF32Type() };
            else if(floating.bits == 64) return mlir::Type { this->builder.getF64Type() };
            else return mlir::Type { this->builder.getF64Type() };
        },
        [this](const BrainFloating &bfloating) {
            // TODO: support bfloat type
            if(bfloating.bits == 32) return mlir::Type { this->builder.getF32Type() };
            else if(bfloating.bits == 64) return mlir::Type { this->builder.getF64Type() };
            else return mlir::Type { this->builder.getF64Type() };
        },
        [this](const Tensor &tensor) {
            llvm::SmallVector<int64_t> shape(tensor.dim.begin(), tensor.dim.end());
            DataType dtype = std::visit([](const auto &ntype) {
                return DataType(ntype);
            }, tensor.dtype);

            return mlir::Type { mlir::RankedTensorType::get(shape, convert_type(dtype)) };
        },
        [this](const Boolean &boolean) {
            return mlir::Type { this->builder.getI1Type() };
        }
    }, dtype);

    return ret;
}

mlir::Value IRGen::convert_literal(const Literal &literal) {
    return std::visit(overloaded {
        [this](const AtomLiteral &atomic) -> mlir::Value {
            // TODO: implement atomic, or remove this type from the spec?
            auto type = builder.getIntegerType(32);
            auto attr = builder.getIntegerAttr(type, 0);
            return builder.create<mlir::arith::ConstantOp>(
                builder.getUnknownLoc(), attr
            ).getResult();
        },
        [this](const IntegerLiteral &integer) -> mlir::Value {
            auto type = builder.getIntegerType(integer.type.bits);
            auto attr = builder.getIntegerAttr(type, integer.value);
            return builder.create<mlir::arith::ConstantOp>(
                builder.getUnknownLoc(), attr
            ).getResult();
        },
        [this](const FloatingLiteral &floating) -> mlir::Value {
            auto type = (floating.type.bits == 32) ? builder.getF32Type() : builder.getF64Type();
            auto attr = builder.getFloatAttr(type, floating.value);
            return builder.create<mlir::arith::ConstantOp>(
                builder.getUnknownLoc(), attr
            ).getResult();
        },
        [this](const BooleanLiteral &boolean) -> mlir::Value {
            auto type = builder.getI1Type();
            auto attr = builder.getIntegerAttr(type, boolean.value);
            return builder.create<mlir::arith::ConstantOp>(
                builder.getUnknownLoc(), attr
            ).getResult();
        },
        [this](const StringLiteral &s) -> mlir::Value {
            // TODO: implement string
            auto type = builder.getIntegerType(32);
            auto attr = builder.getIntegerAttr(type, 0);
            return builder.create<mlir::arith::ConstantOp>(
                builder.getUnknownLoc(), attr
            ).getResult();
        }
    }, literal);
}

std::expected<mlir::Value, IRGenError> IRGen::convert_expr(Expr &expr) {
    auto ret = std::visit(overloaded {
        [&](const UnaryExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] unimplemented!"));
        },
        [&](const InfixExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] unimplemented!"));
        },
        [&](const FnDefExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] nested fn def is undefined behavior."));
        },
        [&](const FnCallExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] unimplemented!"));
        },
        [&](const IdentExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return std::unexpected(IRGenError("[IRGen::convert_expr] unimplemented!"));
        },
        [&](const LiteralExpr& e) -> std::expected<mlir::Value, IRGenError> {
            return convert_literal(e.value);
        },
    }, expr.variant);

    if(!ret) return std::unexpected(ret.error());
    return *ret;
}

std::expected<void, IRGenError> IRGen::convert_fndef(FnDefExpr &fndef) {
    std::unordered_map<std::string, mlir::Value> values;
    llvm::SmallVector<mlir::Type> arg_types;

    for(auto &[name, dtype] : fndef.args) {
        arg_types.push_back(convert_type(dtype));
    }

    mlir::Type ret_type = convert_type(fndef.rtype);
    auto fn_type = builder.getFunctionType(arg_types, { ret_type });

    builder.setInsertionPointToEnd(module->getBody());
    auto fn = builder.create<mlir::func::FuncOp>(
        builder.getUnknownLoc(),
        fndef.name.value,
        fn_type
    );

    auto &entry_block = *fn.addEntryBlock();
    builder.setInsertionPointToStart(&entry_block);

    for(auto [arg_decl, block_arg] : llvm::zip(fndef.args, entry_block.getArguments())) {
        values[std::get<0>(arg_decl).value] = block_arg;
    }

    mlir::Value last_val;
    for(auto &expr : fndef.body) {
        auto val = convert_expr(expr);
        // FIXME: temporary workaround for incomplete values returned by convert_expr
        if(!val) continue; //return std::unexpected(val.error());
        last_val = *val;
    }

    builder.create<mlir::func::ReturnOp>(builder.getUnknownLoc(), last_val);

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
        mlir::func::FuncDialect,
        mlir::arith::ArithDialect,
        mlir::tensor::TensorDialect,
        mlir::linalg::LinalgDialect,
        mlir::scf::SCFDialect,
        mlir::affine::AffineDialect,
        mlir::memref::MemRefDialect
    >();

    this->module = mlir::ModuleOp::create(builder.getUnknownLoc());
    this->convert_all();

    std::string output;
    llvm::raw_string_ostream os(output);
    this->module->print(os);
    this->module->erase();

    return output;
}
