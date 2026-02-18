#pragma once

#include <string>
#include <variant>
#include <iostream>

struct Identifier { std::string value; };

enum class Keyword {
    Let,
    Fn,
    Module,
};

inline std::ostream& operator<<(std::ostream& os, const Identifier& ident) {
    return os << "Identifier(" << ident.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Keyword& keyword) {
    os << "Keyword(";

    switch(keyword) {
        case Keyword::Let:
            os << "Let";
            break;
        case Keyword::Fn:
            os << "Fn";
            break;
        case Keyword::Module:
            os << "Module";
            break;
        default:
            os << "Unknown";
            break;
    }

    os << ")";

    return os;
}



struct AtomLiteral { std::string value; };
struct IntegerLiteral { int value; };
struct FloatingLiteral { long double value; };
struct StringLiteral { std::string value; };
struct BooleanLiteral { bool value; };

using Literal = std::variant<
    AtomLiteral,
    IntegerLiteral,
    FloatingLiteral,
    StringLiteral,
    BooleanLiteral
>;

inline std::ostream& operator<<(std::ostream& os, const AtomLiteral& literal) {
    return os << "Atom(" << literal.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const IntegerLiteral& literal) {
    return os << "Integer(" << literal.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const FloatingLiteral& literal) {
    return os << "Floating(" << literal.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const StringLiteral& literal) {
    return os << "String(" << literal.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const BooleanLiteral& literal) {
    return os << "Boolean(" << literal.value << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Literal& literal) {
    std::visit([&](const auto& value) {
        os << value;
    }, literal);

    return os;
}



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

inline std::ostream& operator<<(std::ostream& os, const TokenKind& token) {
    os << "TokenKind(";

    switch(token) {
        case TokenKind::LParen:
            os << "LParen";
            break;
        case TokenKind::RParen:
            os << "RParen";
            break;
        case TokenKind::LSqBr:
            os << "LSqBr";
            break;
        case TokenKind::RSqBr:
            os << "RSqBr";
            break;
        case TokenKind::Colon:
            os << "Colon";
            break;
        case TokenKind::Comma:
            os << "Comma";
            break;
        case TokenKind::Semicolon:
            os << "Semicolon";
            break;
        case TokenKind::Plus:
            os << "Plus";
            break;
        case TokenKind::Minus:
            os << "Minus";
            break;
        case TokenKind::Star:
            os << "Star";
            break;
        case TokenKind::Slash:
            os << "Slash";
            break;
        case TokenKind::VBar:
            os << "VBar";
            break;
        case TokenKind::Ampersand:
            os << "Ampersand";
            break;
        case TokenKind::Less:
            os << "Less";
            break;
        case TokenKind::Greater:
            os << "Greater";
            break;
        case TokenKind::Equal:
            os << "Equal";
            break;
        case TokenKind::Dot:
            os << "Dot";
            break;
        case TokenKind::Percent:
            os << "Percent";
            break;
        case TokenKind::LBrace:
            os << "LBrace";
            break;
        case TokenKind::RBrace:
            os << "RBrace";
            break;
        case TokenKind::Not:
            os << "Not";
            break;
        case TokenKind::EqualEqual:
            os << "EqualEqual";
            break;
        case TokenKind::NotEqual:
            os << "NotEqual";
            break;
        case TokenKind::LessEqual:
            os << "LessEqual";
            break;
        case TokenKind::GreaterEqual:
            os << "GreaterEqual";
            break;
        case TokenKind::Tilde:
            os << "Tilde";
            break;
        case TokenKind::Circumflex:
            os << "Circumflex";
            break;
        case TokenKind::LeftShift:
            os << "LeftShift";
            break;
        case TokenKind::RightShift:
            os << "RightShift";
            break;
        case TokenKind::RArrow:
            os << "RArrow";
            break;
        case TokenKind::VBarRArrow:
            os << "VBarRArrow";
            break;
        case TokenKind::TildeRArrow:
            os << "TildeRArrow";
            break;
        case TokenKind::Newline:
            os << "Newline";
            break;
        case TokenKind::Whitespace:
            os << "Whitespace";
            break;
        case TokenKind::EndOfFile:
            os << "EndOfFile";
            break;
        default:
            os << "Unknown";
            break;
    }

    os << ")";

    return os;
}



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


inline std::ostream& operator<<(std::ostream& os, const TokenVariant& variant) {
    std::visit([&](const auto& value) {
        os << value;
    }, variant);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "Token(variant=" << token.variant << ", start=" << token.start << ", end=" << token.end << ")";
}