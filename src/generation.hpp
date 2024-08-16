#pragma once

#include "./parser.hpp"
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stdexcept>

class Generator
{
public:
    /**
     * Constructor.
     * @param root The root node of the program.
     */
    inline explicit Generator(node::Program root)
        : m_root_node(std::move(root)), m_label_count(0), m_current_function("") {}

    /**
     * Generate the assembly code for the program.
     * @return The generated assembly code as a string.
     */
    [[nodiscard]] std::string generate() const
    {
        std::stringstream assembly;
        assembly << ".text\n";

        // Generate code for all function definitions
        for (const auto &stmt : m_root_node.statements)
        {
            if (auto *func_def = dynamic_cast<const node::FunctionDefinition *>(stmt.get()))
            {
                generate_function_definition(func_def, assembly);
            }
        }

        // Generate main function
        assembly << ".globl _start\n\n";
        assembly << "_start:\n";
        assembly << "    call main\n";
        assembly << "    mov %rax, %rdi\n";
        assembly << "    mov $60, %rax\n";
        assembly << "    syscall\n";

        return assembly.str();
    }

private:
    node::Program m_root_node;
    mutable int m_label_count;
    mutable std::unordered_map<std::string, std::unordered_map<std::string, int>> m_symbol_table;
    mutable std::string m_current_function;

