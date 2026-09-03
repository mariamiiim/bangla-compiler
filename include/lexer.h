#ifndef LEXER_H
#define LEXER_H

#include <string>

enum class LexTokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,

    ASSIGN,

    EQUAL,
    NOT_EQUAL,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,

    END_OF_LINE,
    END_OF_FILE,

    INVALID
};

struct Token
{
    LexTokenType type;
    std::string value;
    int line;
};

std::string tokenName(LexTokenType type);

class Lexer
{
private:
    std::string source;
    size_t position;
    int line;

public:
    explicit Lexer(const std::string& input);

    unsigned char current();
    unsigned char peek(size_t offset = 1);
    void advance();

    bool isAsciiLetter(unsigned char c);
    bool isDigit(unsigned char c);
    bool isWordTerminator(unsigned char c);

    Token readWord();
    Token readAsciiIdentifier();
    Token readNumber();
    Token readString();
    void skipComment();

    Token nextToken();
};

#endif
