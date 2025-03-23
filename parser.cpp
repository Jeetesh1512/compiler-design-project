#include <bits/stdc++.h>
using namespace std;

template <typename K, typename V>

void removeDuplicates(map<K, V> &inputMap)
{
    set<V> seenValues;

    for (auto it = inputMap.begin(); it != inputMap.end();)
    {
        if (seenValues.find(it->second) != seenValues.end())
        {
            it = inputMap.erase();
        }
        else
        {
            seenValues.insert(it->second);
            it++;
        }
    }
}

void readTerminals_and_nonTerminals(vector<string> &nonTerminals, vector<string> &terminals, set<string> &terminalSet, ifstream &file)
{
    string line;
    getline(file, line);
    stringstream ss(line);
    string symbol;

    if (line.find("%tokens") != string::npos)
    {
        while (ss >> symbol)
        {
            if (symbol != "%tokens")
            {
                terminals.push_back(symbol);
            }
        }
    }

    while (getline(file, line))
    {
        if (line == "%%")
            break;
    }

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream prodStream(line);
        prodStream >> symbol;

        if (symbol.back() == ':')
            symbol.pop_back();

        if (symbol == "%%")
            break;

        if (symbol == ";" || symbol == "|")
            continue;

        if (find(nonTerminals.begin(), nonTerminals.end(), symbol) == nonTerminals.end())
        {
            nonTerminals.push_back(symbol);
        }

        string arrow;
        prodStream >> arrow; // skip ':'

        while (prodStream >> symbol)
        {
            if (symbol == "\t;" || symbol == "|" || symbol.empty())
                continue;

            if (find(nonTerminals.begin(), nonTerminals.end(), symbol) == nonTerminals.end())
            {
                terminalSet.insert(symbol);
            }
        }
    }
}

// Helper to trim the whitespace from a string
string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Helper function to split the string into symbols
vector<string> splitSymbols(const string &body)
{
    vector<string> symbols;
    stringstream ss(body);
    string symbol;

    while (ss >> symbol)
    {
        symbols.push_back(symbol);
    }
    return symbols;
}

void read_productions(map<int, pair<string, vector<string>>> &mp, ifstream &file)
{
    file.seekg(0, ios::beg);
    vector<string> productions;
    bool insideGrammar = false;
    string line;

    while (getline(file, line))
    {
        line = trim(line);

        if (line == "%%")
        {
            if (insideGrammar)
                break;
            insideGrammar = true;
            continue;
        }
        if (insideGrammar && !line.empty() && line != ";")
        {
            productions.push_back(line);
        }
    }
    file.close();

    bool hasAugmentedStart = false;
    for (const string &prod : productions)
    {
        if (prod.find("S'") == 0)
        {
            hasAugmentedStart = true;
            break;
        }
    }
    if (!hasAugmentedStart)
    {
        productions.insert(productions.begin(), "S': S");
    }

    int count = 0;
    string head;

    for (const string &production : productions)
    {
        size_t pos = production.find(":");

        if (pos != string::npos)
        {
            head = trim(production.substr(0, pos));
            string body = trim(production.substr(pos + 1));

            vector<string> bodySymbols = splitSymbols(body);

            mp.insert({count, {head, bodySymbols}});
            count++;
        }
        if (production.find("|") != string::npos)
        {
            pos = production.find("|");
            string body = trim(production.substr(pos + 1));
            if (body.empty())
            {
                body = "epsilon";
            }

            vector<string> bodySymbols = splitSymbols(body);

            mp.insert({count, {head, bodySymbols}});
            count++;
        }
    }
}

void find_first(map<int, pair<string, vector<string>>> &productions, vector<string> &nonTerminals, vector<string> &terminals, map<string, set<string>> &first)
{
    for (auto it : terminals)
    {
        if (it != "epsilon")
        {
            first[it].insert(it); // As FIRST(terminal) = {terminal}
        }
    }

    for (auto &x : nonTerminals)
    {
        first[x] = {}; // initialized as an empty set
    }

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto &x : productions)
        {
            string head = x.second.first;
            vector<string> body = x.second.second;

            if (body.size() == 1 && body[0] == "epsilon")
            {
                if (first[head].insert("epsilon").second)
                {
                    changed = true;
                }
                continue;
            }

            bool epsilon_in_all = true;
            for (const string &symbol : body)
            {
                if (first.find(symbol) != first.end())
                {
                    for (const string &y : first[symbol])
                    {
                        if (y != "epsilon")
                        {
                            if (first[head].insert(y).second)
                                changed = true;
                        }
                    }
                    // if FIRST(symbol) does not contain epsilon, stop propagation
                    if (first[symbol].find("epsilon") == first[symbol].end())
                    {
                        epsilon_in_all = false;
                        break;
                    }
                }
                else
                {
                    // When not in FIRST map
                    cout << "Warning: Symbol '" << symbol << "' not found in the FIRST sets. \n";
                    epsilon_in_all = false;
                    break;
                }
            }

            // if all symbols in the body can derive epsilon, add epsilon to FIRST(head)
            if (epsilon_in_all)
            {
                if (first[head].insert("epsilon").second)
                {
                    changed = true;
                }
            }
        }
    }
}

