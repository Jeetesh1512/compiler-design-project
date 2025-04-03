#include "lexer.cpp"
#include "parser.cpp"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Usage: " << argv[0] << " <input_file> <grammar_file>" << endl;
        return 1;
    }

    string inputfile = argv[1];
    string grammarfile = argv[2];

    lexer(inputfile);
    
    parser(grammarfile);
}