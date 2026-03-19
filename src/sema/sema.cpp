#include <haetae/sema/sema.hpp>

std::expected<AST, SemaError> Sema::analyze() {
    for(auto &expr : this->ast.exprs) {

    }
    
    return std::move(this->ast);
}