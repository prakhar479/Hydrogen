#pragma once

#include "./tokenizer.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <sstream>

/**
 * @brief Node namespace containing all the AST nodes
 */
namespace node
{
    // Base node structure for the Abstract Syntax Tree (AST)
    struct Node
    {
        virtual ~Node() = default;
        virtual std::string visualize(size_t indent = 0) const = 0;
    };

    // Expression node, base class for all expressions
    struct Expression : public Node
    {
    };

    // Assignment node, represents variable assignment
    struct Assignment : public Node
    {
        std::string identifier;
        std::unique_ptr<Expression> expression;
        Assignment(std::string id, std::unique_ptr<Expression> expr)
            : identifier(std::move(id)), expression(std::move(expr)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "Assignment: " + identifier + "\n" + expression->visualize(indent + 2);
        }
    };

    // IntLiteral node, represents integer literals
    struct IntLiteral : public Expression
    {
        int value;
        explicit IntLiteral(int val) : value(val) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "IntLiteral: " + std::to_string(value);
        }
    };

    // Identifier node, represents variable identifiers
    struct Identifier : public Expression
    {
        std::string name;
        explicit Identifier(std::string n) : name(std::move(n)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "Identifier: " + name;
        }
    };

    // BinaryOp node, represents binary operations
    struct BinaryOp : public Expression
    {
        std::unique_ptr<Expression> left;
        TokenType op;
        std::unique_ptr<Expression> right;
        BinaryOp(std::unique_ptr<Expression> l, TokenType o, std::unique_ptr<Expression> r)
            : left(std::move(l)), op(o), right(std::move(r)) {}
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "BinaryOp: " + tokenTypeToString(op) + "\n";
            result += left->visualize(indent + 2) + "\n";
            result += right->visualize(indent + 2);
            return result;
        }
    };

    // ExitStatement node, represents exit statements
    struct ExitStatement : public Node
    {
        std::unique_ptr<Expression> expression;
        explicit ExitStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "ExitStatement:\n" + expression->visualize(indent + 2);
        }
    };

    // LetStatement node, represents variable declarations
    struct LetStatement : public Node
    {
        std::string identifier;
        std::unique_ptr<Expression> value;
        LetStatement(std::string id, std::unique_ptr<Expression> val)
            : identifier(std::move(id)), value(std::move(val)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "LetStatement: " + identifier + "\n" +
                   value->visualize(indent + 2);
        }
    };

    // Block node, represents a block of statements
    struct Block : public Node
    {
        std::vector<std::unique_ptr<Node>> statements;
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "Block:\n";
            for (const auto &stmt : statements)
            {
                result += stmt->visualize(indent + 2) + "\n";
            }
            return result;
        }
    };

    // IfStatement node, represents if-else statements
    struct IfStatement : public Node
    {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Block> thenBlock;
        std::unique_ptr<Block> elseBlock;
        IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Block> thenB, std::unique_ptr<Block> elseB = nullptr)
            : condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "IfStatement:\n";
            result += std::string(indent + 2, ' ') + "Condition:\n" + condition->visualize(indent + 4) + "\n";
            result += std::string(indent + 2, ' ') + "Then:\n" + thenBlock->visualize(indent + 4);
            if (elseBlock)
            {
                result += "\n" + std::string(indent + 2, ' ') + "Else:\n" + elseBlock->visualize(indent + 4);
            }
            return result;
        }
    };

    // WhileStatement node, represents while loops
    struct WhileStatement : public Node
    {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Block> body;
        WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Block> b)
            : condition(std::move(cond)), body(std::move(b)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "WhileStatement:\n" +
                   std::string(indent + 2, ' ') + "Condition:\n" + condition->visualize(indent + 4) + "\n" +
                   std::string(indent + 2, ' ') + "Body:\n" + body->visualize(indent + 4);
        }
    };

    // ForStatement node, represents for loops
    struct ForStatement : public Node
    {
        std::unique_ptr<LetStatement> initialization;
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Assignment> increment;
        std::unique_ptr<Block> body;
        ForStatement(std::unique_ptr<LetStatement> init, std::unique_ptr<Expression> cond,
                     std::unique_ptr<Assignment> inc, std::unique_ptr<Block> b)
            : initialization(std::move(init)), condition(std::move(cond)),
              increment(std::move(inc)), body(std::move(b)) {}
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "ForStatement:\n";
            result += std::string(indent + 2, ' ') + "Initialization:\n" + initialization->visualize(indent + 4) + "\n";
            result += std::string(indent + 2, ' ') + "Condition:\n" + condition->visualize(indent + 4) + "\n";
            result += std::string(indent + 2, ' ') + "Increment:\n" + increment->visualize(indent + 4) + "\n";
            result += std::string(indent + 2, ' ') + "Body:\n" + body->visualize(indent + 4);
            return result;
        }
    };

    // FunctionDefinition node, represents function definitions
    struct FunctionDefinition : public Node
    {
        std::string name;
        std::vector<std::string> parameters;
        std::unique_ptr<Block> body;
        FunctionDefinition(std::string n, std::vector<std::string> params, std::unique_ptr<Block> b)
            : name(std::move(n)), parameters(std::move(params)), body(std::move(b)) {}
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "FunctionDefinition: " + name + "\n";
            result += std::string(indent + 2, ' ') + "Parameters: ";
            for (const auto &param : parameters)
            {
                result += param + " ";
            }
            result += "\n" + std::string(indent + 2, ' ') + "Body:\n" + body->visualize(indent + 4);
            return result;
        }
    };

    // Return node, represents return statements
    struct Return : public Node
    {
        std::unique_ptr<Expression> expression;
        explicit Return(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "Return:\n" + expression->visualize(indent + 2);
        }
    };

    // FunctionCall node, represents function calls
    struct FunctionCall : public Expression
    {
        std::string name;
        std::vector<std::unique_ptr<Expression>> arguments;
        FunctionCall(std::string n, std::vector<std::unique_ptr<Expression>> args)
            : name(std::move(n)), arguments(std::move(args)) {}
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = std::string(indent, ' ') + "FunctionCall: " + name + "\n";
            for (const auto &arg : arguments)
            {
                result += arg->visualize(indent + 2) + "\n";
            }
            return result;
        }
    };

    // BlockExpression node, represents blocks used in expressions
    struct BlockExpression : public Expression
    {
        std::unique_ptr<Block> block;
        explicit BlockExpression(std::unique_ptr<Block> b) : block(std::move(b)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "BlockExpression:\n" + block->visualize(indent + 2);
        }
    };

    // Program node, represents the root of the AST
    struct Program : public Node
    {
        std::vector<std::unique_ptr<Node>> statements;
        std::string visualize(size_t indent = 0) const override
        {
            std::string result = "Program:\n";
            for (const auto &stmt : statements)
            {
                result += stmt->visualize(indent + 2) + "\n";
            }
            return result;
        }
    };
}

