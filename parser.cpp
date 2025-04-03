#include <bits/stdc++.h>
#include "tokens.h"
using namespace std;

template <typename K, typename V>
void removeDuplicates(map<K, V> &inputMap)
{
    set<V> seenValues;

    for (auto it = inputMap.begin(); it != inputMap.end();)
    {
        if (seenValues.find(it->second) != seenValues.end())
        {
            it = inputMap.erase(it);
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

            for (size_t i = 0; i < body.size(); i++)
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

// Helper function to compute FIRST of a sequence of symbols
set<string> getFirstOfSequence(const vector<string> &sequence, const map<string, set<string>> &first)
{
    set<string> result;
    bool canDeriveEpsilon = true;

    for (const string &symbol : sequence)
    {
        if (first.find(symbol) == first.end())
        { // Terminal
            result.insert(symbol);
            canDeriveEpsilon = false;
            break;
        }
        const set<string> &firstSet = first.at(symbol);
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

vector<Item> closure(vector<Item> items, map<int, pair<string, vector<string>>> &productions,
                     map<string, set<string>> &first, vector<string> &nonTerminals)
{
    bool changed;
    do
    {
        changed = false;
        vector<Item> newItems = items;
        for (const auto &item : items)
        {
            int prod_no = get<0>(item);
            string head = get<1>(item);
            vector<string> body = get<2>(item);
            int dot_pos = get<3>(item);
            string lookahead = get<4>(item);

            if (dot_pos >= body.size())
                continue;

            string next_symbol = body[dot_pos];
            if (find(nonTerminals.begin(), nonTerminals.end(), next_symbol) != nonTerminals.end())
            {
                vector<string> remaining(body.begin() + dot_pos + 1, body.end());
                set<string> firstSet = getFirstOfSequence(remaining, first);
                if (firstSet.erase("epsilon"))
                {
                    firstSet.insert(lookahead);
                }
                if (firstSet.empty())
                {
                    firstSet.insert(lookahead);
                }

                for (const auto &prod : productions)
                {
                    if (prod.second.first == next_symbol)
                    {
                        for (const string &la : firstSet)
                        {
                            Item newItem = make_tuple(prod.first,
                                                      prod.second.first,
                                                      prod.second.second,
                                                      0,
                                                      la);
                            if (find(newItems.begin(), newItems.end(), newItem) == newItems.end())
                            {
                                newItems.push_back(newItem);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        items = newItems;
    } while (changed);
    return items;
}

vector<Item> goTo(const vector<Item> &items, const string &symbol,
                  map<int, pair<string, vector<string>>> &productions,
                  map<string, set<string>> &first, vector<string> &nonTerminals)
{
    vector<Item> nextItems;
    for (const auto &item : items)
    {
        int prod_no = get<0>(item);
        string head = get<1>(item);
        vector<string> body = get<2>(item);
        int dot_pos = get<3>(item);
        string lookahead = get<4>(item);

        if (dot_pos < body.size() && body[dot_pos] == symbol)
        {
            Item newItem = make_tuple(prod_no, head, body, dot_pos + 1, lookahead);
            nextItems.push_back(newItem);
        }
    }
    return closure(nextItems, productions, first, nonTerminals);
}

void createItemSet(map<int, pair<string, vector<string>>> &productions, vector<string> &nonTerminals, vector<string> &terminals, map<string, set<string>> &first, map<int, vector<Item>> &states)
{
    if (productions.find(0) == productions.end())
    {
        cout << "Error: Augmented production (S' -> S) was not found" << endl;
        return;
    }

    vector<Item> items;
    items.emplace_back(0, productions[0].first, productions[0].second, 0, "$");

    items = closure(items, productions, first, nonTerminals);

    states[0] = items;
    int stateCount = 1;
    bool changed = true;

    while (changed)
    {
        changed = false;

        map<int, vector<Item>> newStates = states;

        for (const auto &[stateId, stateItems] : states)
        {
            set<string> symbols;
            for (const auto &item : stateItems)
            {
                const vector<string> &body = get<2>(item);
                int dot_pos = get<3>(item);
                if (dot_pos < body.size())
                {
                    symbols.insert(body[dot_pos]);
                }
            }

            for (const string &sym : symbols)
            {
                vector<Item> newState = goTo(stateItems, sym, productions, first, nonTerminals);

                if (!newState.empty())
                {
                    bool stateExists = false;
                    for (const auto &[id, existingState] : newStates)
                    {
                        if (existingState == newState)
                        {
                            stateExists = true;
                            break;
                        }
                    }

                    if (!stateExists)
                    {
                        newStates[stateCount] = newState;
                        stateCount++;
                        changed = true;
                    }
                }
            }
        }
        states = newStates;
    }
}

void constructParsingTable(const map<int, vector<Item>> &states, map<int, pair<string, vector<string>>> &productions, vector<string> &nonTerminals, vector<string> &terminals, set<string> &terminalSet, map<string, set<string>> &first, map<string, set<string>> &follow)
{
    map<pair<int, string>, string> parsingTable;

    for (const auto &[stateId, items] : states)
    {
        for (const auto &item : items)
        {
            int prod_no = get<0>(item);
            string head = get<1>(item);
            vector<string> body = get<2>(item);
            int dot_pos = get<3>(item);
            string lookahead = get<4>(item);

            if (dot_pos == body.size()) // Reduction or Accept
            {
                if (head == productions[0].first && lookahead == "$")
                {
                    parsingTable[{stateId, "$"}] = "Accept";
                }
                else
                {
                    parsingTable[{stateId, lookahead}] = "r" + to_string(prod_no);
                }
            }
            else // Shift or Goto
            {
                string nextSymbol = body[dot_pos];
                vector<Item> gotoItems = goTo(items, nextSymbol, productions, first, nonTerminals);

                if (!gotoItems.empty())
                {
                    for (const auto &[nextStateId, nextState] : states)
                    {
                        if (nextState == gotoItems)
                        {
                            if (find(nonTerminals.begin(), nonTerminals.end(), nextSymbol) != nonTerminals.end())
                            {
                                parsingTable[{stateId, nextSymbol}] = to_string(nextStateId);
                            }
                            else
                            {
                                parsingTable[{stateId, nextSymbol}] = "s" + to_string(nextStateId);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    ofstream outFile("parsingTable.txt");
    if (!outFile)
    {
        cout << "Error Opening the file!!!" << endl;
        return;
    }

    // Ensure `$` is in terminals
    if (find(terminals.begin(), terminals.end(), "$") == terminals.end())
        terminals.push_back("$");

    int colWidth = 12;  // Adjusted for better alignment
    int totalColumns = terminals.size() + nonTerminals.size();

    // Header
    outFile << "\n---------------------------- LR(1) Parsing Table ----------------------------\n";
    outFile << setw(5) << "State" << " |";

    for (const string &term : terminals)
        if (term != "epsilon") outFile << setw(colWidth) << term << " |";
    
    for (const string &nt : nonTerminals)
        if (nt != productions[0].first) outFile << setw(colWidth) << nt << " |";

    outFile << "\n" << string((colWidth + 2) * (totalColumns + 1), '-') << "\n";

    // Table Rows
    for (const auto &[stateId, items] : states)
    {
        outFile << setw(5) << stateId << " |";

        for (const string &term : terminals)
        {
            if (term == "epsilon") continue;
            string action = parsingTable[{stateId, term}];
            outFile << setw(colWidth) << (action.empty() ? " " : action) << " |";
        }
        
        for (const string &nt : nonTerminals)
        {
            if (nt != productions[0].first)
            {
                string gotoAction = parsingTable[{stateId, nt}];
                outFile << setw(colWidth) << (gotoAction.empty() ? " " : gotoAction) << " |";
            }
        }

        outFile << "\n" << string((colWidth + 2) * (totalColumns + 1), '-') << "\n";
    }

    outFile.close();
}


void printItem(const Item &item, ofstream &outFile)
{
    int prod_no = get<0>(item);
    string head = get<1>(item);
    vector<string> body = get<2>(item);
    int dot_pos = get<3>(item);
    string lookahead = get<4>(item);

    // Print production number and head
    outFile << prod_no << ". " << head << " -> ";

    // Print body with dot
    for (size_t i = 0; i < body.size(); i++)
    {
        if (i == dot_pos)
        {
            outFile << ".";
        }
        outFile << body[i] << " ";
    }

    // If dot is at the end
    if (dot_pos == body.size())
    {
        outFile << ".";
    }

    // Print lookahead
    outFile << ", " << lookahead << endl;
}

// Function to print all states
void printStates(const map<int, vector<Item>> &states, ofstream &outFile)
{

    for (const auto &[stateId, items] : states)
    {
        outFile << "State " << stateId << ":" << endl;
        for (const auto &item : items)
        {
            printItem(item, outFile);
        }
        outFile << "-----------------------------" << endl;
    }
    outFile.close();
}

stack<int> stateStack;
stack<string> symbolStack;

void parser(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error in opening file " <<filename <<endl;
        exit(1);
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
        exit(1);
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

    find_follow(productions, nonTerminals, terminals, first, follow);
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

    createItemSet(productions, nonTerminals, terminals, first, states);
    removeDuplicates(states);
    outFile << "\n-----------------------ITEM-SETS-------------------------\n";
    printStates(states, outFile);
    outFile << "-----------------------------------------------------\n\n";

    constructParsingTable(states,productions,nonTerminals,terminals,terminalSet,first,follow);

    cout<<"First and follow sets written to 'itemsets.txt'"<<endl;
    cout<<"Parsing Table written to 'parsingtable.txt'"<<endl;

    for(auto it:parserTokens)
        cout<<it<<endl;
}