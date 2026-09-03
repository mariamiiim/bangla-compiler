#include "parser.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Parser::Parser(const string& source)
    : lexer(source)
{
    advance();
}

void Parser::advance()
{
    currentToken = lexer.nextToken();
}

bool Parser::check(LexTokenType type)
{
    return currentToken.type == type;
}

bool Parser::checkValue(const string& value)
{
    return currentToken.value == value;
}

void Parser::error(const string& message)
{
    cout << "PARSER ERROR at line " << currentToken.line << ": " << message << endl;
    cout << "Found: [" << currentToken.value << "]" << endl;
    exit(1);
}

void Parser::expect(LexTokenType type, const string& message)
{
    if (!check(type))
    {
        error(message);
    }

    advance();
}

void Parser::expectKeyword(const string& keyword)
{
    if (!check(LexTokenType::KEYWORD) || !checkValue(keyword))
    {
        error("Expected keyword: " + keyword);
    }

    advance();
}

void Parser::skipEndOfLines()
{
    while (check(LexTokenType::END_OF_LINE))
    {
        advance();
    }
}

void Parser::parseProgram()
{
    cout << "====================================" << endl;
    cout << "       বাংলা ভাষা - PARSER" << endl;
    cout << "====================================" << endl;

    skipEndOfLines();

    while (!check(LexTokenType::END_OF_FILE))
    {
        parseStatement();
        skipEndOfLines();
    }

    cout << endl;
    cout << "Parsing Successful!" << endl;
    cout << "No syntax errors found." << endl;
}

void Parser::parseStatement()
{
    if (check(LexTokenType::KEYWORD) && checkValue("ধরি"))
    {
        parseDeclaration();
        return;
    }

    if (check(LexTokenType::KEYWORD) && checkValue("দেখাও"))
    {
        parseShow();
        return;
    }

    if (check(LexTokenType::KEYWORD) && checkValue("যদি"))
    {
        parseIf();
        return;
    }

    if (check(LexTokenType::KEYWORD) && checkValue("যতক্ষণ"))
    {
        parseWhile();
        return;
    }

    if (check(LexTokenType::IDENTIFIER))
    {
        parseAssignment();
        return;
    }

    error("Invalid statement");
}

void Parser::parseDeclaration()
{
    expectKeyword("ধরি");

    if (!check(LexTokenType::KEYWORD) ||
        !(checkValue("সংখ্যা") || checkValue("লেখা")))
    {
        error("Expected type: সংখ্যা or লেখা");
    }

    string typeKeyword = currentToken.value;
    advance();

    if (!check(LexTokenType::IDENTIFIER))
    {
        error("Expected variable name");
    }

    string varName = currentToken.value;
    int declLine = currentToken.line;
    advance();

    if (symbolTable.exists(varName))
    {
        error("Variable already declared: " + varName);
    }

    expect(LexTokenType::ASSIGN, "Expected '<-'");

    parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after declaration");

    symbolTable.declare(varName, symbolTypeFromKeyword(typeKeyword), declLine);
}

void Parser::parseAssignment()
{
    if (!check(LexTokenType::IDENTIFIER))
    {
        error("Expected variable name");
    }

    string varName = currentToken.value;
    advance();

    if (!symbolTable.exists(varName))
    {
        error("Variable not declared before use: " + varName);
    }

    expect(LexTokenType::ASSIGN, "Expected '<-'");

    parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after assignment");
}

void Parser::parseShow()
{
    expectKeyword("দেখাও");

    parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after দেখাও");
}

void Parser::parseIf()
{
    expectKeyword("যদি");

    parseCondition();

    expectKeyword("তবে");

    skipEndOfLines();

    while (
        !check(LexTokenType::END_OF_FILE) &&
        !(check(LexTokenType::KEYWORD) &&
          (checkValue("নাহলে") || checkValue("শেষ")))
    )
    {
        parseStatement();
        skipEndOfLines();
    }

    if (check(LexTokenType::KEYWORD) && checkValue("নাহলে"))
    {
        advance();
        skipEndOfLines();

        while (
            !check(LexTokenType::END_OF_FILE) &&
            !(check(LexTokenType::KEYWORD) && checkValue("শেষ"))
        )
        {
            parseStatement();
            skipEndOfLines();
        }
    }

    expectKeyword("শেষ");
}

void Parser::parseWhile()
{
    expectKeyword("যতক্ষণ");

    parseCondition();

    expectKeyword("করো");

    skipEndOfLines();

    while (
        !check(LexTokenType::END_OF_FILE) &&
        !(check(LexTokenType::KEYWORD) && checkValue("শেষ"))
    )
    {
        parseStatement();
        skipEndOfLines();
    }

    expectKeyword("শেষ");
}

void Parser::parseCondition()
{
    parseExpression();

    if (
        check(LexTokenType::EQUAL) ||
        check(LexTokenType::NOT_EQUAL) ||
        check(LexTokenType::GREATER) ||
        check(LexTokenType::LESS) ||
        check(LexTokenType::GREATER_EQUAL) ||
        check(LexTokenType::LESS_EQUAL)
    )
    {
        advance();
    }
    else
    {
        error("Expected comparison operator");
    }

    parseExpression();
}

void Parser::parseExpression()
{
    parseTerm();

    while (check(LexTokenType::PLUS) || check(LexTokenType::MINUS))
    {
        advance();
        parseTerm();
    }
}

void Parser::parseTerm()
{
    parseFactor();

    while (
        check(LexTokenType::MULTIPLY) ||
        check(LexTokenType::DIVIDE) ||
        check(LexTokenType::MODULO)
    )
    {
        advance();
        parseFactor();
    }
}

void Parser::parseFactor()
{
    if (check(LexTokenType::NUMBER))
    {
        advance();
        return;
    }

    if (check(LexTokenType::IDENTIFIER))
    {
        if (!symbolTable.exists(currentToken.value))
        {
            error("Variable not declared before use: " + currentToken.value);
        }

        advance();
        return;
    }

    if (check(LexTokenType::STRING))
    {
        advance();
        return;
    }

    if (check(LexTokenType::LEFT_PAREN))
    {
        advance();
        parseExpression();
        expect(LexTokenType::RIGHT_PAREN, "Expected ')'");
        return;
    }

    error("Expected number, identifier, string or expression");
}
