#include "parser.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// ============================================================
// Complete formal grammar in BNF (Backus-Naur Form)
// implemented by this recursive-descent parser + the lexer
// ============================================================
//
// Notation: <name> = non-terminal, "text" = terminal (literal token),
//           ::= means "is defined as", | means "or",
//           ε   = the empty string.
// Only pure BNF is used (no *, +, [ ], ( ) shorthand); repetition is
// written with recursion.
//
// ------------------------------------------------------------
// PART 1 : SYNTAX GRAMMAR  (rules used by the parser)
// ------------------------------------------------------------
//
// <program>        ::= <eols> <statement_list>
//
// <statement_list> ::= ε
//                    | <statement> <eols> <statement_list>
//
// <eols>           ::= ε
//                    | END_OF_LINE <eols>
//
// <statement>      ::= <declaration>
//                    | <assignment>
//                    | <show>
//                    | <if_stmt>
//                    | <while_stmt>
//
// <declaration>    ::= "ধরি" <type> IDENTIFIER "<-" <expression> ";"
// <type>           ::= "সংখ্যা"
//                    | "লেখা"
// <assignment>     ::= IDENTIFIER "<-" <expression> ";"
// <show>           ::= "দেখাও" <expression> ";"
//
// <if_stmt>        ::= "যদি" <condition> "তবে" <block> "শেষ"
//                    | "যদি" <condition> "তবে" <block> "নাহলে" <block> "শেষ"
//
// <while_stmt>     ::= "যতক্ষণ" <condition> "করো" <block> "শেষ"
//
// <block>          ::= <eols> <statement_list>
//
// <condition>      ::= <expression> <relop> <expression>
// <relop>          ::= "==" | "!=" | ">" | "<" | ">=" | "<="
//
// <expression>     ::= <term>
//                    | <expression> "+" <term>
//                    | <expression> "-" <term>
//
// <term>           ::= <factor>
//                    | <term> "*" <factor>
//                    | <term> "/" <factor>
//                    | <term> "%" <factor>
//
// <factor>         ::= NUMBER
//                    | STRING
//                    | IDENTIFIER
//                    | "(" <expression> ")"
//
// ------------------------------------------------------------
// PART 2 : LEXICAL GRAMMAR  (rules used by the lexer to form tokens)
// ------------------------------------------------------------
//
// NUMBER           ::= <digit> | <digit> NUMBER
// <digit>          ::= "0" | "1" | "2" | "3" | "4"
//                    | "5" | "6" | "7" | "8" | "9"
//
// IDENTIFIER       ::= <ascii_ident> | <word>
// <ascii_ident>    ::= <ascii_letter> <ascii_rest>
// <ascii_rest>     ::= ε
//                    | <ascii_letter> <ascii_rest>
//                    | <digit> <ascii_rest>
// <ascii_letter>   ::= "A" | ... | "Z" | "a" | ... | "z" | "_"
//
// <word>           ::= <word_start> <word_rest>
//                      (any Bangla/UTF-8 word that is NOT a keyword;
//                       it ends at the first word terminator below)
// <word_start>     ::= any character that is not an ASCII letter, digit,
//                      operator, delimiter or whitespace
// <word_rest>      ::= ε
//                    | <word_char> <word_rest>
// <word_char>      ::= any character that is not a <word_terminator>
// <word_terminator>::= " " | TAB | CR | LF | "+" | "-" | "*" | "/" | "%"
//                    | "<" | ">" | "=" | "!" | "\"" | "(" | ")"
//                    | "{" | "}" | ";"
//
// STRING           ::= "\"" <string_chars> "\""
// <string_chars>   ::= ε
//                    | <string_char> <string_chars>
// <string_char>    ::= any character except "\""
//
// END_OF_LINE      ::= LF
//
// <comment>        ::= "//" <comment_chars>      (skipped by the lexer)
// <comment_chars>  ::= ε
//                    | <non_lf_char> <comment_chars>
// <non_lf_char>    ::= any character except LF
//
// Keywords (reserved words, cannot be used as identifiers):
//   "ধরি"  "সংখ্যা"  "লেখা"  "দেখাও"  "যদি"  "তবে"  "নাহলে"
//   "যতক্ষণ"  "করো"  "শেষ"
//
// Whitespace (space, TAB, CR) between tokens is ignored.
// The tokens "{" and "}" are recognised by the lexer but are not
// used by any rule of the current grammar.
//
// ------------------------------------------------------------
// Notes
// ------------------------------------------------------------
//   - Operator precedence: "*" "/" "%" bind tighter than "+" "-".
//   - Both levels are left-associative (left-recursive rules above;
//     the parser implements them with loops).
//   - A <block> ends at the parent's terminator keyword ("নাহলে" or
//     "শেষ") or at end of file.
// ============================================================

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

Program Parser::parseProgram()
{
    cout << "====================================" << endl;
    cout << "       বাংলা ভাষা - PARSER" << endl;
    cout << "====================================" << endl;

    Program program;

    skipEndOfLines();

    while (!check(LexTokenType::END_OF_FILE))
    {
        program.statements.push_back(parseStatement());
        skipEndOfLines();
    }

    cout << endl;
    cout << "Parsing Successful!" << endl;
    cout << "No syntax errors found." << endl;

    return program;
}

