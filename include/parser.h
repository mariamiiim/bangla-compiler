#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "lexer.h"
#include "ast.h"

// ============================================================
// Formal grammar (BNF) implemented by this recursive-descent parser
// ============================================================
// program      ::= statement*
// statement    ::= declaration | assignment | show | if_stmt | while_stmt
//
// declaration  ::= "ধরি" type IDENTIFIER "<-" expression ";"
// type         ::= "সংখ্যা" | "লেখা"
// assignment   ::= IDENTIFIER "<-" expression ";"
// show         ::= "দেখাও" expression ";"
// if_stmt      ::= "যদি" condition "তবে" statement* [ "নাহলে" statement* ] "শেষ"
// while_stmt   ::= "যতক্ষণ" condition "করো" statement* "শেষ"
//
// condition    ::= expression relop expression
// relop        ::= "==" | "!=" | ">" | "<" | ">=" | "<="
//
// expression   ::= term ( ("+" | "-") term )*
// term         ::= factor ( ("*" | "/" | "%") factor )*
// factor       ::= NUMBER | STRING | IDENTIFIER | "(" expression ")"
// ============================================================

// The parser is now a pure SYNTAX phase: it only checks the token stream
// against the grammar and builds an AST. It no longer knows about the
// symbol table or variable types at all -- that work belongs to the
// semantic analyzer (see semantic.h), which runs after this.
class Parser
{
private:
    Lexer lexer;
    Token currentToken;

public:
    explicit Parser(const std::string& source);

    void advance();
    bool check(LexTokenType type);
    bool checkValue(const std::string& value);

    [[noreturn]] void error(const std::string& message);

    void expect(LexTokenType type, const std::string& message);
    void expectKeyword(const std::string& keyword);
    void skipEndOfLines();

    Program parseProgram();

    StmtPtr parseStatement();
    StmtPtr parseDeclaration();
    StmtPtr parseAssignment();
    StmtPtr parseShow();
    StmtPtr parseIf();
    StmtPtr parseWhile();

    std::shared_ptr<BlockStmt> parseBlock(const std::vector<std::string>& terminatorKeywords);
    Condition parseCondition();

    ExprPtr parseExpression();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
};

#endif
