
#include <string>
#include <variant>

struct Identifier { std::string value; };
struct Keyword { std::string name; };
struct Literal { std::string value; };

enum TokenKind {
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
    TokenVariant kind;
    int start;
    int end;
};