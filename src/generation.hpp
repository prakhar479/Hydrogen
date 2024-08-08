#pragma once

#include "./parser.hpp"

class Generator
{
public:
    inline explicit Generator(node::NodeExit root)
        : m_root_node(std::move(root)) {};

    [[nodiscard]] std::string generate() const 
    {
        std::stringstream asm_code;
        // set up the global _start label
        asm_code << "global _start\n_start:\n";
        asm_code << "\tmov rax, 60\n";
        asm_code << "\tmov rdi, " << m_root_node.expression.int_lit.value.value() << "\n";
        asm_code << "\tsyscall\n";
        return asm_code.str();
    }
private:
    node::NodeExit m_root_node;
};