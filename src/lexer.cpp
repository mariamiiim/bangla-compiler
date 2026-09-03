#include "lexer.h"
#include <unordered_map>

using namespace std;

static const unordered_map<string, LexTokenType> keywords =
{
    {"ধরি", LexTokenType::KEYWORD},
    {"সংখ্যা", LexTokenType::KEYWORD},
    {"লেখা", LexTokenType::KEYWORD},
    {"দেখাও", LexTokenType::KEYWORD},

    {"যদি", LexTokenType::KEYWORD},
    {"নাহলে", LexTokenType::KEYWORD},
    {"তবে", LexTokenType::KEYWORD},

    {"যতক্ষণ", LexTokenType::KEYWORD},
    {"করো", LexTokenType::KEYWORD},

    {"শেষ", LexTokenType::KEYWORD}
};

string tokenName(LexTokenType type)
{
    switch (type)
    {
        case LexTokenType::KEYWORD:       return "KEYWORD";
        case LexTokenType::IDENTIFIER:    return "IDENTIFIER";
        case LexTokenType::NUMBER:        return "NUMBER";
        case LexTokenType::STRING:        return "STRING";
        case LexTokenType::PLUS:          return "PLUS";
        case LexTokenType::MINUS:         return "MINUS";
        case LexTokenType::MULTIPLY:      return "MULTIPLY";
        case LexTokenType::DIVIDE:        return "DIVIDE";
        case LexTokenType::MODULO:        return "MODULO";
        case LexTokenType::ASSIGN:        return "ASSIGN";
        case LexTokenType::EQUAL:         return "EQUAL";
        case LexTokenType::NOT_EQUAL:     return "NOT_EQUAL";
        case LexTokenType::GREATER:       return "GREATER";
        case LexTokenType::LESS:          return "LESS";
        case LexTokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case LexTokenType::LESS_EQUAL:    return "LESS_EQUAL";
        case LexTokenType::LEFT_PAREN:    return "LEFT_PAREN";
        case LexTokenType::RIGHT_PAREN:   return "RIGHT_PAREN";
        case LexTokenType::LEFT_BRACE:    return "LEFT_BRACE";
        case LexTokenType::RIGHT_BRACE:   return "RIGHT_BRACE";
        case LexTokenType::SEMICOLON:     return "SEMICOLON";
        case LexTokenType::END_OF_LINE:   return "END_OF_LINE";
        case LexTokenType::END_OF_FILE:   return "EOF";
        case LexTokenType::INVALID:       return "INVALID";
    }

    return "UNKNOWN";
}

Lexer::Lexer(const string& input)
{
    source = input;
    position = 0;
    line = 1;
}

unsigned char Lexer::current()
{
    if (position >= source.size())
    {
        return '\0';
    }

    return static_cast<unsigned char>(source[position]);
}

unsigned char Lexer::peek(size_t offset)
{
    if (position + offset >= source.size())
    {
        return '\0';
    }

    return static_cast<unsigned char>(source[position + offset]);
}

void Lexer::advance()
{
    if (position < source.size())
    {
        position++;
    }
}

bool Lexer::isAsciiLetter(unsigned char c)
{
    return
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        c == '_';
}

bool Lexer::isDigit(unsigned char c)
{
    return c >= '0' && c <= '9';
}

bool Lexer::isWordTerminator(unsigned char c)
{
    return
        c == ' ' ||
        c == '\n' ||
        c == '\r' ||
        c == '\t' ||
        c == '\0' ||

        c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/' ||
        c == '%' ||

        c == '<' ||
        c == '>' ||
        c == '=' ||
        c == '!' ||

        c == '"' ||

        c == '(' ||
        c == ')' ||

        c == '{' ||
        c == '}' ||

        c == ';';
}

Token Lexer::readWord()
{
    string value;
    int startLine = line;

    while (position < source.size())
    {
        unsigned char c = current();

        if (isWordTerminator(c))
        {
            break;
        }

        value += static_cast<char>(c);
        advance();
    }

    if (value.empty())
    {
        unsigned char badChar = current();
        advance();

        return { LexTokenType::INVALID, string(1, static_cast<char>(badChar)), startLine };
    }

    auto found = keywords.find(value);

    if (found != keywords.end())
    {
        return { LexTokenType::KEYWORD, value, startLine };
    }

    return { LexTokenType::IDENTIFIER, value, startLine };
}

