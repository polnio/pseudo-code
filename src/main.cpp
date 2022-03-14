#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

string const validLines[] = {
    "AFFICHER +(.+)",
    "LIRE +(.+)",
    "^ *(?!POUR)([^ ]+) *<- *(.+)",
    "TANT ?QUE +(.+[^ ]) +FAIRE",
    "FIN ?TANT ?QUE",
    "R[EÉ]P[EÉ]TER",
    "JUSQU'[AÀ] +(.+)",
    "POUR *([^ ]+) *<- *([^ ,]+) *, *([^ ]+) +FAIRE",
    "FIN ?POUR"
};
map<string, string> variables;
int noline = 0;

void showError (string const error) {
    cerr << "Error l." << noline << ": " << error << endl;
}

char* getCmdOption (const char** begin, const char** end, const string& option) {
    char **itr = find((char**)begin, (char**)end, option);
    return (itr != end && ++itr != end) ? *itr : 0;
}

bool cmdOptionExists (const char** begin, const char** end, const string& option) {
    return find(begin, end, option) != end;
}

string trimString(string str, string chars) {
    str.erase(0, str.find_first_not_of(chars));
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

void split_str (string const &str, const char delimiter, vector<string> &out) {
    stringstream s(str); string s2; while (getline(s, s2, delimiter)) out.push_back(s2);
}

bool checkVariableExist (string var) {
    if (variables.count(var) == 0) {
        showError(var+" is not defined");
        return false;
    }
    return true;
}

bool testVariableText (string const text) {
    // cout << r << endl;
    smatch sm;
    constexpr const char* delimiters = "+-=><*/";
    char* tokenValue = strtok((char*)text.c_str(), delimiters);
    while (tokenValue != NULL) {
        string textVar = tokenValue;
        tokenValue = strtok(NULL, delimiters);
        if (regex_search(textVar, sm, regex("^ *\"([^\"]*)\" *$"))) {

        } else {
            string const var = trimString(textVar, " ");
            if (regex_match(var, regex("^[0-9]+$"))) continue;
            if (!checkVariableExist(var)) return false;
        }
    }
    return true;
}

bool checkReading (string var) {
    if (!checkVariableExist(var)) return false;
    if (variables[var] != "Texte") {//! Can be modified
        showError(var+": Can not convert "+variables[var]+" into Texte");
        return false;
    }
    return true;
}

int main (int argc, char const *argv[]) {
    int opt = 0;
    string const outputFileName = getCmdOption(argv, argv+argc, "-o");
    short indentSize = stoi(getCmdOption(argv, argv+argc, "-indent"));
    enum OutLines {Afficher, Lire, Arrow, WhileLoop, EndWhileLoop, DoWhileLoop, EndDoWhileLoop, ForLoop, EndForLoop};
    ifstream inputFile; inputFile.open(argv[1]);
    ofstream outputFile; outputFile.open(outputFileName);
    string line;
    smatch sm;
    string tempWrite = "";

    bool isWriting = false;
    int indent = 0;
    bool variablesDefining = false;
    bool started = false;
    bool finished = false;
    if (inputFile.is_open()) {
        while (inputFile) {
            noline++;
            getline(inputFile, line);
            if (line == "") continue;

            if (!started) {
                if (!variablesDefining) {
                    if (regex_search(line, sm, regex("VARIABLES?", regex_constants::icase))) {
                        variablesDefining = true;
                    }
                } else {
                    if (regex_search(line, sm, regex("DEBUT", regex_constants::icase))) {
                        started = true;
                    } else {
                        regex_search(line, sm, regex("([^, ]+)(?:, +([^,]+))*: *(Entier|Texte)", regex_constants::icase));
                        for (int i = 1; i < sm.size() - 1; i++) {
                            // cout << sm[i] << " : " << sm[sm.size() - 1] << endl;
                            variables.insert(pair<string,string>(sm[i], sm[sm.size()-1]));
                        }
                    }
                }
                continue;
            }


            if (isWriting) {
                isWriting = false;
                outputFile << string(indent*indentSize, ' ');
                if (regex_search(line, sm, regex("LIRE *(.+)", regex_constants::icase))) {
                    if (!checkReading(sm[1])) exit(1);
                    outputFile << sm[1] << "=input(" << tempWrite << ")" << endl;
                    continue;
                } else {
                    outputFile << "print(" << tempWrite << ")" << endl;
                }
            }

            for (int i = 0; i < sizeof(validLines)/sizeof(validLines[0]); i++) {
                // cout << validLines[i] << endl;
                if ( regex_search(line, sm, regex(validLines[i], regex_constants::icase))) {
                    switch (i) {
                    case Afficher:
                        if (!testVariableText(sm[1])) exit(1);
                        tempWrite = sm[1];
                        isWriting = true;
                        break;
                    
                    case Lire:
                        if (!checkReading(sm[1])) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << sm[1] << "=input('')" << endl;
                        break;
                    
                    case Arrow:
                        if (!checkVariableExist(sm[1])) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << sm[1] << "=" << sm[2] << endl;
                        break;

                    case WhileLoop:
                        if (!testVariableText(sm[1])) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << "while " << sm[1] << ":" << endl;
                        indent += 1;
                        break;

                    case DoWhileLoop:
                    case EndDoWhileLoop:
                        showError(": DO WHILE loop are not implemented in python");
                        break;
                        
                    case ForLoop:
                        {
                            if (!testVariableText(sm[1])) exit(1);
                            if (!testVariableText(sm[2])) exit(1);
                            bool toIsNum = false; try { stoi(sm[3]); toIsNum = true; } catch (invalid_argument e) {}
                            outputFile << string(indent*indentSize, ' ');
                            outputFile << "for " << sm[1] << " in range(" << sm[2] << "," << (toIsNum ? to_string(stoi(sm[3])+1) : string(sm[3])+"+1") << "):" << endl;
                            indent += 1;
                        }
                        break;

                    case EndWhileLoop:
                    case EndForLoop:
                        indent -= 1;
                        break;

                    default:
                        break;
                    }
                    break;
                }
            }
        }
        if (!variablesDefining) showError("No variable declaration found. Add VARIABLES statement at the top of the file");
        if (!started) showError("No main block found. Please englobe your code with DEBUT and FIN statements");
    }

    outputFile.close();
    inputFile.close();
    return 0;
}
