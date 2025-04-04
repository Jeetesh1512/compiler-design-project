#include <bits/stdc++.h>
#include <regex>
#include "tokens.h"
using namespace std;

//<TokenType,value,line,pos,scope level>
vector<tuple<TokenType, string, int, int, int>> tokens;

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

    while (!remainingCode.empty())
    {
        bool matched = false;

        // Ignore single-line comments (//...)
        if (remainingCode.substr(0, 2) == "//")
        {
            size_t newLinePos = remainingCode.find('\n');
            if (newLinePos != string::npos)
            {
                remainingCode = remainingCode.substr(newLinePos + 1); 
                lineNo++;
                pos = 0;
            }
            else
                break; 
            continue;
        }

        // Ignore multi-line comments (/* ... */)
        if (remainingCode.substr(0, 2) == "/*")
        {
            size_t endCommentPos = remainingCode.find("*/");
            if (endCommentPos != string::npos)
            {
                size_t newLineCount = count(remainingCode.begin(), remainingCode.begin() + endCommentPos, '\n');
                lineNo += newLineCount;
                pos = 0;
                remainingCode = remainingCode.substr(endCommentPos + 2); 
            }
            else
            {
                cout << "Lexical Error: Unterminated comment at line " << lineNo << endl;
                return;
            }
            continue;
        }

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
                }
                else if (tokenType == TokenType::RBRACE)
                {
                    scopeNo--;
                }

                tokens.push_back({tokenType, tokenValue, lineNo, pos, scopeNo});

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

void lexer(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Cannot open file " << filename << endl;
        exit(1);
    }

    string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    tokenise(code);

    ofstream outFile("tokens.txt");

    if (!outFile)
    {
        cout << "Error opening file for writing!" << endl;
        exit(1);
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
}