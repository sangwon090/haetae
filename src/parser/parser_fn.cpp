#include <haetae/parser/parser.hpp>
#include <haetae/lexer/datatype.hpp>

#include <iostream>
#include <variant>
#include <expected>
#include <tuple>

#define MAX_ARGS 1000

std::expected<Expr, ParserError> Parser::parse_fn() {
    auto token = this->next(0);

    Identifier fn_name;
    std::vector<std::tuple<Identifier, DataType>> args;
    DataType rtype;
    std::vector<Expr> fn_body;


    // parse Keyword::fn
    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *kwd_ptr = std::get_if<Keyword>(&token.value().variant)) {
        if(*kwd_ptr != Keyword::Fn) return std::unexpected(ParserError("[Parser::parse_fn] expected Keyword::Fn"));
        this->pos += 1;
        token = this->next(0);
    } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));


    // parse function name
    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *ident_ptr = std::get_if<Identifier>(&token.value().variant)) {
        if(ident_ptr) fn_name = *ident_ptr;
        else return std::unexpected(ParserError("[Parser::parse_fn] function name is not an Identifier"));

        this->pos += 1;
        token = this->next(0);
    } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));


    // parse args
    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
        if(*tok_ptr != TokenKind::LParen) return std::unexpected(ParserError("[Parser::parse_fn] expected Token::LParen"));
        this->pos += 1;
        token = this->next(0);
    } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));

    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) { // early break with no args
        if(*tok_ptr != TokenKind::RParen) return std::unexpected(ParserError("[Parser::parse_fn] expected Token::RParen"));
        this->pos += 1;
        token = this->next(0);
    } else { // parse all the args
        while(args.size() < MAX_ARGS) {
            Identifier arg_name;
            DataType arg_dtype;

            if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
            if(auto *ident_ptr = std::get_if<Identifier>(&token.value().variant)) {
                if(ident_ptr) arg_name = *ident_ptr;
                else return std::unexpected(ParserError("[Parser::parse_fn] arg name is not an Identifier"));

                this->pos += 1;
                token = this->next(0);
            }

            if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
            if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
                if(*tok_ptr != TokenKind::Colon) return std::unexpected(ParserError("[Parser::parse_fn] expected Token::Colon"));

                this->pos += 1;
                token = this->next(0);
            } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));

            auto dtype = this->parse_dtype();
            if(!dtype) return std::unexpected(dtype.error());
            token = this->next(0);

            arg_dtype = *dtype;
            args.push_back({ arg_name, arg_dtype });


            if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
            if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
                if(*tok_ptr == TokenKind::RParen) {
                    this->pos += 1;
                    token = this->next(0);
                    break;
                } else if(*tok_ptr == TokenKind::Comma) {
                    this->pos += 1;
                    token = this->next(0);
                } else return std::unexpected(ParserError("[Parser::parse_fn] expected Token::RParen"));
            } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));

        }

        if(args.size() >= MAX_ARGS) return std::unexpected(ParserError("[Parser::parse_fn] max args exceeded. there might be an infinite loop caused by a parser bug."));
    }


    // parse TokenKind::RArrow
    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
        if(*tok_ptr != TokenKind::RArrow) return std::unexpected(ParserError("[Parser::parse_fn] expected TokenKind::RArrow"));
        this->pos += 1;
        token = this->next(0);
    } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));

    // parse return type
    auto dtype = this->parse_dtype();
    if(!dtype) return std::unexpected(dtype.error());
    rtype = *dtype;
    token = this->next(0);

    // parse TokenKind::LBrace
    if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
    if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
        if(*tok_ptr != TokenKind::LBrace) return std::unexpected(ParserError("[Parser::parse_fn] expected TokenKind::LBrace"));
        this->pos += 1;
        token = this->next(0);
    } else return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));


    // parse function body
    while(1) {
        if(!token) return std::unexpected(ParserError("[Parser::parse_fn] insufficient tokens"));
        if(auto *tok_ptr = std::get_if<TokenKind>(&token.value().variant)) {
            if(*tok_ptr == TokenKind::RBrace) { 
                this->pos += 1;
                token = this->next(0);
                break;
            } else if(*tok_ptr == TokenKind::Newline) {
                this->pos += 1;
                token = this->next(0);
                continue;
            }
        }
        
        auto expr = this->parse_expr();
        if(expr) fn_body.push_back(std::move(*expr));
        else return expr;

        token = this->next(0);
    }

    return Expr{ std::move(FnDefExpr(Identifier(fn_name), std::move(args), rtype, std::move(fn_body))) };
}