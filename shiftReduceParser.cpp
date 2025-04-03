#include <bits/stdc++.h>
using namespace std;

class Parser {
    vector<string> tokens;
    stack<int> stateStack; 
    stack<string> parseStack;  
    map<int, pair<string, vector<string>>> productions;
    map<pair<int, string>, string> parsingTable;
    int tokenIndex;
    ofstream outputFile;  

public:
    Parser(vector<string> tokens, map<int, pair<string, vector<string>>> productions, 
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
            string currentToken = tokens[tokenIndex];

            if (parsingTable.count({currentState, currentToken})) {
                string action = parsingTable[{currentState, currentToken}];

                if (action[0] == 's') {  
                    int nextState = stoi(action.substr(1));

                    outputFile << "Shift: " << currentToken << endl;

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
                    outputFile << endl;
                } 
                else if (action == "Accept") {  
                    outputFile << "Parsing successful!" << endl;
                    return;
                }
            } 
            else {
                outputFile << "Parsing failed!" << endl;
                return;
            }
        }
    }
};
