#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <haetae/lexer/lexer.hpp>
#include <haetae/parser/parser.hpp>

using namespace std;

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

    Lexer lexer(std::move(input_file));
    vector<Token> tokens = lexer.get_tokens();

    cout << "Tokens:\n";
    for(auto token : tokens) cout << token << " ";
    cout << "\n\n";

    Parser parser(std::move(tokens));
    auto opt_ast = parser.get_ast();
    if(!opt_ast) throw std::runtime_error(opt_ast.error().toString());

    cout << "AST:\n";
    for(auto &expr : opt_ast->exprs) cout << expr << '\n';
    
    return 0;
}