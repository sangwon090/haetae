#include "parser.hpp"

std::optional<Token> Parser::next(int n) {
    if(this->pos + n < this->tokens.size()) {
        return this->tokens[this->pos + n];
    } else {
        return std::nullopt;
    }
}

AST Parser::get_ast() {
    AST ret;

    while(auto opt_tok = this->next(0)) {
        if(auto* kind_ptr = std::get_if<TokenKind>(&opt_tok->kind)) {
            
            switch(*kind_ptr) {
                case TokenKind::EndOfFile:
                    return ret;
                
                case TokenKind::Newline:
                    this->pos += 1;
                    continue;
            }
        }

        // temporary code to avoid infinite loop
        // TODO: parse expr
        this->pos += 1;
    }

    return ret;
}