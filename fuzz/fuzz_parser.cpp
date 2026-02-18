#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

#include "../src/lexer/lexer.hpp"
#include "../src/parser/parser.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    std::string input(reinterpret_cast<const char*>(data), size);

    try {
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.get_tokens();

        Parser parser(std::move(tokens));
        auto opt_ast = parser.get_ast();
        if(!opt_ast) return 0;

        AST ast = std::move(opt_ast).value();
    } catch (...) {

    }

    return 0;
}