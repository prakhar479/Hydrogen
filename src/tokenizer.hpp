#pragma once

enum class TokenType
{
    _exit,
    _int,
    _EoE
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer
{
public:
    inline Tokenizer(std::string src) : m_src(std::move(src)) {};
    inline std::vector<Token> tokenize()
    {
        std::string buffer;
        std::vector<Token> tokens;
        while (this->peek().has_value())
        {
            char c = this->consume();
            if (isspace(c) || c == ';')
            {
                if (!buffer.empty())
                {
                    Token token;
                    if (buffer == "exit")
                    {
                        token.type = TokenType::_exit;
                    }
                    else if (isNumeric(buffer))
                    {
                        token.type = TokenType::_int;
                        token.value = buffer;
                    }
                    else if (buffer == ";")
                    {
                        token.type = TokenType::_EoE;
                    }
                    else
                    {
                        std::cerr << "Error: Invalid token: " << buffer << " detected" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    tokens.push_back(token);
                    buffer.clear();
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
                buffer += c;
            }
        }

        if (!buffer.empty())
        {
            std::cerr << "Error: Invalid tokens in input" << std::endl;
            exit(EXIT_FAILURE);
        }

        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(size_t ahead = 1) const
    {
        if (m_pos + ahead < m_src.size())
        {
            return m_src[m_pos + ahead];
        }
        return std::nullopt;
    }

    [[nodiscard]] inline bool isNumeric(const std::string &str) const
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

    inline char consume()
    {
        return m_src[++m_pos];
    }

    const std::string m_src;
    size_t m_pos = -1;
};