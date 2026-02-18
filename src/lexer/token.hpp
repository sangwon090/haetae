#pragma once

#include <string>
#include <variant>

struct Identifier { std::string value; };

enum class Keyword {
    Let,
    Fn,
    Module,
};

struct AtomLiteral { std::string val; };
struct IntegerLiteral { int val; };
struct FloatingLiteral { long double val; };
struct StringLiteral { std::string val; };
struct BooleanLiteral { bool val; };

using Literal = std::variant<
    AtomLiteral,
    IntegerLiteral,
    FloatingLiteral,
    StringLiteral,
    BooleanLiteral
>;

enum class TokenKind {
    LParen,             // (
    RParen,             // )
    LSqBr,              // [
    RSqBr,              // ]
    Colon,              // :
    Comma,              // ,
    Semicolon,          // ;
    Plus,               // +
    Minus,              // -
    Star,               // *
    Slash,              // /
    VBar,               // |
    Ampersand,          // &
    Less,               // <
    Greater,            // >
    Equal,              // =
    Dot,                // .
    Percent,            // %
    LBrace,             // {
    RBrace,             // }
    Not,                // !
    EqualEqual,         // ==
    NotEqual,           // !=
    LessEqual,          // <=
    GreaterEqual,       // >=
    Tilde,              // ~
    Circumflex,         // ^
    LeftShift,          // <<
    RightShift,         // >>
//  DoubleStar,         // **
//  DoubleSlash,        // //
//  At,                 // @
//  AtEqual,            // @=
    RArrow,             // ->
//  Ellipsis,           // ...
//  ColonEqual,         // :=
    VBarRArrow,         // |>
    TildeRArrow,        // ~>

    Newline,
    Whitespace,
    EndOfFile
};

using TokenVariant = std::variant<
    TokenKind,
    Identifier,
    Keyword,
    Literal
>;

struct Token {
    TokenVariant variant;
    int start;
    int end;
};