set<string> getFirstOfSymbols(const vector<string> &symbols, map<string, set<string>> &first)
{
    set<string> result;
    bool canDeriveEpsilon = true;
    for (const auto &sym : symbols)
    {
        if (first.find(sym) == first.end())
        {
            // Terminal
            result.insert(sym);
            canDeriveEpsilon = false;
            break;
        }

        const auto &firstSet = first[sym];
        result.insert(firstSet.begin(), firstSet.end());
        result.erase("epsilon");
        if (firstSet.find("epsilon") == firstSet.end())
        {
            canDeriveEpsilon = false;
            break;
        }
    }
    if (canDeriveEpsilon)
    {
        result.insert("epsilon");
    }
    return result;
}

void find_follow(map<int, pair<string, vector<string>>> &productions, vector<string> &nonTerminals, vector<string> &terminals, map<string, set<string>> &first, map<string, set<string>> &follow)
{
    for (const string it : nonTerminals)
        follow[it] = {};

    follow[nonTerminals[0]].insert("$"); // for start symbol

    bool changed = true;
    while (changed)
    {
        changed = false;

        for (const auto &prod : productions)
        {
            string head = prod.second.first;
            vector<string> body = prod.second.second;

            for (size_t i = 0; i<body.size(); i++)
            {
                string B = body[i];

                // if B is a non-terminal
                if (find(nonTerminals.begin(), nonTerminals.end(), B) != nonTerminals.end())
                {
                    // if B is followed by another symbol, add FIRST(next)-epsilon to FOLLOW(B)
                    if (i + 1 < body.size())
                    {
                        vector<string> nextSymbols(body.begin() + i + 1, body.end());
                        set<string> firstOfNext = getFirstOfSymbols(nextSymbols, first);

                        for (const string &f : firstOfNext)
                        {
                            if (f != "epsilon" && follow[B].find(f) == follow[B].end())
                            {
                                follow[B].insert(f);
                                changed = true;
                            }
                        }

                        // if FIRST(next) contains epsilon, add FOLLOW(head) to FOLLOW(B)
                        if (firstOfNext.find("epsilon") != firstOfNext.end())
                        {
                            for (const string &f : follow[head])
                            {
                                if (follow[B].find(f) == follow[B].end())
                                {
                                    follow[B].insert(f);
                                    changed = true;
                                }
                            }
                        }
                    }
                    // B is at the end of production
                    else
                    {
                        for (const string &f : follow[head])
                        {
                            if (follow[B].find(f) == follow[B].end())
                            {
                                follow[B].insert(f);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

using Item = tuple<int, string, vector<string>, int, string>;

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cout << "To run: " << argv[0] << " <grammar file>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open())
    {
        cout << "Error in opening file" << endl;
        return 1;
    }

    string line;
    vector<string> nonTerminals, terminals;
    set<string> terminalSet;
    map<string, set<string>> first;
    map<string, set<string>> follow;
    map<int, vector<Item>> states;
    readTerminals_and_nonTerminals(nonTerminals, terminals, terminalSet, file);
    map<int, pair<string, vector<string>>> productions;

    ofstream outFile("itemsets.txt");

    if (!outFile)
    {
        cout << "Error opening the file!!" << endl;
        return 1;
    }

    outFile << "---------NON-TERMINALS-----------\n";
    for (auto it : nonTerminals)
        outFile << it << "\n";
    outFile << "---------------------------------\n\n\n";

    outFile << "---------------------------------\n";
    outFile << "Start Symbol: " << nonTerminals[0] << endl;
    outFile << "---------------------------------\n";

    outFile << "\n-----------TERMINALS-------------\n";
    for (auto it : terminals)
        outFile << it << "\n";
    outFile << "---------------------------------\n\n\n";

    outFile << "-----------------------------------Productions-----------------------------------------\n\n";
    read_productions(productions, file);
    for (auto it : productions)
    {
        outFile << "|" << it.first << "|  " << it.second.first << " -> ";
        for (const auto &symbol : it.second.second)
        {
            outFile << symbol << " ";
        }
        outFile << "\n";
    }
    outFile << "--------------------------------------------------------------------------------\n\n\n";

    find_first(productions, nonTerminals, terminals, first);
    outFile << "\n-----------------------FIRST-------------------------\n";
    for (auto &x : first)
    {
        outFile << x.first << ": {";
        for (auto &y : x.second)
        {
            outFile << y << ", ";
        }
        outFile << "}\n";
    }
    outFile << "-----------------------------------------------------\n\n";

    find_follow(productions,nonTerminals,terminals,first,follow);
    outFile << "\n---------------------------------------FOLLOW-----------------------------------------\n";
    for (auto &x : follow)
    {
        outFile << x.first << " : {";
        for (auto &y : x.second)
        {
            outFile << y << ", ";
        }
        outFile << "}\n";
    }
    outFile << "---------------------------------------------------------------------------------------\n\n";
    return 0;
}