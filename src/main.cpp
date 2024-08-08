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
std::string tok_to_asm(const vector<Token> &tokens)
{
    stringstream asm_code;
    vector<Token> expression = {};
    auto it = tokens.begin();
    while (it != tokens.end())
    {
        do
        {
            expression.push_back(*it);
            it++;
        } while (it->type != TokenType::_EoE && it != tokens.end());

        if (expression.front().type == TokenType::_exit)
        {
            asm_code << "\tmov rax, 60\n";
            if (expression.size() == 2 && expression[1].type == TokenType::_int)
            {
                asm_code << "\tmov rdi, " + expression[1].value.value() + "\n";
            }
            else
            {
                cout << "Error: Invalid exit statement" << endl;
                exit(EXIT_FAILURE);
            }
            asm_code << "\tsyscall\n";
        }
        else
        {
            cout << "Error: Invalid statement" << endl;
            exit(EXIT_FAILURE);
        }
        expression.clear();
        it++;
    }
    return asm_code.str();
}

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
    for (const Token &token : tokens)
    {
        cout << "Token: ";
        switch (token.type)
        {
        case TokenType::_exit:
            cout << "exit";
            break;
        case TokenType::_int:
            cout << "int";
            break;
        case TokenType::_EoE:
            cout << ";";
            break;
        }
        if (token.value.has_value())
        {
            cout << " " << token.value.value();
        }
        cout << endl;
    }

    cout << "Parsing Tokens ..." << endl;
    Parser parser(std::move(tokens));
    std::optional<node::NodeExit> tree = parser.parse();

    if (!tree.has_value())
    {
        cerr << "No exit statement found" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Generating assembly code..." << endl;

    Generator generator(tree.value());
    string asm_code = generator.generate();
    // string asm_code = tok_to_asm(tokens);
    cout << "Generated assembly code" << endl;
    cout << string(50, '*') << endl;

    {
        fstream file("out.asm", ios::out);
        cout << "Writing assembly code to file..." << endl;
        file << asm_code;
    }
    cout << "Assembly code written to file" << endl;
    cout << string(50, '*') << endl;

    cout << "Compiling assembly code..." << endl;
    system("nasm -f elf64 out.asm -o out.o");
    system("ld out.o -o a.out");
    cout << "Compiled assembly code" << endl;
    cout << string(50, '*') << endl;

    exit(EXIT_SUCCESS);
}
