#ifndef CODEGEN_H
#define CODEGEN_H

#include <string>
#include <vector>
#include "ast.h"

// Code generation phase: walks the (semantically checked) AST and emits
// stack-machine style assembly directly. Expressions are generated in
// post-order (left, right, operator), so precedence and parentheses are
// already handled by the shape of the tree.
class CodeGenerator
{
private:
    std::vector<std::string> code;
    int labelCounter = 0;

    std::string newLabel();
    void emit(const std::string& line);

    void genExpr(const ExprPtr& expr);
    void genCondition(const Condition& condition);
    void genStmt(const StmtPtr& stmt);
    void genBlock(const std::shared_ptr<BlockStmt>& block);

public:
    std::vector<std::string> generate(const Program& program);
};

#endif
