#pragma once

#include "./tokenizer.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

namespace node
{
    struct Node
    {
        virtual ~Node() = default;
        virtual std::string visualize(size_t indent = 0) const = 0;
    };

    struct Expression : public Node
    {
    };

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

    struct IntLiteral : public Expression
    {
        int value;
        explicit IntLiteral(int val) : value(val) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "IntLiteral: " + std::to_string(value);
        }
    };

    struct Identifier : public Expression
    {
        std::string name;
        explicit Identifier(std::string n) : name(std::move(n)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "Identifier: " + name;
        }
    };

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

    struct ExitStatement : public Node
    {
        std::unique_ptr<Expression> expression;
        explicit ExitStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
        std::string visualize(size_t indent = 0) const override
        {
            return std::string(indent, ' ') + "ExitStatement:\n" + expression->visualize(indent + 2);
        }
    };

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
    explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_pos(0) {}

    std::unique_ptr<node::Program> parse()
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
                throw std::runtime_error("Unexpected token at position " + std::to_string(m_pos));
            }
        }
        return program;
    }

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
        auto body = parseBlock();
        return std::make_unique<node::FunctionDefinition>(name, std::move(parameters), std::move(body));
    }

    std::unique_ptr<node::Block> parseBlock()
    {
        consume(TokenType::_open_brace);
        auto block = std::make_unique<node::Block>();
        while (m_tokens[m_pos].type != TokenType::_close_brace)
        {
            if (auto stmt = parseStatement())
            {
                block->statements.push_back(std::move(stmt));
            }
            else
            {
                throw std::runtime_error("Expected statement in block");
            }
        }
        consume(TokenType::_close_brace);
        return block;
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
            return std::make_unique<node::Identifier>(m_tokens[m_pos++].value.value());
        }
        else if (m_tokens[m_pos].type == TokenType::_open_paren)
        {
            consume(TokenType::_open_paren);
            auto expr = parseExpression();
            consume(TokenType::_close_paren);
            return expr;
        }
        throw std::runtime_error("Unexpected token in expression");
    }

    std::unique_ptr<node::Assignment> parseAssignment()
    {
        std::string identifier = consume(TokenType::_identifier).value.value();
        consume(TokenType::_assign);
        auto expr = parseExpression();
        consume(TokenType::_EOS);
        return std::make_unique<node::Assignment>(identifier, std::move(expr));
    }

    Token consume(TokenType expected)
    {
        if (m_pos >= m_tokens.size())
        {
            throw std::runtime_error("Unexpected end of input");
        }
        if (m_tokens[m_pos].type != expected)
        {
            throw std::runtime_error("Unexpected token: expected " + tokenTypeToString(expected) +
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

    std::vector<Token> m_tokens;
    size_t m_pos;
};