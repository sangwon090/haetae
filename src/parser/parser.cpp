#include "parser.hpp"
#include "expr.hpp"
#include "../utils/overloaded.hpp"

#include <stdexcept>
#include <variant>

Token* Parser::next(int n) {
    if(this->pos + n < this->tokens.size()) {
        return &this->tokens[this->pos + n];
    } else {
        return nullptr;
    }
}

std::optional<Operator> Parser::get_operator(Token tok, bool is_prefix) {
    if(auto* kind_ptr = std::get_if<TokenKind>(&tok.variant)) {
        if(is_prefix) {
            switch(*kind_ptr) {
                case TokenKind::Plus:
                    return Operator::UnaryPlus;
                
                case TokenKind::Minus:
                    return Operator::UnaryMinus;
                
                case TokenKind::Tilde:
                    return Operator::BitwiseNot;
            }
        } else {
            switch(*kind_ptr) {
                case TokenKind::Plus:
                    return Operator::Plus;

                case TokenKind::Minus:
                    return Operator::Minus;

                case TokenKind::Slash:
                    return Operator::Divide;

                case TokenKind::Star:
                    return Operator::Multiply;

                case TokenKind::EqualEqual:
                    return Operator::Equal;

                case TokenKind::NotEqual:
                    return Operator::NotEqual;

                case TokenKind::Less:
                    return Operator::Less;
                    
                case TokenKind::LessEqual:
                    return Operator::LessEqual;

                case TokenKind::Greater:
                    return Operator::Greater;

                case TokenKind::GreaterEqual:
                    return Operator::GreaterEqual;

                case TokenKind::Percent:
                    return Operator::Modulo;

                case TokenKind::Ampersand:
                    return Operator::BitwiseAnd;

                case TokenKind::Circumflex:
                    return Operator::BitwiseXor;

                case TokenKind::VBar:
                    return Operator::BitwiseOr;

                case TokenKind::LParen:
                    return Operator::FnCall;

                case TokenKind::VBarRArrow:
                    return Operator::PipelineHost;

                case TokenKind::TildeRArrow:
                    return Operator::PipelineDevice;
            }
        }
    }

    return std::nullopt;
}

std::optional<Precedence> Parser::get_precedence(Operator op) {
    switch (op) {
        case Operator::Plus:
        case Operator::Minus:
            return Precedence::PlusMinus;

        case Operator::Multiply:
        case Operator::Divide:
        case Operator::Modulo:
            return Precedence::MulDivMod;

        case Operator::BitwiseAnd:
            return Precedence::BitwiseAnd;

        case Operator::BitwiseOr:
            return Precedence::BitwiseOr;

        case Operator::BitwiseXor:
            return Precedence::BitwiseXor;

        case Operator::Less:
        case Operator::LessEqual:
        case Operator::Greater:
        case Operator::GreaterEqual:
            return Precedence::LessGreater;

        case Operator::Equal:
        case Operator::NotEqual:
            return Precedence::EqualNotEqual;

        case Operator::FnCall:
            return Precedence::FnCall;

        case Operator::PipelineDevice:
        case Operator::PipelineHost:
            return Precedence::Pipeline;
    }

    return std::nullopt;
}

std::expected<Expr, ParserError> Parser::parse_prefix() {
    auto opt_tok = this->next(0);
    if(!opt_tok) return std::unexpected(ParserError("[Parser::parse_prefix] token expected but not found"));

    return std::visit(overloaded {
        [this](const Identifier& ident) -> std::expected<Expr, ParserError> {
            this->pos += 1;
            return Expr {
                IdentExpr { ident.value }
            };
        },
        [this](const Literal& lit) -> std::expected<Expr, ParserError> {
            this->pos += 1;
            return Expr { 
                LiteralExpr { std::move(lit) }
            };
        },
        [this](const TokenKind& tok) -> std::expected<Expr, ParserError> {
            switch(tok) {
                case TokenKind::Plus:
                case TokenKind::Minus:
                case TokenKind::Tilde: {
                    return this->parse_nud();
                }

                case TokenKind::LParen: {
                    this->pos += 1;
                    auto opt_expr = this->parse_expr(Precedence::Lowest);
                    if(!opt_expr) return std::unexpected(ParserError("[Parser::parse_prefix] expected expr but nothing found"));

                    auto opt_next = this->next(0);
                    if(!opt_next) return std::unexpected(ParserError("[Parser::parse_prefix] expected token but nothing found"));

                    if(auto* kind_ptr = std::get_if<TokenKind>(&opt_next->variant)) {                    
                        switch(*kind_ptr) {
                            case TokenKind::RParen:
                                this->pos += 1;
                                return std::move(*opt_expr);

                            default:
                                return std::unexpected(ParserError("[Parser::parse_prefix] expected RParen but something else found"));
                        }
                    } else {
                        return std::unexpected(ParserError("[Parser::parse_prefix] expected TokenKind but something else found"));
                    }
                }
                
                default:
                    return std::unexpected(ParserError("[Parser::parse_prefix] expected LParen but something else found"));
            }
        },
        [this](const Keyword& kw) -> std::expected<Expr, ParserError> {
            return std::unexpected(ParserError("[Parser::parse_prefix] unexpected keyword found"));
        },
    }, opt_tok->variant);
}

