#include "symboltable.h"

using namespace std;

bool SymbolTable::exists(const string &name) const
{
    return symbols.find(name) != symbols.end();
}

bool SymbolTable::declare(const string &name, SymbolType type, int line)
{
    if (exists(name))
    {
        return false;
    }

    symbols[name] = {type, line};
    return true;
}

const SymbolInfo *SymbolTable::lookup(const string &name) const
{
    auto found = symbols.find(name);

    if (found == symbols.end())
    {
        return nullptr;
    }

    return &found->second;
}

SymbolType symbolTypeFromKeyword(const string &keyword)
{
    if (keyword == "সংখ্যা")
    {
        return SymbolType::NUMBER;
    }

    return SymbolType::STRING;
}

string symbolTypeName(SymbolType type)
{
    switch (type)
    {
    case SymbolType::NUMBER:
        return "সংখ্যা";
    case SymbolType::STRING:
        return "লেখা";
    }

    return "unknown";
}
