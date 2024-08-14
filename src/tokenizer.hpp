#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cctype>
#include <iostream>

/**
 * @enum TokenType
 * @brief Enum representing different types of tokens in the language.
 */
enum class TokenType
{
    _exit,         ///< Represents the "exit" keyword.
    _int,          ///< Represents an integer literal.
    _EOS,          ///< Represents the end of a statement.
    _open_paren,   ///< Represents an opening parenthesis '('.
    _close_paren,  ///< Represents a closing parenthesis ')'.
    _open_brace,   ///< Represents an opening curly brace '{'.
    _close_brace,  ///< Represents a closing curly brace '}'.
    _multiply,     ///< Represents the multiplication operator '*'.
    _percent,      ///< Represents the modulo operator '%'.
    _plus,         ///< Represents the addition operator '+'.
    _minus,        ///< Represents the subtraction operator '-'.
    _equal,        ///< Represents the equality operator '=='.
    _less_than,    ///< Represents the less than operator '<'.
    _greater_than, ///< Represents the greater than operator '>'.
    _assign,       ///< Represents the assignment operator '='.
    _identifier,   ///< Represents an identifier (variable or function name).
    _for,          ///< Represents the "for" keyword for loop.
    _if,           ///< Represents the "if" keyword for conditional statement.  
    _else,         ///< Represents the "else" keyword for conditional statement.
    _let,          ///< Represents the "let" keyword for variable declaration.
    _define,       ///< Represents the "define" keyword for function definition.
    _while,        ///< Represents the "while" keyword for while loop.
    _ERROR         ///< Represents an invalid or unrecognized token.
};

/**
 * @brief Converts a TokenType to its corresponding string representation.
 * @param type The TokenType to convert.
 * @return A string representing the TokenType.
 */
static std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::_exit:
        return "exit";
    case TokenType::_int:
        return "int";
    case TokenType::_EOS:
        return "EOS";
    case TokenType::_open_paren:
        return "(";
    case TokenType::_close_paren:
        return ")";
    case TokenType::_open_brace:
        return "{";
    case TokenType::_close_brace:
        return "}";
    case TokenType::_multiply:
        return "*";
    case TokenType::_percent:
        return "%";
    case TokenType::_plus:
        return "+";
    case TokenType::_minus:
        return "-";
    case TokenType::_equal:
        return "==";
    case TokenType::_less_than:
        return "<";
    case TokenType::_greater_than:
        return ">";
    case TokenType::_assign:
        return "=";
    case TokenType::_identifier:
        return "identifier";
    case TokenType::_ERROR:
        return "ERROR";
    case TokenType::_for:
        return "for";
    case TokenType::_if:
        return "if";
    case TokenType::_else:
        return "else";
    case TokenType::_let:
        return "let";
    case TokenType::_define:
        return "define";
    case TokenType::_while:
        return "while";
    default:
        return "UNKNOWN";
    }
}

/**
 * @struct Token
 * @brief Structure representing a token with its type and optional value.
 */
struct Token
{
    TokenType type;                   ///< The type of the token.
    std::optional<std::string> value; ///< The value of the token, if applicable.
};

/**
 * @class Tokenizer
 * @brief A simple tokenizer for a custom programming language.
 */
class Tokenizer
{
public:
    /**
     * @brief Constructor that initializes the tokenizer with a source string.
     * @param src The source string to tokenize.
     */
    Tokenizer(std::string src) : m_src(std::move(src)), m_pos(0) {}

    /**
     * @brief Tokenizes the source string into a vector of tokens.
     * @return A vector of Token objects representing the tokenized input.
     */
    std::vector<Token> tokenize()
    {
        std::vector<Token> tokens; ///< Vector to store the tokens
        std::string buffer;        ///< Buffer to accumulate characters for a token

        // Loop through each character in the source string
        while (m_pos < m_src.size())
        {
            char c = m_src[m_pos++]; ///< Get the current character and advance the position

            // Handle comments
            if (c == '/' && m_src[m_pos] == '>')
            {
                // Skip until the end of the line
                while (m_pos < m_src.size() && m_src[m_pos] != '\n')
                {
                    ++m_pos;
                }
            }
            // Handle whitespace or end of statement (semicolon)
            else if (std::isspace(c) || c == ';')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer)); ///< Parse the buffer as a token
                    buffer.clear();                       ///< Clear the buffer for the next token
                }
                if (c == ';')
                {
                    tokens.push_back({TokenType::_EOS, std::nullopt}); ///< Add end of statement token
                }
            }
            // Handle operators
            else if (c == '+')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_plus, "+"});
            }
            else if (c == '-')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_minus, "-"});
            }
            else if (c == '*')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_multiply, "*"});
            }
            else if (c == '%')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_percent, "%"});
            }
            else if (c == '=')
            {
                if (m_pos < m_src.size() && m_src[m_pos] == '=')
                {
                    if (!buffer.empty())
                    {
                        tokens.push_back(parseToken(buffer));
                        buffer.clear();
                    }
                    tokens.push_back({TokenType::_equal, "=="});
                    ++m_pos; ///< Skip the second '=' character
                }
                else
                {
                    if (!buffer.empty())
                    {
                        tokens.push_back(parseToken(buffer));
                        buffer.clear();
                    }
                    tokens.push_back({TokenType::_assign, "="});
                }
            }
            // Handle less than and greater than operators
            else if (c == '<')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_less_than, "<"});
            }
            else if (c == '>')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_greater_than, ">"});
            }
            // Handle curly braces
            else if (c == '{')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_open_brace, "{"});
            }
            else if (c == '}')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_close_brace, "}"});
            }
            // Handle parentheses
            else if (c == '(')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_open_paren, std::string(1, c)});
            }
            else if (c == ')')
            {
                if (!buffer.empty())
                {
                    tokens.push_back(parseToken(buffer));
                    buffer.clear();
                }
                tokens.push_back({TokenType::_close_paren, std::string(1, c)});
            }
            else
            {
                buffer += c; ///< Accumulate character in buffer
            }
        }

        // Handle any remaining buffer after the loop ends
        if (!buffer.empty())
        {
            tokens.push_back(parseToken(buffer));
        }

        return tokens;
    }

