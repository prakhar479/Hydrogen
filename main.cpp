#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

using namespace std;

enum class TokenType
{
    _return,
    _int,
    _EoE,
};

struct Token
{
    TokenType type;
    optional<string> value;
};

bool isNumeric(const string &str)
{
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

vector<Token> tokenize(const string &input)
{
    cout << "Tokenizing input..." << endl;
    vector<Token> tokens;
    string current_token;
    for (char c : input)
    {
        if (isspace(c) || c == ';')
        {
            if (!current_token.empty())
            {
                // cout << "Current token: " << current_token << endl;
                Token token;
                if (current_token == "return")
                {
                    token.type = TokenType::_return;
                }
                else if (isNumeric(current_token))
                {
                    token.type = TokenType::_int;
                    token.value = current_token;
                }
                else if (current_token == ";")
                {
                    token.type = TokenType::_EoE;
                }
                else
                {
                    token.type = TokenType::_return;
                }
                tokens.push_back(token);
                current_token.clear();
            }
            if (c == ';')
            {
                Token token;
                token.type = TokenType::_EoE;
                tokens.push_back(token);
            }
        }
        else
        {
            current_token += c;
        }
    }
    cout << "Completed tokenizing input" << endl;
    cout << string(50, '*') << endl;
    return tokens;
}

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

        if (expression.front().type == TokenType::_return)
        {
            asm_code << "\tmov rax, 60\n";
            if (expression.size() == 2 && expression[1].type == TokenType::_int)
            {
                asm_code << "\tmov rdi, " + expression[1].value.value() + "\n";
            }
            else
            {
                cout << "Error: Invalid return statement" << endl;
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

    vector<Token> tokens = tokenize(file_content);
    for (const Token &token : tokens)
    {
        cout << "Token: ";
        switch (token.type)
        {
        case TokenType::_return:
            cout << "return";
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

    cout << "Generating assembly code..." << endl;
    string asm_code = tok_to_asm(tokens);
    cout << "Generated assembly code" << endl;
    cout << string(50, '*') << endl;

    {
        fstream file("out.asm", ios::out);
        cout << "Setting up file for writing..." << endl;
        // file << "section .text\n";
        file << "global _start\n";
        file << "_start:\n";
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
