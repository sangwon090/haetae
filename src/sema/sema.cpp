#include <haetae/lexer/token.hpp>
#include <haetae/lexer/datatype.hpp>
#include <haetae/sema/sema.hpp>
#include <haetae/utils/overloaded.hpp>

std::expected<DataType, SemaError> Sema::check_dtype(Expr* expr, SymbolTable& parent_symbols) {
    SymbolTable symbols(parent_symbols);

    auto ret = std::visit(overloaded {
        [&](UnaryExpr& unary) -> std::expected<DataType, SemaError> {
            return this->check_dtype(unary.right.get(), symbols);
        },
        [&](InfixExpr& infix) -> std::expected<DataType, SemaError> {
            auto left = this->check_dtype(infix.left.get(), symbols);
            auto right = this->check_dtype(infix.right.get(), symbols);

            if(!left) return std::unexpected(left.error());
            if(!right) return std::unexpected(right.error());
            if(*left != *right) return std::unexpected(SemaError("[Sema::check_dtype] infix op type not matching"));

            return *left;
        },
        [&](FnDefExpr& fndef) -> std::expected<DataType, SemaError> {
            DataType last_dtype;

            for(auto &expr : fndef.body) {
                auto dtype = this->check_dtype(&expr, symbols);
                if(!dtype) return std::unexpected(dtype.error());

                last_dtype = *dtype;
            }

            if(last_dtype != fndef.rtype) {
                return std::unexpected(SemaError("[Sema::check_dtype] fn return type not macthing"));
            } else return last_dtype;
        },
        [&](FnCallExpr& fncall) -> std::expected<DataType, SemaError> {
            auto symbol = symbols.lookup(fncall.ident);
            if(!symbol) return std::unexpected(SemaError("[Sema::check_dtype] function not found"));
            
            if(auto *fn_sym = std::get_if<FnSymbol>(symbol)) {
                return fn_sym->rtype;         
            } else return std::unexpected(SemaError("[Sema::check_dtype] not a valid function"));
        },
        [&](IdentExpr& ident) -> std::expected<DataType, SemaError> {
            auto symbol = symbols.lookup(ident.value);
            if(!symbol) return std::unexpected(SemaError("[Sema::check_dtype] indentifier not found"));

            if(auto *ident_sym = std::get_if<VarSymbol>(symbol)) {
                return ident_sym->dtype;
            } else return std::unexpected(SemaError("[Sema::check_dtype] not a valid identifier"));
        },
        [&](LiteralExpr& literal) -> std::expected<DataType, SemaError> {
            return get_dtype_from_literal(literal.value);
        }
    }, expr->variant);

    if(!ret) return std::unexpected(ret.error());
    expr->sema_info = SemaInfo { *ret };

    return *ret;
}

std::expected<AST, SemaError> Sema::analyze_all() {
    SymbolTable symbols;
    
    // update the function symbols first
    for(auto &expr : this->ast.exprs) {
        if(auto* fndef_ptr = std::get_if<FnDefExpr>(&expr.variant)) {
            if(symbols.find(fndef_ptr->name)) {
                return std::unexpected(SemaError("[Sema] function name duplicated."));
            }

            symbols.insert(fndef_ptr->name, FnSymbol { fndef_ptr->args, fndef_ptr->rtype });
        } else {
            continue;
        }
    }

    for(auto &expr : this->ast.exprs) {
        auto dtype = this->check_dtype(&expr, symbols);
        if(!dtype) return std::unexpected(dtype.error());
    }
    
    this->symbols = std::move(symbols);
    return std::move(this->ast);
}