class Parser
{
public:
    /**
     * @brief Construct a new Parser object
     *
     * @param tokens Vector of tokens to be parsed
     */
    explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_pos(0) {}

    /**
     * @brief Parse the input tokens and generate an AST
     *
     * @return std::unique_ptr<node::Program> Root node of the AST
     */
    std::unique_ptr<node::Program>
    parse()
    {
        auto program = std::make_unique<node::Program>();
        while (m_pos < m_tokens.size())
        {
            if (auto stmt = parseStatement())
            {
                program->statements.push_back(std::move(stmt));
            }
            else
            {
                throw parseError("Unexpected token at position " + std::to_string(m_pos));
            }
        }
        return program;
    }

    /**
     * @brief Visualize the AST
     *
     * @param tree Root node of the AST
     * @return std::string String representation of the AST
     */
    std::string visualize(node::Program &tree) const
    {
        return tree.visualize();
    }

private:
    std::unique_ptr<node::Node> parseStatement()
    {
        switch (m_tokens[m_pos].type)
        {
        case TokenType::_exit:
            return parseExitStatement();
        case TokenType::_let:
            return parseLetStatement();
        case TokenType::_if:
            return parseIfStatement();
        case TokenType::_while:
            return parseWhileStatement();
        case TokenType::_for:
            return parseForStatement();
        case TokenType::_define:
            return parseFunctionDefinition();
        case TokenType::_open_brace:
            return parseBlock();
        case TokenType::_identifier:
            if (m_pos + 1 < m_tokens.size() && m_tokens[m_pos + 1].type == TokenType::_open_paren)
            {
                return parseFunctionCall(consume(TokenType::_identifier).value.value());
            }
            return parseAssignment();
        default:
            return nullptr;
        }
    }

