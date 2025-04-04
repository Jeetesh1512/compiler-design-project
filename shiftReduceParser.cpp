#include <bits/stdc++.h>
#include "tokens.h"
#include "symbolTable.cpp"
using namespace std;

class Parser
{
    //<TokenType,value,line,pos,scope level>
    vector<tuple<TokenType, string, int, int, int>> tokens;
    stack<int> stateStack;
    stack<string> parseStack;
    map<int, pair<string, vector<string>>> productions;
    map<pair<int, string>, string> parsingTable;
    int tokenIndex;
    ofstream outputFile;
    SymbolTable symbolTable;

public:
    Parser(vector<tuple<TokenType, string, int, int, int>> tokens, map<int, pair<string, vector<string>>> productions,
           map<pair<int, string>, string> parsingTable, const string &outputFileName)
    {
        this->tokens = tokens;
        this->productions = productions;
        this->parsingTable = parsingTable;
        tokenIndex = 0;
        stateStack.push(0);
        outputFile.open(outputFileName);
        if (!outputFile.is_open())
        {
            cerr << "Failed to open output file!" << endl;
            exit(1);
        }
    }

    ~Parser()
    {
        if (outputFile.is_open())
        {
            outputFile.close();
        }
    }

    void parse()
    {
        while (tokenIndex < tokens.size())
        {
            int currentState = stateStack.top();
            TokenType tokenType = get<0>(tokens[tokenIndex]);
            string tokenValue = get<1>(tokens[tokenIndex]);
            int line = get<2>(tokens[tokenIndex]);
            int pos = get<3>(tokens[tokenIndex]);
            int scope = get<4>(tokens[tokenIndex]);

            string currentToken = tokenTypeNames[get<0>(tokens[tokenIndex])];

            outputFile << "\nCurrent State: " << currentState << ", Current Token: " << currentToken << endl;
            if (parsingTable.count({currentState, currentToken}))
            {
                string action = parsingTable[{currentState, currentToken}];

                outputFile << "Action: " << action << endl;
                if (action[0] == 's')
                {
                    int nextState = stoi(action.substr(1));

                    outputFile << "Shift: " << currentToken << "\n\n"
                               << endl;

                    parseStack.push(currentToken);
                    stateStack.push(nextState);

                    handleToken(tokenType, tokenValue, line, pos, scope);

                    tokenIndex++;
                }
                else if (action[0] == 'r')
                {
                    int productionIndex = stoi(action.substr(1));
                    auto [lhs, rhs] = productions[productionIndex];

                    for (size_t i = 0; i < rhs.size(); i++)
                    {
                        parseStack.pop();
                        stateStack.pop();
                    }

                    parseStack.push(lhs);
                    int gotoState = stoi(parsingTable[{stateStack.top(), lhs}]);
                    stateStack.push(gotoState);

                    outputFile << "Reduce: " << lhs << " → ";
                    for (const string &s : rhs)
                        outputFile << s << " ";
                    outputFile << "\nGoto State: " << gotoState << "\n\n"
                               << endl;
                }
                else if (action == "Accept")
                {
                    outputFile << "\n\nParsing successful!" << endl;
                    ofstream outFile("symbol_table.txt");
                    if (outFile.is_open())
                    {
                        symbolTable.printTable(outFile);
                        cout << "Symbol Table written to file 'symbol_table.txt'" << endl;
                        outFile.close();
                    }
                    else
                    {
                        cerr << "Error: Unable to open file.\n";
                    }
                    return;
                }
            }
            else
            {
                cerr << "Parsing failed at state " << currentState << " with token " << currentToken << endl;
                outputFile << "Parsing failed!" << endl;
                return;
            }
        }
    }

    void handleToken(TokenType type, string value, int line, int pos, int scope)
    {
        if (type == TokenType::LBRACE)
        {
            symbolTable.enterScope();
        }
        else if (type == TokenType::RBRACE)
        {
            symbolTable.exitScope();
        }
        else if (type == TokenType::INT || type == TokenType::FLOAT)
        {
            if (tokenIndex + 1 < tokens.size() && get<0>(tokens[tokenIndex + 1]) == TokenType::ID)
            {
                string varName = get<1>(tokens[tokenIndex + 1]);
                symbolTable.insert(tokenTypeNames[type], varName, "", line, scope);
            }
        }
        else if (type == TokenType::ID)
        {
            if (!symbolTable.exists(value))
            {
                cerr<< "Error: Undeclared variable '" << value << "' at line " << line << endl;
                outputFile << "Error: Undeclared variable '" << value << "' at line " << line << endl;
            }
            else
            {
                symbolTable.markUsed(value, line, pos);
            }
        }
        else if (type == TokenType::EQ)
        {
            if (tokenIndex - 1 >= 0 && get<0>(tokens[tokenIndex - 1]) == TokenType::ID)
            {
                string lhsVar = get<1>(tokens[tokenIndex - 1]);

                if (tokenIndex + 1 < tokens.size())
                {
                    TokenType rhsType = get<0>(tokens[tokenIndex + 1]);
                    string rhsVal = get<1>(tokens[tokenIndex + 1]);

                    if (rhsType == TokenType::NUM || rhsType == TokenType::DEC)
                    {
                        symbolTable.updateValue(lhsVar, rhsVal, line, pos);
                    }
                    else if (rhsType == TokenType::ID)
                    {
                        if (!symbolTable.exists(rhsVal))
                        {
                            cerr<< "\n\nError: Undeclared variable '" << rhsVal << "' at line " << line <<"\n\n"<< endl;
                            outputFile << "Error: Undeclared variable '" << rhsVal << "' at line " << line << endl;
                            return;
                        }

                        string lhsType = symbolTable.getType(lhsVar);
                        string rhsTypeStr = symbolTable.getType(rhsVal);

                        if (lhsType != rhsTypeStr)
                        {
                            cerr << "\n\nError: Type mismatch between '" << lhsVar << "' (" << lhsType
                                 << ") and '" << rhsVal << "' (" << rhsTypeStr << ") at line " << line <<"\n\n"<< endl;
                            outputFile << "Error: Type mismatch between '" << lhsVar << "' (" << lhsType
                                       << ") and '" << rhsVal << "' (" << rhsTypeStr << ") at line " << line << endl;
                        }

                        symbolTable.updateValue(lhsVar, "UNKNOWN", line, pos);
                        symbolTable.markUsed(rhsVal, line, pos);
                    }
                }
            }
        }

        else if (type == TokenType::INC || type == TokenType::DECC)
        {
            if (tokenIndex - 1 >= 0 && get<0>(tokens[tokenIndex - 1]) == TokenType::ID)
            {
                string varName = get<1>(tokens[tokenIndex - 1]);
                if (!symbolTable.exists(varName))
                {
                    cerr << "Error: Undefined variable '" << varName
                         << "' used in increment/decrement at line " << line << endl;
                    return;
                }
                symbolTable.markUsed(varName, line, pos);
            }
        }
    }
};
