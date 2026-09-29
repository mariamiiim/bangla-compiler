#include "semantic.h"
#include <iostream>

using namespace std;

static ValueType valueTypeFromSymbolType(SymbolType type)
{
    return type == SymbolType::NUMBER ? ValueType::NUMBER : ValueType::STRING;
}

string valueTypeName(ValueType type)
{
    switch (type)
    {
    case ValueType::NUMBER:
        return "সংখ্যা";
    case ValueType::STRING:
        return "লেখা";
    case ValueType::UNKNOWN:
        return "unknown";
    }

    return "unknown";
}

void SemanticAnalyzer::reportError(int line, const string &message)
{
    errors.push_back("Line " + to_string(line) + ": " + message);
}

ValueType SemanticAnalyzer::analyzeExpr(const ExprPtr &expr)
{
    if (!expr)
    {
        return ValueType::UNKNOWN;
    }

    switch (expr->kind)
    {
    case ExprKind::NUMBER:
    {
        expr->type = ValueType::NUMBER;
        return expr->type;
    }

    case ExprKind::STRING:
    {
        expr->type = ValueType::STRING;
        return expr->type;
    }

    case ExprKind::VARIABLE:
    {
        auto *var = static_cast<VariableExpr *>(expr.get());
        const SymbolInfo *info = symbolTable.lookup(var->name);

        if (!info)
        {
            reportError(expr->line, "Variable not declared before use: " + var->name);
            expr->type = ValueType::UNKNOWN;
            return expr->type;
        }

        expr->type = valueTypeFromSymbolType(info->type);
        return expr->type;
    }

    case ExprKind::BINARY:
    {
        auto *bin = static_cast<BinaryExpr *>(expr.get());
        ValueType leftType = analyzeExpr(bin->left);
        ValueType rightType = analyzeExpr(bin->right);

        bool isArithmetic =
            bin->op == "+" || bin->op == "-" ||
            bin->op == "*" || bin->op == "/" || bin->op == "%";

        if (leftType == ValueType::UNKNOWN || rightType == ValueType::UNKNOWN)
        {
            expr->type = ValueType::UNKNOWN;
            return expr->type;
        }

        if (isArithmetic)
        {
            if (bin->op == "+" && leftType == ValueType::STRING && rightType == ValueType::STRING)
            {
                expr->type = ValueType::STRING;
                return expr->type;
            }

            if (leftType != ValueType::NUMBER || rightType != ValueType::NUMBER)
            {
                reportError(expr->line,
                            "Type mismatch: operator '" + bin->op + "' needs সংখ্যা operands, got " +
                                valueTypeName(leftType) + " and " + valueTypeName(rightType));
                expr->type = ValueType::UNKNOWN;
                return expr->type;
            }

            expr->type = ValueType::NUMBER;
            return expr->type;
        }

        if (leftType != rightType)
        {
            reportError(expr->line,
                        "Type mismatch: cannot compare " + valueTypeName(leftType) +
                            " with " + valueTypeName(rightType));
            expr->type = ValueType::UNKNOWN;
            return expr->type;
        }

        expr->type = ValueType::NUMBER;
        return expr->type;
    }
    }

    return ValueType::UNKNOWN;
}

void SemanticAnalyzer::analyzeCondition(Condition &condition)
{
    ValueType leftType = analyzeExpr(condition.left);
    ValueType rightType = analyzeExpr(condition.right);

    if (leftType != ValueType::UNKNOWN && rightType != ValueType::UNKNOWN && leftType != rightType)
    {
        reportError(condition.left->line,
                    "Type mismatch in condition: cannot compare " + valueTypeName(leftType) +
                        " with " + valueTypeName(rightType));
    }
}

void SemanticAnalyzer::analyzeBlock(const shared_ptr<BlockStmt> &block)
{
    if (!block)
    {
        return;
    }

    for (const StmtPtr &stmt : block->statements)
    {
        analyzeStmt(stmt);
    }
}

void SemanticAnalyzer::analyzeStmt(const StmtPtr &stmt)
{
    switch (stmt->kind)
    {
    case StmtKind::DECLARATION:
    {
        auto *decl = static_cast<DeclarationStmt *>(stmt.get());

        if (symbolTable.exists(decl->name))
        {
            reportError(stmt->line, "Variable already declared: " + decl->name);
            analyzeExpr(decl->value);
            return;
        }

        ValueType valueType = analyzeExpr(decl->value);
        SymbolType declaredType = symbolTypeFromKeyword(decl->typeKeyword);

        if (valueType != ValueType::UNKNOWN && valueTypeFromSymbolType(declaredType) != valueType)
        {
            reportError(stmt->line,
                        "Cannot initialize " + symbolTypeName(declaredType) + " variable '" + decl->name +
                            "' with a " + valueTypeName(valueType) + " value");
        }

        symbolTable.declare(decl->name, declaredType, stmt->line);
        return;
    }

    case StmtKind::ASSIGNMENT:
    {
        auto *assign = static_cast<AssignmentStmt *>(stmt.get());
        const SymbolInfo *info = symbolTable.lookup(assign->name);

        if (!info)
        {
            reportError(stmt->line, "Variable not declared before use: " + assign->name);
            analyzeExpr(assign->value);
            return;
        }

        ValueType valueType = analyzeExpr(assign->value);
        ValueType varType = valueTypeFromSymbolType(info->type);

        if (valueType != ValueType::UNKNOWN && valueType != varType)
        {
            reportError(stmt->line,
                        "Cannot assign " + valueTypeName(valueType) + " value to " +
                            valueTypeName(varType) + " variable '" + assign->name + "'");
        }

        return;
    }

    case StmtKind::SHOW:
    {
        auto *show = static_cast<ShowStmt *>(stmt.get());
        analyzeExpr(show->value);
        return;
    }

    case StmtKind::IF:
    {
        auto *ifStmt = static_cast<IfStmt *>(stmt.get());
        analyzeCondition(ifStmt->condition);
        analyzeBlock(ifStmt->thenBlock);
        analyzeBlock(ifStmt->elseBlock);
        return;
    }

    case StmtKind::WHILE:
    {
        auto *whileStmt = static_cast<WhileStmt *>(stmt.get());
        analyzeCondition(whileStmt->condition);
        analyzeBlock(whileStmt->body);
        return;
    }

    case StmtKind::BLOCK:
    {
        analyzeBlock(static_pointer_cast<BlockStmt>(stmt));
        return;
    }
    }
}

bool SemanticAnalyzer::analyze(Program &program)
{
    for (const StmtPtr &stmt : program.statements)
    {
        analyzeStmt(stmt);
    }

    return errors.empty();
}

const vector<string> &SemanticAnalyzer::getErrors() const
{
    return errors;
}

const SymbolTable &SemanticAnalyzer::getSymbolTable() const
{
    return symbolTable;
}
