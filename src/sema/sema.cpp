#include <haetae/sema/sema.hpp>
#include <haetae/utils/overloaded.hpp>

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

    this->symbols = std::move(symbols);
    return std::move(this->ast);
}