    std::unique_ptr<node::ExitStatement> parseExitStatement()
    {
        consume(TokenType::_exit);
        auto expr = parseExpression();
        consume(TokenType::_EOS);
        return std::make_unique<node::ExitStatement>(std::move(expr));
    }

    std::unique_ptr<node::LetStatement> parseLetStatement()
    {
        consume(TokenType::_let);
        std::string identifier = consume(TokenType::_identifier).value.value();
        consume(TokenType::_assign);
        auto value = parseExpression();
        consume(TokenType::_EOS);

        m_variables.insert(identifier);

        return std::make_unique<node::LetStatement>(identifier, std::move(value));
    }

    std::unique_ptr<node::IfStatement> parseIfStatement()
    {
        consume(TokenType::_if);
        consume(TokenType::_open_paren);
        auto condition = parseExpression();
        consume(TokenType::_close_paren);
        auto thenBlock = parseBlock();
        std::unique_ptr<node::Block> elseBlock = nullptr;
        if (m_pos < m_tokens.size() && m_tokens[m_pos].type == TokenType::_else)
        {
            consume(TokenType::_else);
            elseBlock = parseBlock();
        }
        return std::make_unique<node::IfStatement>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
    }

    std::unique_ptr<node::WhileStatement> parseWhileStatement()
    {
        consume(TokenType::_while);
        consume(TokenType::_open_paren);
        auto condition = parseExpression();
        consume(TokenType::_close_paren);
        auto body = parseBlock();
        return std::make_unique<node::WhileStatement>(std::move(condition), std::move(body));
    }

    std::unique_ptr<node::ForStatement> parseForStatement()
    {
        consume(TokenType::_for);
        consume(TokenType::_open_paren);
        auto init = parseLetStatement();
        auto condition = parseExpression();
        consume(TokenType::_EOS);
        auto increment = parseAssignment();
        consume(TokenType::_close_paren);
        auto body = parseBlock();
        return std::make_unique<node::ForStatement>(std::move(init), std::move(condition), std::move(increment), std::move(body));
    }

    std::unique_ptr<node::FunctionDefinition> parseFunctionDefinition()
    {
        consume(TokenType::_define);
        std::string name = consume(TokenType::_identifier).value.value();
        m_functions.insert(name);

        consume(TokenType::_open_paren);
        std::vector<std::string> parameters;
        while (m_tokens[m_pos].type != TokenType::_close_paren)
        {
            parameters.push_back(consume(TokenType::_identifier).value.value());
            if (m_tokens[m_pos].type != TokenType::_close_paren)
            {
                consume(TokenType::_EOS);
            }
        }
        consume(TokenType::_close_paren);

        m_variables.insert(parameters.begin(), parameters.end());
        auto body = parseBlock(true);
        for (const auto &param : parameters)
        {
            m_variables.erase(param);
        }

        return std::make_unique<node::FunctionDefinition>(name, std::move(parameters), std::move(body));
    }

    std::unique_ptr<node::Block> parseBlock(bool returned = false)
    {
        bool hasReturn = false;

        consume(TokenType::_open_brace);
        auto block = std::make_unique<node::Block>();
        while (m_tokens[m_pos].type != TokenType::_close_brace)
        {
            if (m_tokens[m_pos].type == TokenType::_ret)
            {
                if (returned)
                {
                    block->statements.push_back(parseReturn());
                    hasReturn = true;
                }
                else
                {
                    throw parseError("Return statement not allowed in non-returning block");
                }
            }
            else if (auto stmt = parseStatement())
            {
                block->statements.push_back(std::move(stmt));
            }
            else
            {
                throw parseError("Expected either return or statement in block");
            }
        }
        if (!hasReturn && returned)
        {
            throw parseError("Block used in assignment must have a return statement");
        }
        consume(TokenType::_close_brace);
        return block;
    }

    std::unique_ptr<node::Return> parseReturn()
    {
        consume(TokenType::_ret);
        auto expr = parseExpression();
        consume(TokenType::_EOS);
        return std::make_unique<node::Return>(std::move(expr));
    }

    std::unique_ptr<node::Expression> parseExpression()
    {
        return parseBinaryExpression();
    }

