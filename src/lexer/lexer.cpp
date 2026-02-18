#include <haetae/lexer/lexer.hpp>
#include <haetae/lexer/token.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <cctype>

std::string Lexer::load_file(std::ifstream file) {
    std::string content;

    if(file.is_open()) {
        std::stringstream ss;

        ss << file.rdbuf();
        content = ss.str();
    } else {
        throw std::runtime_error("[Lexer] ifstream is not opened");
    }

    return content;
}

Token Lexer::read_number() {
    int start = offset;
    std::string buf;
    bool is_float = false;

    while (offset < source.length()) {
        char ch = source[offset];

        if (std::isdigit(ch)) {
            buf += ch;
            offset += 1;
        } else if (ch == '.') {
            if(!is_float) {
                is_float = true;
                buf += ch;
                this->offset += 1;
            } else {
                throw std::runtime_error("[Lexer] unexpected dot found while parsing number");
            }
        } else {
            break;
        }
    }

    if(is_float) return Token { Literal { FloatingLiteral(std::stold(buf)) }, start, offset };
    else return Token { Literal { IntegerLiteral(std::stoi(buf)) }, start, offset };
}

Token Lexer::read_string() {
    std::string buf;
    int start = offset;

    offset += 1;

    while(offset < source.length()) {
        char ch = source[offset];
        offset += 1;

        switch(ch) {
            case '"':
                return {Literal { StringLiteral(buf) }, start, offset};
            
            default:
                buf += ch;
                break;
        }
    }

    throw std::runtime_error("[Lexer] closing quotation mark expected but not found");
}