#include <iostream>
std::expected<Expr, ParserError> Parser::parse_expr(Precedence prec) {
    auto expr_left = this->parse_prefix();
    if(!expr_left) return std::unexpected(expr_left.error());

    while(1) {
        auto opt_tok = this->next(0);
        if(!opt_tok) return std::move(expr_left);

        if(auto* kind_ptr = std::get_if<TokenKind>(&opt_tok->variant)) {
            if(*kind_ptr == TokenKind::EndOfFile || *kind_ptr == TokenKind::Newline) {
                return std::move(expr_left);
            }

            auto op = Parser::get_operator(*opt_tok, false);
            if(!op) return std::move(expr_left);

            if(prec >= Parser::get_precedence(*op)) {
                return std::move(expr_left);
            }

            auto expr_next = this->parse_led(std::move(*expr_left));
            if(!expr_next) return std::unexpected(expr_next.error());

            expr_left = std::move(expr_next);
        } else {
            return std::move(expr_left);
        }


    } 

    return std::unexpected(ParserError("[Parser::parse_expr] unreachable code reached"));
}

std::expected<Expr, ParserError> Parser::parse_nud() {
    auto opt_tok = this->next(0);
    if(!opt_tok) return std::unexpected(ParserError("[Parser::parse_nud] no next token"));

    auto op = get_operator(*opt_tok, true);
    if(!op) return std::unexpected(ParserError("[Parser::parse_nud] expected prefix operator but not found"));

    if(auto* kind_ptr = std::get_if<TokenKind>(&opt_tok->variant)) {
        this->pos += 1;

        auto expr_right = this->parse_expr(Precedence::Unary);
        if(!expr_right) return std::unexpected(ParserError("[Parser::parse_nud] cannot parse right expr"));

        return Expr {
            UnaryExpr {
                *op,
                std::make_unique<Expr>(std::move(*expr_right))
            }
        };
    } else {
        return std::unexpected(ParserError("[Parser::parse_nud] expected TokenKind but not found"));
    }
}

std::expected<Expr, ParserError> Parser::parse_led(Expr left) {
    auto opt_tok = this->next(0);
    if(!opt_tok) return std::unexpected(ParserError("[Parser::parse_led] token expected but nothing found"));

    auto op = Parser::get_operator(*opt_tok, false);
    if(!op) return std::unexpected(ParserError("[Parser::parse_led] operator found but nothing found"));

    auto prec = Parser::get_precedence(*op);
    
    this->pos += 1;

    if(*op == Operator::FnCall) {
        auto expr = this->parse_fncall(std::move(left));
        if(!expr) return std::unexpected(expr.error());
    
        return std::move(*expr);
    }


    auto opt_right = this->parse_expr(*prec);
    if(!opt_right) return std::unexpected(opt_right.error());

    return Expr {
        InfixExpr {
            std::move(*op),
            std::make_unique<Expr>(std::move(left)),
            std::make_unique<Expr>(std::move(*opt_right))
        }
    };
}

std::expected<Expr, ParserError> Parser::parse_fncall(Expr left) {
    if(auto* ident = std::get_if<IdentExpr>(&left.variant)) {
        std::vector<std::unique_ptr<Expr>> args;

        auto next = this->next(0);
        if(!next) return std::unexpected(ParserError("[Parser::parse_fncall] no tokens"));

        if(auto *tok = std::get_if<TokenKind>(&next->variant)) {
            if(*tok == TokenKind::RParen) {
                this->pos += 1;
                return Expr { FnCallExpr { ident->value, std::move(args) } };
            }
        } else {
            return std::unexpected(ParserError("[Parser::parse_fncall] expected ident but not found"));
        }

        while(1) {
            auto arg = this->parse_expr(Precedence::Lowest);
            if(!arg) return std::unexpected(arg.error());
            args.push_back(std::make_unique<Expr>(std::move(*arg))); 

            auto next = this->next(0);
            if(!next) {
                return std::unexpected(ParserError("[Parser::parse_fncall] expected ')' but reached EOF"));
            }

            if(auto* tok = std::get_if<TokenKind>(&next->variant)) {
                if(*tok == TokenKind::Comma) {
                    this->pos += 1;
                } else if(*tok == TokenKind::RParen) {
                    this->pos += 1;
                    break;
                } else {
                    return std::unexpected(ParserError("[Parser::parse_fncall] unexpected token found"));
                }
            } else {
                return std::unexpected(ParserError("[Parser::parse_fncall] expected token kind after argument"));
            }
        }

        return Expr { FnCallExpr { ident->value, std::move(args) } };
    } else {
        return std::unexpected(ParserError("[Parser::parse_fncall] expected ident but not found"));
    }

    return std::unexpected(ParserError("not implemeneted"));
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

        auto expr = this->parse_expr(Precedence::Lowest);
        if(!expr) return std::unexpected(expr.error());
        ret.exprs.push_back(std::move(*expr));
    }

    return ret;
}