Token Lexer::readAsciiIdentifier()
{
    string value;
    int startLine = line;

    while (position < source.size())
    {
        unsigned char c = current();

        if (isAsciiLetter(c) || isDigit(c))
        {
            value += static_cast<char>(c);
            advance();
        }
        else
        {
            break;
        }
    }

    return { LexTokenType::IDENTIFIER, value, startLine };
}

Token Lexer::readNumber()
{
    string value;
    int startLine = line;

    while (position < source.size() && isDigit(current()))
    {
        value += static_cast<char>(current());
        advance();
    }

    return { LexTokenType::NUMBER, value, startLine };
}

Token Lexer::readString()
{
    int startLine = line;

    advance();

    string value;

    while (position < source.size())
    {
        unsigned char c = current();

        if (c == '"')
        {
            advance();
            return { LexTokenType::STRING, value, startLine };
        }

        if (c == '\n')
        {
            line++;
            value += '\n';
            advance();
            continue;
        }

        value += static_cast<char>(c);
        advance();
    }

    return { LexTokenType::INVALID, "Unterminated string", startLine };
}

void Lexer::skipComment()
{
    advance();
    advance();

    while (position < source.size())
    {
        unsigned char c = current();

        if (c == '\n')
        {
            break;
        }

        advance();
    }
}

Token Lexer::nextToken()
{
    while (position < source.size())
    {
        unsigned char c = current();

        if (c == ' ' || c == '\t' || c == '\r')
        {
            advance();
            continue;
        }

        if (c == '/' && peek() == '/')
        {
            skipComment();
            continue;
        }

        break;
    }

    if (position >= source.size())
    {
        return { LexTokenType::END_OF_FILE, "", line };
    }

    unsigned char c = current();
    int currentLine = line;

    if (c == '\n')
    {
        advance();

        Token token = { LexTokenType::END_OF_LINE, "\\n", currentLine };
        line++;
        return token;
    }

    if (isDigit(c))
    {
        return readNumber();
    }

    if (isAsciiLetter(c))
    {
        return readAsciiIdentifier();
    }

    if (c == '"')
    {
        return readString();
    }

    bool isKnownAsciiOperator =
        c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
        c == '<' || c == '>' || c == '=' || c == '!' ||
        c == '(' || c == ')' || c == '{' || c == '}' || c == ';';

    if (!isKnownAsciiOperator)
    {
        // Not ASCII letter/digit/known operator -> could be a UTF-8
        // multibyte character (Bangla text) or keyword, handled by readWord().
        return readWord();
    }

    if (c == '<' && peek() == '-')
    {
        advance();
        advance();
        return { LexTokenType::ASSIGN, "<-", currentLine };
    }

    if (c == '=' && peek() == '=')
    {
        advance();
        advance();
        return { LexTokenType::EQUAL, "==", currentLine };
    }

    if (c == '!' && peek() == '=')
    {
        advance();
        advance();
        return { LexTokenType::NOT_EQUAL, "!=", currentLine };
    }

    if (c == '>' && peek() == '=')
    {
        advance();
        advance();
        return { LexTokenType::GREATER_EQUAL, ">=", currentLine };
    }

    if (c == '<' && peek() == '=')
    {
        advance();
        advance();
        return { LexTokenType::LESS_EQUAL, "<=", currentLine };
    }

    advance();

    switch (c)
    {
        case '+': return { LexTokenType::PLUS, "+", currentLine };
        case '-': return { LexTokenType::MINUS, "-", currentLine };
        case '*': return { LexTokenType::MULTIPLY, "*", currentLine };
        case '/': return { LexTokenType::DIVIDE, "/", currentLine };
        case '%': return { LexTokenType::MODULO, "%", currentLine };
        case '>': return { LexTokenType::GREATER, ">", currentLine };
        case '<': return { LexTokenType::LESS, "<", currentLine };
        case '(': return { LexTokenType::LEFT_PAREN, "(", currentLine };
        case ')': return { LexTokenType::RIGHT_PAREN, ")", currentLine };
        case '{': return { LexTokenType::LEFT_BRACE, "{", currentLine };
        case '}': return { LexTokenType::RIGHT_BRACE, "}", currentLine };
        case ';': return { LexTokenType::SEMICOLON, ";", currentLine };
        case '=': return { LexTokenType::INVALID, "=", currentLine };
        case '!': return { LexTokenType::INVALID, "!", currentLine };
        default:  return { LexTokenType::INVALID, string(1, static_cast<char>(c)), currentLine };
    }
}