private:
    /**
     * @brief Parses a buffer string into a Token.
     * @param buffer The buffer containing the string to parse.
     * @return A Token representing the parsed buffer.
     */
    Token parseToken(const std::string &buffer)
    {
        if (buffer.empty())
            return {TokenType::_ERROR, std::nullopt}; ///< Return error token if buffer is empty

        TokenType type;
        std::optional<std::string> value = std::nullopt;

        // Check if the buffer is a valid integer
        if (std::isdigit(buffer[0]))
        {
            if (isValidInteger(buffer))
            {
                type = TokenType::_int;
                value = buffer;
            }
            else
            {
                std::cerr << "Error: Invalid integer: " << buffer << std::endl;
                type = TokenType::_ERROR;
            }
        }
        else
        {
            // Match the buffer to a keyword or identifier
            type = matchKeywordOrIdentifier(buffer, value);
        }

        return {type, value};
    }

    /**
     * @brief Matches a buffer string to a keyword or identifier.
     * @param buffer The buffer containing the string to match.
     * @param value The optional value to be set if the token is an identifier.
     * @return The TokenType representing the matched keyword or identifier.
     */
    TokenType matchKeywordOrIdentifier(const std::string &buffer, std::optional<std::string> &value)
    {
        if (buffer.empty())
            return TokenType::_ERROR;

        // Ensure the buffer starts with an alphabetic character or underscore
        if (!std::isalpha(buffer[0]) && buffer[0] != '_')
        {
            std::cerr << "Error: Invalid token: " << buffer << std::endl;
            return TokenType::_ERROR;
        }

        TokenType type;

        // Use switch-case to match the first character to known keywords
        switch (buffer[0])
        {
        case 'e':
            if (buffer == "exit")
                type = TokenType::_exit;
            else if (buffer == "else")
                type = TokenType::_else;
            else
                type = TokenType::_identifier;
            break;
        case 'f':
            if (buffer == "for")
                type = TokenType::_for;
            else
                type = TokenType::_identifier;
            break;
        case 'w':
            if (buffer == "while")
                type = TokenType::_while;
            else
                type = TokenType::_identifier;
            break;
        case 'l':
            if (buffer == "let")
                type = TokenType::_let;
            else
                type = TokenType::_identifier;
            break;
        case 'd':
            if (buffer == "define")
                type = TokenType::_define;
            else
                type = TokenType::_identifier;
            break;
        case 'i':
            if (buffer == "if")
                type = TokenType::_if;
            else
                type = TokenType::_identifier;
            break;
        default:
            // If not a keyword, check if it's a valid identifier
            if (isIdentifier(buffer))
                type = TokenType::_identifier;
            else
            {
                std::cerr << "Error: Invalid token: " << buffer << std::endl;
                type = TokenType::_ERROR;
            }
            break;
        }

        // If the token is an identifier, set the value
        if (type == TokenType::_identifier)
        {
            value = buffer;
        }

        return type;
    }

    /**
     * @brief Checks if a buffer string is a valid identifier.
     * @param buffer The buffer containing the string to check.
     * @return True if the buffer is a valid identifier, false otherwise.
     */
    bool isIdentifier(const std::string &buffer) const
    {
        for (char c : buffer)
        {
            if (!std::isalnum(c) && c != '_')
                return false;
        }
        return true;
    }

    /**
     * @brief Checks if a buffer string is a valid integer.
     * @param buffer The buffer containing the string to check.
     * @return True if the buffer is a valid integer, false otherwise.
     */
    bool isValidInteger(const std::string &buffer) const
    {
        for (char c : buffer)
        {
            if (!std::isdigit(c))
                return false;
        }
        return true;
    }

    const std::string m_src; ///< The source string to be tokenized.
    size_t m_pos;            ///< The current position in the source string.
};
