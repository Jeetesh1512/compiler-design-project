#include "lexer.cpp"
#include "itemSetCreator.cpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string inputfile = argv[1];
    string grammarfile = "grammar.txt";

    lexer(inputfile);
    
    createAllItemSets(grammarfile);
}