    /**
     * Generate the assembly code for a function definition.
     * @param func_def The function definition node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_function_definition(const node::FunctionDefinition *func_def, std::stringstream &assembly) const
    {
        m_current_function = func_def->name;
        m_symbol_table[m_current_function] = {};

        assembly << func_def->name << ":\n";
        assembly << "    push %rbp\n"; // Save the previous base pointer
        assembly << "    mov %rsp, %rbp\n"; // Set the current stack pointer as the new base pointer

        // Allocate space for local variables
        int local_var_count = 0;
        for (const auto &stmt : func_def->body->statements)
        {
            if (dynamic_cast<const node::LetStatement *>(stmt.get()))
            {
                local_var_count++;
            }
        }
        if (local_var_count > 0)
        {
            assembly << "    sub $" << (local_var_count * 8) << ", %rsp\n"; // Adjust the stack pointer to allocate space for local variables
        }

        // Store parameters
        for (size_t i = 0; i < func_def->parameters.size(); i++)
        {
            m_symbol_table[m_current_function][func_def->parameters[i]] = -(static_cast<int>(i) + 1) * 8;
            assembly << "    mov " << (i < 6 ? register_for_param(i) : std::to_string((i - 5) * 8) + "(%rbp)") << ", " << (-(static_cast<int>(i) + 1) * 8) << "(%rbp)\n"; // Store the parameter values in the stack frame
        }

        // Generate code for function body
        for (const auto &stmt : func_def->body->statements)
        {
            generate_node(stmt.get(), assembly);
        }

        // Epilogue
        assembly << "    mov %rbp, %rsp\n"; // Restore the stack pointer
        assembly << "    pop %rbp\n"; // Restore the previous base pointer
        assembly << "    ret\n\n"; // Return from the function

        m_current_function = "";
    }

    /**
     * Generate the assembly code for a node in the AST.
     * @param node The node to generate code for.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_node(const node::Node *node, std::stringstream &assembly) const
    {
        if (auto *assignment = dynamic_cast<const node::Assignment *>(node))
        {
            generate_assignment(assignment, assembly);
        }
        else if (auto *let_stmt = dynamic_cast<const node::LetStatement *>(node))
        {
            generate_let_statement(let_stmt, assembly);
        }
        else if (auto *if_stmt = dynamic_cast<const node::IfStatement *>(node))
        {
            generate_if_statement(if_stmt, assembly);
        }
        else if (auto *while_stmt = dynamic_cast<const node::WhileStatement *>(node))
        {
            generate_while_statement(while_stmt, assembly);
        }
        else if (auto *return_stmt = dynamic_cast<const node::Return *>(node))
        {
            generate_return_statement(return_stmt, assembly);
        }
        else if (auto *block = dynamic_cast<const node::Block *>(node))
        {
            for (const auto &stmt : block->statements)
            {
                generate_node(stmt.get(), assembly);
            }
        }
    }

    /**
     * Generate the assembly code for an expression node.
     * @param expr The expression node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_expression(const node::Expression *expr, std::stringstream &assembly) const
    {
        if (auto *int_literal = dynamic_cast<const node::IntLiteral *>(expr))
        {
            assembly << "    mov $" << int_literal->value << ", %rax\n"; // Move the integer literal value into the accumulator register
        }
        else if (auto *identifier = dynamic_cast<const node::Identifier *>(expr))
        {
            int offset = m_symbol_table.at(m_current_function).at(identifier->name);
            assembly << "    mov " << offset << "(%rbp), %rax\n"; // Move the value of the identifier from the stack frame into the accumulator register
        }
        else if (auto *binary_op = dynamic_cast<const node::BinaryOp *>(expr))
        {
            generate_binary_op(binary_op, assembly);
        }
        else if (auto *func_call = dynamic_cast<const node::FunctionCall *>(expr))
        {
            generate_function_call(func_call, assembly);
        }
        else if (auto *block_expr = dynamic_cast<const node::BlockExpression *>(expr))
        {
            for (const auto &stmt : block_expr->block->statements)
            {
                generate_node(stmt.get(), assembly);
            }
        }
    }

    /**
     * Generate the assembly code for an assignment node.
     * @param assignment The assignment node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_assignment(const node::Assignment *assignment, std::stringstream &assembly) const
    {
        generate_expression(assignment->expression.get(), assembly);
        int offset = m_symbol_table.at(m_current_function).at(assignment->identifier);
        assembly << "    mov %rax, " << offset << "(%rbp)\n"; // Move the value in the accumulator register to the memory location of the identifier in the stack frame
    }

    /**
     * Generate the assembly code for a let statement node.
     * @param let_stmt The let statement node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_let_statement(const node::LetStatement *let_stmt, std::stringstream &assembly) const
    {
        generate_expression(let_stmt->value.get(), assembly);
        m_symbol_table[m_current_function][let_stmt->identifier] = -(static_cast<int>(m_symbol_table[m_current_function].size()) + 1) * 8;
        int offset = m_symbol_table.at(m_current_function).at(let_stmt->identifier);
        assembly << "    mov %rax, " << offset << "(%rbp)\n"; // Move the value in the accumulator register to the memory location of the identifier in the stack frame
    }

    /**
     * Generate the assembly code for an if statement node.
     * @param if_stmt The if statement node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_if_statement(const node::IfStatement *if_stmt, std::stringstream &assembly) const
    {
        int label = m_label_count++;
        generate_expression(if_stmt->condition.get(), assembly);
        assembly << "    cmp $0, %rax\n"; // Compare the value in the accumulator register with 0
        assembly << "    je .L_else_" << label << "\n"; // Jump to the else block if the condition is false

        for (const auto &stmt : if_stmt->thenBlock->statements)
        {
            generate_node(stmt.get(), assembly);
        }

        assembly << "    jmp .L_endif_" << label << "\n"; // Jump to the end of the if statement

        assembly << ".L_else_" << label << ":\n"; // Label for the else block

        if (if_stmt->elseBlock)
        {
            for (const auto &stmt : if_stmt->elseBlock->statements)
            {
                generate_node(stmt.get(), assembly);
            }
        }

        assembly << ".L_endif_" << label << ":\n"; // Label for the end of the if statement
    }

    /**
     * Generate the assembly code for a while statement node.
     * @param while_stmt The while statement node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_while_statement(const node::WhileStatement *while_stmt, std::stringstream &assembly) const
    {
        int label = m_label_count++;
        assembly << ".L_while_" << label << ":\n"; // Label for the start of the while loop
        generate_expression(while_stmt->condition.get(), assembly);
        assembly << "    cmp $0, %rax\n"; // Compare the value in the accumulator register with 0
        assembly << "    je .L_endwhile_" << label << "\n"; // Jump to the end of the while loop if the condition is false

        for (const auto &stmt : while_stmt->body->statements)
        {
            generate_node(stmt.get(), assembly);
        }

        assembly << "    jmp .L_while_" << label << "\n"; // Jump back to the start of the while loop

        assembly << ".L_endwhile_" << label << ":\n"; // Label for the end of the while loop
    }

    /**
     * Generate the assembly code for a return statement node.
     * @param return_stmt The return statement node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_return_statement(const node::Return *return_stmt, std::stringstream &assembly) const
    {
        generate_expression(return_stmt->expression.get(), assembly);
        assembly << "    mov %rbp, %rsp\n"; // Restore the stack pointer
        assembly << "    pop %rbp\n"; // Restore the previous base pointer
        assembly << "    ret\n"; // Return from the function
    }

    /**
     * Generate the assembly code for a binary operation node.
     * @param binary_op The binary operation node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_binary_op(const node::BinaryOp *binary_op, std::stringstream &assembly) const
    {
        generate_expression(binary_op->right.get(), assembly);
        assembly << "    push %rax\n"; // Push the value in the accumulator register onto the stack
        generate_expression(binary_op->left.get(), assembly);
        assembly << "    pop %rbx\n"; // Pop the value from the stack into a temporary register

        switch (binary_op->op)
        {
        case TokenType::_plus:
            assembly << "    add %rbx, %rax\n"; // Add the value in the temporary register to the value in the accumulator register
            break;
        case TokenType::_minus:
            assembly << "    sub %rbx, %rax\n"; // Subtract the value in the temporary register from the value in the accumulator register
            break;
        case TokenType::_multiply:
            assembly << "    imul %rbx\n"; // Multiply the value in the accumulator register by the value in the temporary register
            break;
        case TokenType::_percent:
            assembly << "    xor %rdx, %rdx\n"; // Clear the rdx register
            assembly << "    idiv %rbx\n"; // Divide the value in the accumulator register by the value in the temporary register, storing the quotient in the rax register and the remainder in the rdx register
            assembly << "    mov %rdx, %rax\n"; // Move the remainder from the rdx register to the rax register
            break;
        case TokenType::_equal:
        case TokenType::_less_than:
        case TokenType::_greater_than:
            generate_comparison(binary_op->op, assembly);
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
        }
    }

    /**
     * Generate the assembly code for a comparison operation.
     * @param op The comparison operator.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_comparison(TokenType op, std::stringstream &assembly) const
    {
        assembly << "    cmp %rbx, %rax\n"; // Compare the value in the temporary register with the value in the accumulator register
        switch (op)
        {
        case TokenType::_equal:
            assembly << "    sete %al\n"; // Set the low byte of the rax register to 1 if the values are equal, or 0 otherwise
            break;
        case TokenType::_less_than:
            assembly << "    setl %al\n"; // Set the low byte of the rax register to 1 if the value in the accumulator register is less than the value in the temporary register, or 0 otherwise
            break;
        case TokenType::_greater_than:
            assembly << "    setg %al\n"; // Set the low byte of the rax register to 1 if the value in the accumulator register is greater than the value in the temporary register, or 0 otherwise
            break;
        default:
            throw std::runtime_error("Invalid comparison operator");
        }
        assembly << "    movzb %al, %rax\n"; // Zero-extend the low byte of the rax register to the full rax register
    }

    /**
     * Generate the assembly code for a function call node.
     * @param func_call The function call node.
     * @param assembly The stringstream to store the generated assembly code.
     */
    void generate_function_call(const node::FunctionCall *func_call, std::stringstream &assembly) const
    {
        // Push caller-saved registers
        assembly << "    push %rdi\n";
        assembly << "    push %rsi\n";
        assembly << "    push %rdx\n";
        assembly << "    push %rcx\n";
        assembly << "    push %r8\n";
        assembly << "    push %r9\n";

        // Evaluate and push arguments in reverse order
        for (auto it = func_call->arguments.rbegin(); it != func_call->arguments.rend(); ++it)
        {
            generate_expression(it->get(), assembly);
            assembly << "    push %rax\n"; // Push the value in the accumulator register onto the stack
        }

        // Pop arguments into the appropriate registers
        for (size_t i = 0; i < func_call->arguments.size(); i++)
        {
            if (i < 6)
            {
                assembly << "    pop " << register_for_param(i) << "\n"; // Pop the value from the stack into the appropriate register
            }
        }

        // Call the function
        assembly << "    call " << func_call->name << "\n";

        // Clean up the stack if there were more than 6 arguments
        if (func_call->arguments.size() > 6)
        {
            assembly << "    add $" << (func_call->arguments.size() - 6) * 8 << ", %rsp\n"; // Adjust the stack pointer to remove the arguments from the stack
        }

        // Pop caller-saved registers
        assembly << "    pop %r9\n";
        assembly << "    pop %r8\n";
        assembly << "    pop %rcx\n";
        assembly << "    pop %rdx\n";
        assembly << "    pop %rsi\n";
        assembly << "    pop %rdi\n";
    }

    /**
     * Get the register for a function parameter.
     * @param index The index of the parameter.
     * @return The register name.
     */
    static std::string register_for_param(size_t index)
    {
        static const std::vector<std::string> param_registers = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        if (index < param_registers.size())
        {
            return param_registers[index];
        }
        throw std::runtime_error("Too many function parameters");
    }
};