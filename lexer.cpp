#include<bits/stdc++.h>
#include<regex>
using namespace std;

set<char> validSymbols = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '+', '-', '*', '/', '%',
    '<', '>', '=',
    ';', ',', '(', ')', '{', '}',
    ' ', '\t', '\n',
    '_', '.'
};

enum class TokenType{
    ID,         //variable names
    NUM,        //numbers (integers)
    DEC,        //decimal numbers (floats)
    INT,        //"int" keyword
    FLOAT,      //"float" keyword
    EQ,         //"=" keyword
    LPAREN,     //"("
    RPAREN,     //")"
    LBRACE,     //"{"
    RBRACE,     //"}"
    SEMI,       //";"
    COMMA,      //","
    LT,         //"<"
    GT,         //">"
    PLUS,       //"+"
    MINUS,      //"-"
    MULT,       //"*"
    DIV,        //"/"
    MOD,        //"%"
    INC,        //"++"
    DEC        //"--"
};

unordered_map<TokenType,string> tokenTypeNames = {
    {TokenType::ID,"ID"},
    {TokenType::NUM,"NUM"},
    {TokenType::DEC,"DEC"},
    {TokenType::INT,"INT"},
    {TokenType::FLOAT,"FLOAT"},
    {TokenType::EQ,"EQ"},
    {TokenType::LPAREN,"LPAREN"},
    {TokenType::RPAREN,"RPAREN"},
    {TokenType::LBRACE,"LBRACE"},
    {TokenType::RBRACE,"RBRACE"},
    {TokenType::SEMI,"SEMI"},
    {TokenType::COMMA,"COMMA"},
    {TokenType::LT,"LT"},
    {TokenType::GT,"GT"},
    {TokenType::PLUS,"PLUS"},
    {TokenType::MINUS,"MINUS"},
    {TokenType::MULT,"MULT"},
    {TokenType::DIV,"DIV"},
    {TokenType::MOD,"MOD"},
    {TokenType::INC,"INC"},
    {TokenType::DEC,"DEC"}
};

//<TokenType,value,line,pos,scope level>
vector<tuple<TokenType,string,int,int,int>> tokens;

void tokenise(const string &code)
{
    vector<pair<TokenType,regex>> tokenPatterns = {
        {TokenType::ID,regex("[a-zA-Z_][a-zA-Z0-9_]*")},
        {TokenType::NUM,regex("\\d+")},
        {TokenType::DEC,regex("\\d+\\.\\d*")},
        {TokenType::INT,regex("int")},
        {TokenType::FLOAT,regex("float")},
        {TokenType::EQ,regex("=")},
        {TokenType::LPAREN,regex("(")},
        {TokenType::RPAREN,regex(")")},
        {TokenType::LBRACE,regex("{")},
        {TokenType::RBRACE,regex("}")},
        {TokenType::SEMI,regex(";")},
        {TokenType::COMMA,regex(",")},
        {TokenType::LT,regex("<")},
        {TokenType::GT,regex(">")},
        {TokenType::PLUS,regex("\\+")},
        {TokenType::MINUS,regex("\\-")},
        {TokenType::MULT,regex("\\*")},
        {TokenType::DIV,regex("/")},
        {TokenType::MOD,regex("%")},
        {TokenType::INC,regex("\\+\\+")},
        {TokenType::DEC,regex("\\-\\-")}
    };
}

int main(){
   
return 0;
}