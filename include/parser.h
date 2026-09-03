#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "lexer.h"
#include "symboltable.h"

class Parser
{
private:
    Lexer lexer;
    Token currentToken;
    SymbolTable symbolTable;

public:
    explicit Parser(const std::string& source);

    void advance();
    bool check(LexTokenType type);
    bool checkValue(const std::string& value);

    [[noreturn]] void error(const std::string& message);

    void expect(LexTokenType type, const std::string& message);
    void expectKeyword(const std::string& keyword);
    void skipEndOfLines();

    void parseProgram();
    void parseStatement();
    void parseDeclaration();
    void parseAssignment();
    void parseShow();
    void parseIf();
    void parseWhile();
    void parseCondition();
    void parseExpression();
    void parseTerm();
    void parseFactor();
};

#endif
