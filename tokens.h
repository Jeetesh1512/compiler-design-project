#ifndef TOKENS_H
#define TOKENS_H

#include<bits/stdc++.h>
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
    DECC,   //"--"
    END_OF_INPUT //"$" To represent end of input
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
    {TokenType::DECC, "DECC"},
    {TokenType::END_OF_INPUT,"$"}
};

extern vector<tuple<TokenType, string, int, int, int>> tokens;
#endif