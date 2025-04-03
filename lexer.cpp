#include <bits/stdc++.h>
#include <regex>
#include "symbolTable.cpp"
using namespace std;

enum class TokenType
{
    ID,     // variable names
    NUM,    // numbers (integers)
    MAIN,   // main
    WHILE,  // WHILE
    DEC,    // decimal numbers (floats)
    INT,    //"int" keyword
    FLOAT,  //"float" keyword
    EQ,     //"=" keyword
    LPAREN, //"("
    RPAREN, //")"
    LBRACE, //"{"
    RBRACE, //"}"
    SEMI,   //";"
    COMMA,  //","
    LT,     //"<"
    GT,     //">"
    PLUS,   //"+"
    MINUS,  //"-"
    MULT,   //"*"
    DIV,    //"/"
    MOD,    //"%"
    INC,    //"++"
    DECC    //"--"
};

unordered_map<TokenType, string> tokenTypeNames = {
    {TokenType::ID, "ID"},
    {TokenType::NUM, "NUM"},
    {TokenType::MAIN, "MAIN"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::DEC, "DEC"},
    {TokenType::INT, "INT"},
    {TokenType::FLOAT, "FLOAT"},
    {TokenType::EQ, "EQ"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::SEMI, "SEMI"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::LT, "LT"},
    {TokenType::GT, "GT"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::MULT, "MULT"},
    {TokenType::DIV, "DIV"},
    {TokenType::MOD, "MOD"},
    {TokenType::INC, "INC"},
    {TokenType::DECC, "DECC"}};

//<TokenType,value,line,pos,scope level>
vector<tuple<TokenType, string, int, int, int>> tokens;
SymbolTable symTable;

void tokenise(const string &code)
{
    vector<pair<TokenType, regex>> tokenPatterns = {
        {TokenType::INT, regex("int")},
        {TokenType::FLOAT, regex("float")},
        {TokenType::MAIN, regex("main")},
        {TokenType::WHILE, regex("while")},
        {TokenType::EQ, regex("=")},
        {TokenType::LT, regex("<")},
        {TokenType::GT, regex(">")},
        {TokenType::INC, regex("\\+\\+")},
        {TokenType::DECC, regex("\\-\\-")},
        {TokenType::PLUS, regex("\\+")},
        {TokenType::MINUS, regex("\\-")},
        {TokenType::MULT, regex("\\*")},
        {TokenType::DIV, regex("/")},
        {TokenType::MOD, regex("%")},
        {TokenType::LPAREN, regex("\\(")},
        {TokenType::RPAREN, regex("\\)")},
        {TokenType::LBRACE, regex("\\{")},
        {TokenType::RBRACE, regex("\\}")},
        {TokenType::SEMI, regex(";")},
        {TokenType::COMMA, regex(",")},
        {TokenType::ID, regex("[a-zA-Z_][a-zA-Z0-9_]*")},
        {TokenType::DEC, regex("\\d+\\.\\d*")},
        {TokenType::NUM, regex("\\d+")},
    };

    int lineNo = 1;
    int pos = 0;
    int scopeNo = 0;
    string remainingCode = code;
    string lastType = "";

    while (!remainingCode.empty())
    {
        bool matched = false;

        for (const auto &pattern : tokenPatterns)
        {
            smatch match;
            if (regex_search(remainingCode, match, pattern.second, regex_constants::match_continuous))
            {
                string tokenValue = match.str();
                TokenType tokenType = pattern.first;

                if (tokenType == TokenType::LBRACE)
                {
                    scopeNo++;
                    symTable.enterScope();
                }
                else if (tokenType == TokenType::RBRACE)
                {
                    scopeNo--;
                    symTable.exitScope();
                }
                else if (tokenType == TokenType::ID)
                {
                    if (!lastType.empty() && (lastType == "int" || lastType == "float"))
                    {
                        symTable.insert(lastType, tokenValue, "", lineNo, pos);
                    }
                    else if (symTable.exists(tokenValue))
                    {
                        symTable.markUsed(tokenValue, lineNo, pos);
                    }
                    else
                    {
                        cout << "Error: Undeclared variable " << tokenValue << " at line " << lineNo << endl;
                    }
                }

                if (tokenType == TokenType::EQ)
                {
                    if (!tokens.empty() && get<0>(tokens.back()) == TokenType::ID)
                    {
                        string varName = get<1>(tokens.back());
                        string value = match.suffix().str();
                        string varType = symTable.getType(varName);
                        symTable.insert(varType, varName, value, lineNo, pos);
                    }
                }

                tokens.push_back({tokenType, tokenValue, lineNo, pos, scopeNo});
                lastType = (tokenType == TokenType::INT || tokenType == TokenType::FLOAT) ? tokenValue : "";

                size_t newLineCount = count(tokenValue.begin(), tokenValue.end(), '\n');

                if (newLineCount > 0)
                {
                    lineNo += newLineCount;
                    pos = 0;
                }
                else
                {
                    pos += tokenValue.length();
                }

                remainingCode = match.suffix().str();

                matched = true;
                break;
            }
        }
        if (!matched)
        {
            char currentChar = remainingCode[0];
            if (!isspace(currentChar))
            {
                cout << "Lexical Error: Invalid Charcter " << currentChar << " at Line " << lineNo << " at position " << pos << endl;
                return;
            }

            if (currentChar == '\n')
            {
                lineNo++;
                pos = 0;
            }
            else
                pos++;

            remainingCode.erase(0, 1);
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        cout << "To run: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open())
    {
        cout << "Cannot open file " << argv[1] << endl;
        return 1;
    }

    string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    tokenise(code);
    symTable.printTable();

    ofstream outFile("tokens.txt");

    if (!outFile)
    {
        cout << "Error opening file for writing!" << endl;
        return 1;
    }

    outFile << "+-----------+----------------+-----------+-----------+----------------+" << endl;
    outFile << "| Token     | Value          | Line      | Pos       | Scope          |" << endl;
    outFile << "+-----------+----------------+-----------+-----------+----------------+" << endl;

    for (const auto &token : tokens)
    {
        outFile << "| " << left << setw(9) << tokenTypeNames[get<0>(token)] << " | "
                << setw(14) << get<1>(token) << " | "
                << setw(9) << to_string(get<2>(token)) << " | "
                << setw(9) << get<3>(token) << " | " << setw(14) << get<4>(token) << " |" << endl;
    }

    outFile << "+-----------+----------------+-----------+-----------+----------------+" << endl;

    outFile.close();
    cout << "Tokens written to tokens.txt" << endl;

    return 0;
}