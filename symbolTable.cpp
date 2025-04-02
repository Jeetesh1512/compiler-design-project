#include <bits/stdc++.h>
using namespace std;

struct Symbol
{
    string tokenType;
    string name;
    string value;
    bool isUsed;
    int line;
    int pos;
    int scope;
    vector<pair<int,int>> references;
};

class SymbolTable
{
    unordered_map<int, unordered_map<string, Symbol>> table;
    stack<int> scopeStack;
    int currScope;

public:
    SymbolTable()
    {
        currScope = 0;
        scopeStack.push(currScope);
    }

    int getCurrentScope()
    {
        return currScope;
    }

    void enterScope()
    {
        currScope++;
        scopeStack.push(currScope);
    }

    void exitScope()
    {
        if (!scopeStack.empty())
        {
            table.erase(currScope);
            scopeStack.pop();
            currScope = scopeStack.top();
        }
    }

    void insert(string tokenType, string name, string value, int line, int pos)
    {
        table[currScope][name] = {
            tokenType,
            name,
            value,
            false,
            line,
            pos,
            currScope};
    }

    void markUsed(string name,int line,int pos)
    {
        for (auto it = scopeStack.top(); it >= 0; it--)
        {
            if (table[it].find(name) != table[it].end())
            {
                table[it][name].isUsed = true;
                table[it][name].references.push_back({line, pos});
                return;
            }
        }
    }

    bool exists(string name)
    {
        for (auto it = scopeStack.top(); it >= 0; it--)
        {
            if (table[it].find(name) != table[it].end())
                return true;
        }
        return false;
    }

    string getValue(string name)
    {
        for (auto it = scopeStack.top(); it >= 0; it--)
        {
            if (table[it].find(name) != table[it].end())
                return table[it][name].value;
        }
        return "";
    }

    void printTable()
{
    cout << "+-----------+--------+-------+------+------+\n";
    cout << "| TokenType | Name   | Value | Line | Scope |\n";
    cout << "+-----------+--------+-------+------+------+\n";
    for (auto &scopeEntry : table)
    {
        for (auto &varEntry : scopeEntry.second)
        {
            auto &sym = varEntry.second;
            cout << "| " << sym.tokenType << " | " << sym.name << " | " << sym.value 
                 << " | " << sym.line << " | " << sym.scope << " |\n";
            
            if (!sym.references.empty())
            {
                cout << "  Used at: ";
                for (auto &ref : sym.references)
                {
                    cout << "(Line " << ref.first << ", Pos " << ref.second << ") ";
                }
                cout << endl;
            }
        }
    }
    cout << "+-----------+--------+-------+------+------+\n";
}

};
