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
    vector<tuple<int, int, int>> references;
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
            scopeStack.pop();
            if (!scopeStack.empty())
                currScope = scopeStack.top();
            else
                currScope = 0;
        }
    }

    void insert(string tokenType, string name, string value, int line, int pos)
    {
        size_t semicolonPos = value.find(';');
        if (semicolonPos != string::npos)
            value = value.substr(0, semicolonPos);

        if (table[currScope].find(name) != table[currScope].end())
        {
            table[currScope][name].value = value;
            table[currScope][name].line = line;
            table[currScope][name].pos = pos;
        }
        else
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
    }

    void markUsed(string name, int line, int pos)
    {
        stack<int> tempStack = scopeStack;
        while (!tempStack.empty())
        {
            int scopeLevel = tempStack.top();
            tempStack.pop();
            if (table[scopeLevel].find(name) != table[scopeLevel].end())
            {
                table[scopeLevel][name].isUsed = true;
                table[scopeLevel][name].references.push_back({line, pos, currScope});
                return;
            }
        }
    }

    string getType(const string &varName)
    {
        stack<int> tempStack = scopeStack; 
    
        while (!tempStack.empty())
        {
            int scopeLevel = tempStack.top();
            tempStack.pop();
    
            if (table[scopeLevel].find(varName) != table[scopeLevel].end())
            {
                return table[scopeLevel][varName].tokenType; 
            }
        }
        return "UNKNOWN"; 
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
        cout << "+-----------+--------+-------+------+-------+\n";
        cout << "| TokenType | Name   | Value | Line | Scope |\n";
        cout << "+-----------+--------+-------+------+-------+\n";

        for (auto &scopeEntry : table)
        {
            for (auto &varEntry : scopeEntry.second)
            {
                auto &sym = varEntry.second;

                cout << "| " << setw(9) << left << sym.tokenType
                     << " | " << setw(6) << left << sym.name
                     << " | " << setw(5) << left << sym.value
                     << " | " << setw(4) << sym.line
                     << " | " << setw(4) << sym.scope << " |\n";
                cout << "+-----------+--------+-------+------+-------+\n";

                if (!sym.references.empty())
                {
                    cout << "|  Referenced at:                           |\n";
                    for (auto &ref : sym.references)
                    {
                        int refLine, refPos, refScope;
                        tie(refLine, refPos, refScope) = ref;
                        cout << "|    → Line " << setw(3) << refLine
                             << ", Pos " << setw(3) << refPos
                             << ", Scope " << refScope << "           |\n";
                    }
                    cout << "+-------------------------------------------+\n";
                }
            }
        }
    }
};