    std::unique_ptr<node::Expression> parseBinaryExpression(int minPrecedence = 0)
    {
        auto left = parsePrimaryExpression();

        while (true)
        {
            int precedence = getOperatorPrecedence(m_tokens[m_pos].type);
            if (precedence < minPrecedence)
                break;

            TokenType op = m_tokens[m_pos].type;
            consume(op);

            auto right = parseBinaryExpression(precedence + 1);
            left = std::make_unique<node::BinaryOp>(std::move(left), op, std::move(right));
        }

        return left;
    }

    std::unique_ptr<node::Expression> parsePrimaryExpression()
    {
        if (m_tokens[m_pos].type == TokenType::_int)
        {
            return std::make_unique<node::IntLiteral>(std::stoi(m_tokens[m_pos++].value.value()));
        }
        else if (m_tokens[m_pos].type == TokenType::_identifier)
        {
            std::string identifier = m_tokens[m_pos++].value.value();
            if (m_pos < m_tokens.size() && m_tokens[m_pos].type == TokenType::_open_paren)
            {
                return parseFunctionCall(identifier);
            }
            if (m_variables.find(identifier) == m_variables.end())
            {
                throw parseError("Variable '" + identifier + "' used before declaration");
            }
            return std::make_unique<node::Identifier>(identifier);
        }
        else if (m_tokens[m_pos].type == TokenType::_open_paren)
        {
            consume(TokenType::_open_paren);
            auto expr = parseExpression();
            consume(TokenType::_close_paren);
            return expr;
        }
        else if (m_tokens[m_pos].type == TokenType::_open_brace)
        {
            return std::make_unique<node::BlockExpression>(parseBlock(true));
        }
        throw parseError("Unexpected token in expression");
    }

    std::unique_ptr<node::Assignment> parseAssignment()
    {
        std::string identifier = consume(TokenType::_identifier).value.value();
        consume(TokenType::_assign);

        auto expr = parseExpression();
        consume(TokenType::_EOS);
        if (m_variables.find(identifier) == m_variables.end())
        {
            throw parseError("Variable '" + identifier + "' used before declaration");
        }
        return std::make_unique<node::Assignment>(identifier, std::move(expr));
    }

    std::unique_ptr<node::FunctionCall> parseFunctionCall(const std::string &name)
    {
        if (m_functions.find(name) == m_functions.end())
        {
            throw parseError("Function '" + name + "' called before definition");
        }
        consume(TokenType::_open_paren);
        std::vector<std::unique_ptr<node::Expression>> arguments;
        while (m_tokens[m_pos].type != TokenType::_close_paren)
        {
            arguments.push_back(parseExpression());
            if (m_tokens[m_pos].type != TokenType::_close_paren)
            {
                consume(TokenType::_EOS);
            }
        }
        consume(TokenType::_close_paren);
        return std::make_unique<node::FunctionCall>(name, std::move(arguments));
    }

    Token consume(TokenType expected)
    {
        if (m_pos >= m_tokens.size())
        {
            throw parseError("Unexpected end of input");
        }
        if (m_tokens[m_pos].type != expected)
        {
            throw parseError("Unexpected token: expected " + tokenTypeToString(expected) +
                             ", got " + tokenTypeToString(m_tokens[m_pos].type));
        }
        return m_tokens[m_pos++];
    }

    int getOperatorPrecedence(TokenType op)
    {
        switch (op)
        {
        case TokenType::_multiply:
        case TokenType::_percent:
            return 5;
        case TokenType::_plus:
        case TokenType::_minus:
            return 4;
        case TokenType::_less_than:
        case TokenType::_greater_than:
        case TokenType::_equal:
            return 3;
        default:
            return -1;
        }
    }

    std::runtime_error parseError(const std::string &message)
    {
        std::ostringstream errorMsg;
        errorMsg << "Parse error at position " << m_pos << ": " << message << "\n";
        if (m_pos < m_tokens.size())
        {
            errorMsg << "Current token: " << tokenTypeToString(m_tokens[m_pos].type);
            if (m_tokens[m_pos].value)
            {
                errorMsg << " ('" << m_tokens[m_pos].value.value() << "')";
            }
        }
        else
        {
            errorMsg << "End of input reached unexpectedly";
        }
        return std::runtime_error(errorMsg.str());
    }

    std::vector<Token> m_tokens;                    // Vector of tokens to be parsed
    size_t m_pos;                                   // Current position in the token vector
    std::unordered_set<std::string> m_variables;    // Set of variable names
    std::unordered_set<std::string> m_functions;    // Set of function names
};