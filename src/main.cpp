#include <haetae/lexer/lexer.hpp>
#include <haetae/parser/parser.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <format>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
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

    auto lexer_start = high_resolution_clock::now();
    Lexer lexer(std::move(input_file));
    vector<Token> tokens = lexer.get_tokens();
    auto lexer_end = high_resolution_clock::now();
    auto lexer_duration = duration_cast<microseconds>(lexer_end - lexer_start);

    cout << std::format("Lexer took {}μs\n", lexer_duration.count()) << "Tokens:\n";
    for(auto token : tokens) cout << token << " ";
    cout << "\n\n";

    auto parser_start = high_resolution_clock::now();
    Parser parser(std::move(tokens));
    auto opt_ast = parser.get_ast();
    auto parser_end = high_resolution_clock::now();
    auto parser_duration = duration_cast<microseconds>(parser_end - parser_start);
    if(!opt_ast) throw std::runtime_error(opt_ast.error().toString());

    cout << std::format("Parser took {}μs\n", parser_duration.count()) << "AST:\n";
    for(auto &expr : opt_ast->exprs) cout << expr << '\n';
    
    return 0;
}