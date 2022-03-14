#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

char* getCmdOption (const char** begin, const char** end, const string& option) {
    char **itr = find((char**)begin, (char**)end, option);
    return (itr != end && ++itr != end) ? *itr : 0;
}

bool cmdOptionExists (const char** begin, const char** end, const string& option) {
    return find(begin, end, option) != end;
}

void split_str (string const &str, const char delimiter, vector<string> &out) {
    stringstream s(str); string s2; while (getline(s, s2, delimiter)) out.push_back(s2);
}

bool checkVariable (map<string,string> variables, string var, int noline) {
    if (variables.count(var) == 0) {
        cerr << noline << ": " << var << " is not defined" << endl;
        return false;
    }
    return true;
}

bool checkReading (map<string,string> variables, string var, int noline) {
    if (!checkVariable(variables, var, noline)) return false;
    if (variables[var] != "Texte") {//! Can be modified
        cerr << noline << ": " << var << ": Can not convert " << variables[var] << " into Texte";
        return false;
    }
    return true;
}

int main (int argc, char const *argv[]) {
    int opt = 0;
    string const outputFileName = getCmdOption(argv, argv+argc, "-o");
    short indentSize = stoi(getCmdOption(argv, argv+argc, "-indent"));
    string validLines[] = {
        "AFFICHER *(.+)",
        "LIRE *(.+)",
        "^ *(?!POUR)([^ ]+) *<- *(.+)",
        "TANT ?QUE (.+[^ ]) *FAIRE",
        "FIN ?TANT ?QUE",
        "R[EÉ]P[EÉ]TER",
        "JUSQU'[AÀ] *(.+)",
        "POUR *([^ ]+) *<- *([^ ,]+) *, *([^ ]+) *FAIRE",
        "FIN ?POUR"
    };
    enum OutLines {Afficher, Lire, Arrow, WhileLoop, EndWhileLoop, DoWhileLoop, EndDoWhileLoop, ForLoop, EndForLoop};
    ifstream inputFile; inputFile.open(argv[1]);
    ofstream outputFile; outputFile.open(outputFileName);
    int noline = 0;
    string line;
    smatch sm;
    string tempWrite = "";

    bool isWriting = false;
    int indent = 0;
    bool variablesDefining = false;
    bool started = false;
    bool finished = false;
    map<string, string> variables;
    if (inputFile.is_open()) {
        while (inputFile) {
            noline++;
            getline(inputFile, line);
            if (line == "") continue;

            if (!started) {
                if (!variablesDefining) {
                    regex_search(line, sm, regex("VARIABLES?", regex_constants::icase));
                    if (sm.length() > 0) {
                        variablesDefining = true;
                    }
                } else {
                    regex_search(line, sm, regex("DEBUT", regex_constants::icase));
                    if (sm.length() > 0) {
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
                regex_search(line, sm, regex("LIRE *(.+)", regex_constants::icase));
                if (sm.length() > 0) {
                    if (!checkReading(variables, sm[1], noline)) exit(1);
                    outputFile << sm[1] << "=input(" << tempWrite << ")" << endl;
                    continue;
                } else {
                    outputFile << "print(" << tempWrite << ")" << endl;
                }
            }

            for (int i = 0; i < sizeof(validLines)/sizeof(validLines[0]); i++) {
                // cout << validLines[i] << endl;
                regex_search(line, sm, regex(validLines[i], regex_constants::icase));
                if (sm.length() > 0) {
                    switch (i) {
                    case Afficher:
                        tempWrite = sm[1];
                        isWriting = true;
                        break;
                    
                    case Lire:
                        if (!checkReading(variables, sm[1], noline)) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << sm[1] << "=input('')" << endl;
                        break;
                    
                    case Arrow:
                        if (!checkVariable(variables, sm[1], noline)) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << sm[1] << "=" << sm[2] << endl;
                        break;

                    case WhileLoop:
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << "while " << sm[1] << ":" << endl;
                        indent += 1;
                        break;

                    case DoWhileLoop:
                    case EndDoWhileLoop:
                        cerr << noline << ": DO WHILE loop are not implemented in python";
                        break;
                        
                    case ForLoop:
                        {
                            bool toIsNum = false;
                            try { stoi(sm[3]); toIsNum = true; } catch (invalid_argument e) {}
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
        if (!variablesDefining) cerr << "No variable declaration found. Add VARIABLES statement at the top of the file";
        if (!started) cerr << "No main block found. Please englobe your code with DEBUT and FIN statements";
    }

    outputFile.close();
    inputFile.close();
    return 0;
}
