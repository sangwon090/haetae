#include <haetae/lexer/lexer.hpp>
#include <haetae/parser/parser.hpp>
#include <haetae/sema/sema.hpp>
#include <haetae/irgen/irgen.hpp>
#include <haetae/utils/timer.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <format>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {

    // handle input args
    if(argc == 1) {
        cout << "error: no input files\n";
        return -1;
    }

    if(!filesystem::exists(argv[1])) {
        cout << "error: input file not exists\n";
        return -1;
    }

    ifstream input_file(argv[1]);

    if(!input_file.is_open()) {
        cout << "error: cannot open input file\n";
        return -1;
    }


    // lexical analysis
    auto [tokens, lexer_duration] = time_execution([&]() {
        Lexer lexer(std::move(input_file));
        return lexer.get_tokens();
    });

    cout << std::format("Lexer took {}μs\n", lexer_duration.count()) << "Tokens:\n";
    for(auto token : tokens) cout << token << " ";
    cout << "\n\n";


    // parse
    auto [ast, parser_duration] = time_execution([&]() {
        Parser parser(std::move(tokens));
        return parser.get_ast();
    });

    if(!ast) throw std::runtime_error(ast.error().toString());

    cout << std::format("Parser took {}μs\n", parser_duration.count()) << "AST:\n";
    for(auto &expr : ast->exprs) cout << expr << '\n';
    cout << "\n";


    // semantic analysis
    auto [sema_ast, sema_duration] = time_execution([&]() {
        Sema sema(std::move(*ast));
        return sema.analyze_all();
    });

    if(!sema_ast) throw std::runtime_error(sema_ast.error().toString());

    cout << std::format("Sema took {}μs\n", sema_duration.count()) << "Analyzed AST:\n";
    for(auto &expr : sema_ast->exprs) cout << expr << '\n';
    cout << "\n";


    // IR generation
    auto [ir, irgen_duration] = time_execution([&]() {
        IRGen irgen(std::move(*sema_ast));
        return irgen.generate_ir();
    });

    cout << std::format("IRGen took {}μs\n", irgen_duration.count()) << "MLIR:\n";
    cout << ir << '\n';

    return 0;
}