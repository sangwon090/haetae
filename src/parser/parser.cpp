#include <haetae/parser/parser.hpp>
#include <haetae/utils/overloaded.hpp>

#include <optional>
#include <expected>

std::optional<Token> Parser::next(int n) {
    if(this->pos + n < this->tokens.size()) {
        return this->tokens[this->pos + n];
    } else {
        return std::nullopt;
    }
}

std::expected<Expr, ParserError> Parser::parse() {
    auto next = this->next(0);
    if(!next) return std::unexpected(ParserError("[Parser::parse] token expected but not found."));

    return std::visit(overloaded {
        [this](const Keyword& kw) -> std::expected<Expr, ParserError> {
            switch(kw) {
                case Keyword::Fn:
                    return this->parse_fn();
            }

            return this->parse_expr(Precedence::Lowest);
        },
        [this](const auto& other) {
            return this->parse_expr(Precedence::Lowest);
        }
    }, next->variant);
}

std::expected<AST, ParserError> Parser::get_ast() {
    AST ret;

    while(auto opt_tok = this->next(0)) {
        if(auto* kind_ptr = std::get_if<TokenKind>(&opt_tok->variant)) {
            
            switch(*kind_ptr) {
                case TokenKind::EndOfFile:
                    return ret;
                
                case TokenKind::Newline:
                    this->pos += 1;
                    continue;
            }
        } 

        auto expr = this->parse();
        if(!expr) return std::unexpected(expr.error());
        ret.exprs.push_back(std::move(*expr));
    }

    return ret;
}
