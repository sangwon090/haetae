#include <iostream>
#include <filesystem>
#include <fstream>

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

    return 0;
}