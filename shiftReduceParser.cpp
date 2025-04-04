#include <bits/stdc++.h>
#include "tokens.h"
using namespace std;

class Parser {
    vector<tuple<TokenType, string, int, int, int>> tokens;
    stack<int> stateStack; 
    stack<string> parseStack;  
    map<int, pair<string, vector<string>>> productions;
    map<pair<int, string>, string> parsingTable;
    int tokenIndex;
    ofstream outputFile;  

public:
    Parser(vector<tuple<TokenType, string, int, int, int>> tokens, map<int, pair<string, vector<string>>> productions, 
           map<pair<int, string>, string> parsingTable, const string& outputFileName) 
    {
        this->tokens = tokens;
        this->productions = productions;
        this->parsingTable = parsingTable;
        tokenIndex = 0;
        stateStack.push(0);  
        outputFile.open(outputFileName);  
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file!" << endl;
            exit(1);  
        }
    }

    ~Parser() {
        if (outputFile.is_open()) {
            outputFile.close(); 
        }
    }

    void parse() {
        while (tokenIndex < tokens.size()) {
            int currentState = stateStack.top();
            string currentToken = tokenTypeNames[get<0>(tokens[tokenIndex])];

            outputFile << "\nCurrent State: " << currentState << ", Current Token: " << currentToken << endl; 
            if (parsingTable.count({currentState, currentToken})) {
                string action = parsingTable[{currentState, currentToken}];

                outputFile << "Action: " << action << endl;
                if (action[0] == 's') {  
                    int nextState = stoi(action.substr(1));

                    outputFile << "Shift: " << currentToken << "\n\n"<<endl;

                    parseStack.push(currentToken);
                    stateStack.push(nextState);
                    tokenIndex++;  
                } 
                else if (action[0] == 'r') {  
                    int productionIndex = stoi(action.substr(1));
                    auto [lhs, rhs] = productions[productionIndex];

                    for (size_t i = 0; i < rhs.size(); i++) {
                        parseStack.pop();
                        stateStack.pop();
                    }

                    parseStack.push(lhs);  
                    int gotoState = stoi(parsingTable[{stateStack.top(), lhs}]);
                    stateStack.push(gotoState);

                    outputFile << "Reduce: " << lhs << " → ";
                    for (const string& s : rhs) outputFile << s << " ";
                    outputFile << "\nGoto State: " << gotoState << "\n\n" << endl;
                } 
                else if (action == "Accept") {  
                    outputFile << "\n\nParsing successful!" << endl;
                    return;
                }
            } 
            else {
                cerr << "Parsing failed at state " << currentState << " with token " << currentToken << endl;
                outputFile << "Parsing failed!" << endl;
                return;
            }
        }
    }
};
