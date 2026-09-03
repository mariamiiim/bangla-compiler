#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

enum class SymbolType
{
    NUMBER,  // সংখ্যা
    STRING   // লেখা
};

struct SymbolInfo
{
    SymbolType type;
    int declaredLine;
};

class SymbolTable
{
private:
    std::unordered_map<std::string, SymbolInfo> symbols;

public:
    // Returns true if a variable with this name is already declared.
    bool exists(const std::string& name) const;

    // Declares a new variable. Returns false if it already existed
    // (in which case nothing is overwritten).
    bool declare(const std::string& name, SymbolType type, int line);

    // Returns a pointer to the symbol info, or nullptr if not found.
    const SymbolInfo* lookup(const std::string& name) const;
};

// Maps the source-level type keyword (সংখ্যা / লেখা) to a SymbolType.
SymbolType symbolTypeFromKeyword(const std::string& keyword);

// Human readable name for error messages / debugging.
std::string symbolTypeName(SymbolType type);

#endif
