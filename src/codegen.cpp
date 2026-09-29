#include "codegen.h"
#include <sstream>

using namespace std;

static string compareMnemonic(const string& op)
{
    if (op == "==") return "CMPEQ";
    if (op == "!=") return "CMPNE";
    if (op == ">")  return "CMPGT";
    if (op == "<")  return "CMPLT";
    if (op == ">=") return "CMPGE";
    return "CMPLE";
}

string CodeGenerator::newLabel()
{
    return "L" + to_string(labelCounter++);
}

void CodeGenerator::emit(const string& line)
{
    code.push_back(line);
}

void CodeGenerator::genExpr(const ExprPtr& expr)
{
    switch (expr->kind)
    {
        case ExprKind::NUMBER:
        {
            double v = static_cast<NumberExpr*>(expr.get())->value;
            ostringstream out;
            if (v == static_cast<long long>(v)) out << static_cast<long long>(v);
            else out << v;
            emit("    PUSH " + out.str());
            return;
        }

        case ExprKind::STRING:
            emit("    PUSH \"" + static_cast<StringExpr*>(expr.get())->value + "\"");
            return;

        case ExprKind::VARIABLE:
            emit("    LOAD " + static_cast<VariableExpr*>(expr.get())->name);
            return;

        case ExprKind::BINARY:
        {
            auto* bin = static_cast<BinaryExpr*>(expr.get());
            genExpr(bin->left);
            genExpr(bin->right);

            if (bin->op == "+")
                emit(bin->type == ValueType::STRING ? "    CONCAT" : "    ADD");
            else if (bin->op == "-") emit("    SUB");
            else if (bin->op == "*") emit("    MUL");
            else if (bin->op == "/") emit("    DIV");
            else if (bin->op == "%") emit("    MOD");
            else emit("    " + compareMnemonic(bin->op));
            return;
        }
    }
}

void CodeGenerator::genCondition(const Condition& condition)
{
    genExpr(condition.left);
    genExpr(condition.right);
    emit("    " + compareMnemonic(condition.op));
}

void CodeGenerator::genBlock(const shared_ptr<BlockStmt>& block)
{
    if (!block) return;
    for (const StmtPtr& s : block->statements) genStmt(s);
}

void CodeGenerator::genStmt(const StmtPtr& stmt)
{
    switch (stmt->kind)
    {
        case StmtKind::DECLARATION:
        {
            auto* d = static_cast<DeclarationStmt*>(stmt.get());
            genExpr(d->value);
            emit("    STORE " + d->name);
            return;
        }

        case StmtKind::ASSIGNMENT:
        {
            auto* a = static_cast<AssignmentStmt*>(stmt.get());
            genExpr(a->value);
            emit("    STORE " + a->name);
            return;
        }

        case StmtKind::SHOW:
            genExpr(static_cast<ShowStmt*>(stmt.get())->value);
            emit("    PRINT");
            return;

        case StmtKind::IF:
        {
            auto* i = static_cast<IfStmt*>(stmt.get());
            string elseLabel = newLabel();
            string endLabel = i->elseBlock ? newLabel() : elseLabel;

            genCondition(i->condition);
            emit("    JZ " + elseLabel);
            genBlock(i->thenBlock);

            if (i->elseBlock)
            {
                emit("    JMP " + endLabel);
                emit(elseLabel + ":");
                genBlock(i->elseBlock);
            }

            emit(endLabel + ":");
            return;
        }

        case StmtKind::WHILE:
        {
            auto* w = static_cast<WhileStmt*>(stmt.get());
            string startLabel = newLabel();
            string endLabel = newLabel();

            emit(startLabel + ":");
            genCondition(w->condition);
            emit("    JZ " + endLabel);
            genBlock(w->body);
            emit("    JMP " + startLabel);
            emit(endLabel + ":");
            return;
        }

        case StmtKind::BLOCK:
            genBlock(static_pointer_cast<BlockStmt>(stmt));
            return;
    }
}

vector<string> CodeGenerator::generate(const Program& program)
{
    code.clear();
    labelCounter = 0;

    for (const StmtPtr& s : program.statements) genStmt(s);
    emit("    HALT");

    return code;
}
