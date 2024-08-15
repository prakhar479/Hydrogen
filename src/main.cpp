#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <utility>

#include "./tokenizer.hpp"
#include "./parser.hpp"
#include "./generation.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Error: Invalid number of arguments" << endl;
        cerr << "Usage: " << argv[0] << " <src file name>" << endl;
        exit(EXIT_FAILURE);
    }

    char *file_name = argv[1];

    cout << "Reading file: " << file_name << endl;
    string file_content;
    {
        stringstream buffer;
        fstream file(file_name, ios::in);
        buffer << file.rdbuf();
        file_content = buffer.str();
    }
    cout << "File Loaded" << endl;
    cout << string(50, '*') << endl;

    Tokenizer tokenizer(file_content);
    vector<Token> tokens = tokenizer.tokenize();
    tokenizer.printTokens(tokens);

    cout << "Parsing Tokens ..." << endl;
    Parser parser(std::move(tokens));
    std::unique_ptr<node::Program> tree = parser.parse();
    if (!tree)
    {
        cerr << "Error: Invalid syntax" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Parsing Tokens Done" << endl;  
    cout << string(50, '*') << endl;
    cout << "Parse Tree: " << endl;
    cout << parser.visualize(*tree) << endl;

    // if (!tree.has_value())
    // {
    //     cerr << "Error: Invalid syntax" << endl;
    //     exit(EXIT_FAILURE);
    // }
    // cout << "Generating assembly code..." << endl;

    // Generator generator(tree.value());
    // string asm_code = generator.generate();
    // // string asm_code = tok_to_asm(tokens);
    // cout << "Generated assembly code" << endl;
    // cout << string(50, '*') << endl;

    // {
    //     fstream file("out.asm", ios::out);
    //     cout << "Writing assembly code to file..." << endl;
    //     file << asm_code;
    // }
    // cout << "Assembly code written to file" << endl;
    // cout << string(50, '*') << endl;

    // cout << "Compiling assembly code..." << endl;
    // system("nasm -f elf64 out.asm -o out.o");
    // system("ld out.o -o a.out");
    // cout << "Compiled assembly code" << endl;
    // cout << string(50, '*') << endl;

    exit(EXIT_SUCCESS);
}
