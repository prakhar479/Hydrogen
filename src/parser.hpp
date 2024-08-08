#pragma once
#include "./tokenizer.hpp"

namespace node
{
    struct NodeExpr
    {
        Token int_lit;
    };
    struct NodeExit
    {
        struct NodeExpr expression;
    };

};

class Parser
{
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)) {};

    std::optional<node::NodeExit> parse()
    {
        std::optional<node::NodeExit> node_exit;
        while (peek().has_value())
        {
            auto token = peek().value();
            if (token.type == TokenType::_exit)
            {
                consume();
                if (auto node_expr = parse_expr())
                {
                    node_exit = node::NodeExit{.expression = node_expr.value()};
                }
                else
                {
                    std::cerr << "Error: Invalid exit statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (!peek().has_value() || !(peek().value().type == TokenType::_EoE))
                {
                    std::cerr << "Error: Expected end of expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                consume();
            }
            else
            {
                std::cerr << "Invalid start of expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        m_pos = 0;
        return node_exit;
    }

private:
    std::optional<node::NodeExpr> parse_expr()
    {
        if (auto token = peek(); token.has_value() && token->type == TokenType::_int)
        {
            return node::NodeExpr{.int_lit = consume()};
        }
        return std::nullopt;
    }

    [[nodiscard]] inline std::optional<Token> peek(size_t ahead = 1) const
    {
        if (m_pos + ahead < m_tokens.size())
        {
            return m_tokens[m_pos + ahead];
        }
        return std::nullopt;
    }

    inline Token consume()
    {
        return m_tokens[++m_pos];
    }

    const std::vector<Token> m_tokens;
    size_t m_pos = -1;
};