StmtPtr Parser::parseStatement()
{
    if (check(LexTokenType::KEYWORD) && checkValue("ধরি"))
    {
        return parseDeclaration();
    }

    if (check(LexTokenType::KEYWORD) && checkValue("দেখাও"))
    {
        return parseShow();
    }

    if (check(LexTokenType::KEYWORD) && checkValue("যদি"))
    {
        return parseIf();
    }

    if (check(LexTokenType::KEYWORD) && checkValue("যতক্ষণ"))
    {
        return parseWhile();
    }

    if (check(LexTokenType::IDENTIFIER))
    {
        return parseAssignment();
    }

    error("Invalid statement");
}

StmtPtr Parser::parseDeclaration()
{
    int declLine = currentToken.line;
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
    advance();

    expect(LexTokenType::ASSIGN, "Expected '<-'");

    ExprPtr value = parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after declaration");

    return make_shared<DeclarationStmt>(typeKeyword, varName, value, declLine);
}

StmtPtr Parser::parseAssignment()
{
    if (!check(LexTokenType::IDENTIFIER))
    {
        error("Expected variable name");
    }

    int stmtLine = currentToken.line;
    string varName = currentToken.value;
    advance();

    expect(LexTokenType::ASSIGN, "Expected '<-'");

    ExprPtr value = parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after assignment");

    return make_shared<AssignmentStmt>(varName, value, stmtLine);
}

StmtPtr Parser::parseShow()
{
    int stmtLine = currentToken.line;
    expectKeyword("দেখাও");

    ExprPtr value = parseExpression();

    expect(LexTokenType::SEMICOLON, "Expected ';' after দেখাও");

    return make_shared<ShowStmt>(value, stmtLine);
}

shared_ptr<BlockStmt> Parser::parseBlock(const vector<string>& terminatorKeywords)
{
    int blockLine = currentToken.line;
    vector<StmtPtr> statements;

    skipEndOfLines();

    auto atTerminator = [&]()
    {
        if (check(LexTokenType::END_OF_FILE))
        {
            return true;
        }

        if (!check(LexTokenType::KEYWORD))
        {
            return false;
        }

        return find(terminatorKeywords.begin(), terminatorKeywords.end(), currentToken.value)
            != terminatorKeywords.end();
    };

    while (!atTerminator())
    {
        statements.push_back(parseStatement());
        skipEndOfLines();
    }

    return make_shared<BlockStmt>(statements, blockLine);
}

StmtPtr Parser::parseIf()
{
    int stmtLine = currentToken.line;
    expectKeyword("যদি");

    Condition condition = parseCondition();

    expectKeyword("তবে");

    shared_ptr<BlockStmt> thenBlock = parseBlock({"নাহলে", "শেষ"});
    shared_ptr<BlockStmt> elseBlock = nullptr;

    if (check(LexTokenType::KEYWORD) && checkValue("নাহলে"))
    {
        advance();
        elseBlock = parseBlock({"শেষ"});
    }

    expectKeyword("শেষ");

    return make_shared<IfStmt>(condition, thenBlock, elseBlock, stmtLine);
}

StmtPtr Parser::parseWhile()
{
    int stmtLine = currentToken.line;
    expectKeyword("যতক্ষণ");

    Condition condition = parseCondition();

    expectKeyword("করো");

    shared_ptr<BlockStmt> body = parseBlock({"শেষ"});

    expectKeyword("শেষ");

    return make_shared<WhileStmt>(condition, body, stmtLine);
}

Condition Parser::parseCondition()
{
    Condition condition;

    condition.left = parseExpression();

    if (
        check(LexTokenType::EQUAL) ||
        check(LexTokenType::NOT_EQUAL) ||
        check(LexTokenType::GREATER) ||
        check(LexTokenType::LESS) ||
        check(LexTokenType::GREATER_EQUAL) ||
        check(LexTokenType::LESS_EQUAL)
    )
    {
        condition.op = currentToken.value;
        advance();
    }
    else
    {
        error("Expected comparison operator");
    }

    condition.right = parseExpression();

    return condition;
}

ExprPtr Parser::parseExpression()
{
    int startLine = currentToken.line;
    ExprPtr left = parseTerm();

    while (check(LexTokenType::PLUS) || check(LexTokenType::MINUS))
    {
        string op = currentToken.value;
        advance();
        ExprPtr right = parseTerm();
        left = make_shared<BinaryExpr>(op, left, right, startLine);
    }

    return left;
}

ExprPtr Parser::parseTerm()
{
    int startLine = currentToken.line;
    ExprPtr left = parseFactor();

    while (
        check(LexTokenType::MULTIPLY) ||
        check(LexTokenType::DIVIDE) ||
        check(LexTokenType::MODULO)
    )
    {
        string op = currentToken.value;
        advance();
        ExprPtr right = parseFactor();
        left = make_shared<BinaryExpr>(op, left, right, startLine);
    }

    return left;
}

ExprPtr Parser::parseFactor()
{
    int ln = currentToken.line;

    if (check(LexTokenType::NUMBER))
    {
        double value = strtod(currentToken.value.c_str(), nullptr);
        advance();
        return make_shared<NumberExpr>(value, ln);
    }

    if (check(LexTokenType::IDENTIFIER))
    {
        string name = currentToken.value;
        advance();
        return make_shared<VariableExpr>(name, ln);
    }

    if (check(LexTokenType::STRING))
    {
        string value = currentToken.value;
        advance();
        return make_shared<StringExpr>(value, ln);
    }

    if (check(LexTokenType::LEFT_PAREN))
    {
        advance();
        ExprPtr inner = parseExpression();
        expect(LexTokenType::RIGHT_PAREN, "Expected ')'");
        return inner;
    }

    error("Expected number, identifier, string or expression");
}