Token Lexer::read_ident() {
    std::string buf;
    int start = offset;

    while(offset < source.length()) {
        char ch = source[offset];

        if((ispunct(ch) && ch != '_') ||
            ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\v' ) break;
        
        buf += ch;
        offset += 1;
    }

    // TODO: check for keywords

    return {Identifier{ buf }, start, offset};
}

Token Lexer::read_newline() {
    int start = offset;

    while(offset < source.length()) {
        char ch = source[offset];
        
        if(ch != '\n' && ch != '\r') break;
        offset += 1;
    }

    return {TokenKind::Newline, start, offset};
}

bool Lexer::is_continuable(Token token) {
    auto *kind_ptr = std::get_if<TokenKind>(&token.variant);
    if(!kind_ptr) return false;

    switch(*kind_ptr) {
        case TokenKind::Plus:
        case TokenKind::Minus:
        case TokenKind::Star:
        case TokenKind::Slash:
        case TokenKind::Percent:
        case TokenKind::Equal:
        case TokenKind::EqualEqual:
        case TokenKind::NotEqual:
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual:
        case TokenKind::Ampersand:
        case TokenKind::Circumflex:
        case TokenKind::VBar:
        case TokenKind::LeftShift:
        case TokenKind::RightShift:
        case TokenKind::VBarRArrow:
        case TokenKind::TildeRArrow:
        case TokenKind::Comma:
        case TokenKind::LParen:
        case TokenKind::LSqBr:
        case TokenKind::LBrace:
        case TokenKind::Colon:
        case TokenKind::RArrow:
            return true;
    }

    return false;
}

std::vector<Token> Lexer::get_tokens() {
    std::vector<Token> tokens;
    offset = 0;

    while(offset < source.length()) {
        char ch = source[offset];

        switch(ch) {
            case ' ':
            case '\t':
                offset += 1;
                break;
            
            case '\n':
            case '\r':
                if(!tokens.empty() && !is_continuable(tokens.back())) {
                    tokens.push_back(read_newline());
                } else {
                    read_newline();
                }

                break;
                
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                tokens.push_back(read_number());
                break;
            
            case '"':
                tokens.push_back(read_string());
                break;
            
            case '#':
                while(offset < source.length()) {
                    if(source[offset] == '\n' || source[offset] == '\r') break;
                    offset += 1;
                }
                break;
            
            case '(':
                tokens.push_back({TokenKind::LParen, offset, offset + 1});
                offset += 1;
                break;
            
            case ')':
                tokens.push_back({TokenKind::RParen, offset, offset + 1});
                offset += 1;
                break;

            case '[':
                tokens.push_back({TokenKind::LSqBr, offset, offset + 1});
                offset += 1;
                break;
            
            case ']':
                tokens.push_back({TokenKind::RSqBr, offset, offset + 1});
                offset += 1;
                break;
            
            case ',':
                tokens.push_back({TokenKind::Comma, offset, offset + 1});
                offset += 1;
                break;
            
            case ';':
                tokens.push_back({TokenKind::Semicolon, offset, offset + 1});
                offset += 1;
                break;
            
            case '+':
                tokens.push_back({TokenKind::Plus, offset, offset + 1});
                offset += 1;
                break;
            
            case ':':
                tokens.push_back({TokenKind::Colon, offset, offset + 1});
                offset += 1;
                break;
            
            case '-':
                if(offset + 1 < source.length() && source[offset+1] == '>') {
                    tokens.push_back({TokenKind::RArrow, offset, offset + 2 });
                    offset += 2;
                } else {
                    tokens.push_back({TokenKind::Minus, offset, offset + 1});
                    offset += 1;
                }
                break;

            case '*':
                tokens.push_back({TokenKind::Star, offset, offset + 1});
                offset += 1;
                break;
            
            case '/':
                tokens.push_back({TokenKind::Slash, offset, offset + 1});
                offset += 1;
                break;
            
            case '|':
                if(offset + 1 < source.length() && source[offset + 1] == '>') {
                    if(!tokens.empty()) {
                        if(auto *kind = std::get_if<TokenKind>(&tokens.back().variant)) {
                            if(*kind == TokenKind::Newline) tokens.pop_back();
                        }
                    }

                    tokens.push_back({TokenKind::VBarRArrow, offset, offset + 2});
                    offset += 2;
                } else {
                    tokens.push_back({TokenKind::VBar, offset, offset + 1});
                    offset += 1;
                }
                break;
            
            case '&':
                tokens.push_back({TokenKind::Ampersand, offset, offset + 1});
                offset += 1;
                break;
            
            case '<':
                if(offset + 1 < source.length()) {
                    if(source[offset+1] == '<') { tokens.push_back({TokenKind::LeftShift, offset, offset+2}); offset += 2; }
                    else if(source[offset+1] == '=') { tokens.push_back({TokenKind::LessEqual, offset, offset+2}); offset += 2; }
                    else {
                        tokens.push_back({TokenKind::Less, offset, offset + 1});
                        offset += 1;
                    }
                } else {
                    tokens.push_back({TokenKind::Less, offset, offset + 1});
                    offset += 1;
                }
                break;
            
            case '>':
                if(offset + 1 < source.length()) {
                    if(source[offset+1] == '>') { tokens.push_back({TokenKind::RightShift, offset, offset + 2}); offset += 2; }
                    else if(source[offset+1] == '=') { tokens.push_back({TokenKind::GreaterEqual, offset, offset + 2}); offset += 2; }
                    else {
                        tokens.push_back({TokenKind::Greater, offset, offset + 1});
                        offset += 1;
                    }
                } else {
                    tokens.push_back({TokenKind::Greater, offset, offset + 1});
                    offset += 1;
                }
                break;

            case '=':
                if(offset + 1 < source.length() && source[offset+1] == '=') {
                    tokens.push_back({TokenKind::EqualEqual, offset, offset + 2});
                    offset += 2;
                } else {
                    tokens.push_back({TokenKind::Equal, offset, offset + 1});
                    offset += 1;
                }
                break;
            
            case '.':
                tokens.push_back({TokenKind::Dot, offset, offset + 1});
                offset += 1;
                break;

            case '%':
                tokens.push_back({TokenKind::Percent, offset, offset + 1});
                offset += 1;
                break;
            
            case '{':
                tokens.push_back({TokenKind::LBrace, offset, offset + 1});
                offset += 1;
                break;
       
            case '}':
                tokens.push_back({TokenKind::RBrace, offset, offset + 1});
                offset += 1;
                break;

            case '~':
                if(offset + 1 < source.length() && source[offset + 1] == '>') {
                    if(!tokens.empty()) {
                        if(auto *kind = std::get_if<TokenKind>(&tokens.back().variant)) {
                            if(*kind == TokenKind::Newline) tokens.pop_back();
                        }
                    }

                    tokens.push_back({TokenKind::TildeRArrow, offset, offset + 2});
                    offset += 2;
                } else {
                    tokens.push_back({TokenKind::Tilde, offset, offset + 1});
                    offset += 1;
                }
                break;
            
            case '^':
                tokens.push_back({TokenKind::Circumflex, offset, offset + 1});
                offset += 1;
                break;
            
            case '!':
                if(offset + 1 < source.length() && source[offset+1] == '=') {
                    tokens.push_back({TokenKind::NotEqual, offset, offset + 2});
                    offset += 2;
                } else {
                    tokens.push_back({TokenKind::Not, offset, offset + 1});
                    offset += 1;
                }
                break;

            default:
                if((ispunct(ch) && ch != '_') || ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\v' ) {
                    std::cerr << "[Lexer] unexpected character `" << ch << "` found. ignoring.";
                    offset += 1;
                } else {
                    tokens.push_back(read_ident());
                    break;
                }
        }
    }

    tokens.push_back({TokenKind::EndOfFile, offset, offset});

    return tokens;
}