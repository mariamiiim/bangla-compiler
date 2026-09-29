#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <string>
#include <vector>
#include "ast.h"
#include "symboltable.h"

// The semantic analyzer walks the AST produced by the (purely syntactic)
// Parser and:
//   1. owns the symbol table (declares variables, catches
//      redeclaration / use-before-declaration),
//   2. type-checks every expression (সংখ্যা vs লেখা),
//   3. annotates every Expr node's `type` field so later phases
//      (IR generation, optimization) don't need to re-derive it.
class SemanticAnalyzer
{
private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;

    ValueType analyzeExpr(const ExprPtr& expr);
    void analyzeCondition(Condition& condition);
    void analyzeStmt(const StmtPtr& stmt);
    void analyzeBlock(const std::shared_ptr<BlockStmt>& block);

    void reportError(int line, const std::string& message);

public:
    // Runs semantic analysis over the whole program.
    // Returns true if no semantic errors were found.
    bool analyze(Program& program);

    const std::vector<std::string>& getErrors() const;
    const SymbolTable& getSymbolTable() const;
};

#endif
