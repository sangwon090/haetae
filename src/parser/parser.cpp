#include <haetae/parser/parser.hpp>
#include <haetae/utils/overloaded.hpp>

#include <optional>
#include <expected>
#include <variant>

#define MAX_SHAPES 100

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

std::expected<DataType, ParserError> Parser::parse_dtype(bool is_shape) {
    auto opt_tok = this->next(0);
    if(!opt_tok) return std::unexpected(ParserError("[Parser::parse_dtype] insufficient tokens"));

    if(auto* dtype_ptr = std::get_if<DataType>(&opt_tok->variant)) {
        return std::visit(overloaded {
            [this, &is_shape](const Tensor& tensor) -> std::expected<DataType, ParserError> {
                if(is_shape) return std::unexpected(ParserError("[Parser::parse_dtype] nested tensor rank is impossible"));

                this->pos += 1;
                auto token = this->next(0);

                // parse TokenKind::Less
                if(!token) return std::unexpected(ParserError("[Parser::parse_dtype] insufficient tokens"));
                if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
                    if(*tok_ptr != TokenKind::Less) return std::unexpected(ParserError("[Parser::parse_dtype] expected TokenKind::Less but not found"));

                    this->pos += 1;
                    token = this->next(0);
                }

                // parse data type
                auto dtype = this->parse_dtype(true);
                if(!dtype) return dtype;
                token = this->next(0);


                auto ntype = std::visit(overloaded {
                    [](const Integer& integer) -> std::expected<NumericType, ParserError> {
                        return NumericType { integer };
                    },
                    [](const Floating& floating) -> std::expected<NumericType, ParserError> {
                        return NumericType { floating };
                    },
                    [](const BrainFloating& bfloating) -> std::expected<NumericType, ParserError> {
                        return NumericType { bfloating };
                    },
                    [](const auto& any) -> std::expected<NumericType, ParserError> {
                        return std::unexpected(ParserError("[Parser::parse_dtype] tensor data type must be one of numeric types"));
                    }
                }, *dtype);
                if(!ntype) return std::unexpected(ntype.error());

                if(!token) return std::unexpected(ParserError("[Parser::parse_dtype] insufficient tokens"));
                if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
                    if(*tok_ptr != TokenKind::Semicolon) return std::unexpected(ParserError("[Parser::parse_dtype] expected TokenKind::Semicolon but not found"));

                    this->pos += 1;
                    token = this->next(0);
                } else return std::unexpected(ParserError("[Parser::parse_dtype] TokenKind::Semicolon expected but not found"));

                // parse shape
                std::vector<int> shape;
                while(shape.size() < MAX_SHAPES) {
                    size_t size = 0;

                    if(!token) return std::unexpected(ParserError("[Parser::parse_dtype] insufficient tokens"));
                    if(auto *lit_ptr = std::get_if<Literal>(&token.value().variant)) {
                        if(auto *int_ptr = std::get_if<IntegerLiteral>(lit_ptr)) {
                            size = (size_t) int_ptr->value;
                            this->pos += 1;
                            token = this->next(0);
                        } else {
                            return std::unexpected(ParserError("[Parser::parse_dtype] expected IntegerLiteral but something else found"));
                        }
                    }

                    shape.push_back(size);

                    if(!token) return std::unexpected(ParserError("[Parser::parse_dtype] insufficient tokens"));
                    if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
                        if(*tok_ptr == TokenKind::Comma) {
                            this->pos += 1;
                            token = this->next(0);
                            continue;
                        } else if(*tok_ptr == TokenKind::Greater) {
                            this->pos += 1;
                            token = this->next(0);
                            break;
                        } else return std::unexpected(ParserError("[Parser::parse_dtype] unexpected token found"));
                    }
                }

                return DataType{ Tensor { *ntype, shape } };
            },
            [this](const auto& etc) -> std::expected<DataType, ParserError> {
                this->pos += 1;
                return etc;
            }
        }, *dtype_ptr);
        
    } else return std::unexpected(ParserError("[Parser::parse_dtype] expected DataType, but something else found."));
}