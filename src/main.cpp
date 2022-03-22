#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

string const validLines[] = {
    "AFFICHER +(.+)",
    "LIRE +(.+)",
    "^ *(?!POUR)([^ ]+) *<- *(.+)",
    "SI +(.+[^ ]) +ALORS",
    "FIN ?SI",
    "TANT ?QUE +(.+[^ ]) +FAIRE",
    "FIN ?TANT ?QUE",
    "R[EÉ]P[EÉ]TER",
    "JUSQU'[AÀ] +(.+)",
    "POUR *([^ ]+) *<- *([^ ,]+) *, *([^ ]+) +FAIRE",
    "FIN ?POUR"
};
enum OutLines {Afficher, Lire, Arrow, IfStatement, EndIf, WhileLoop, EndWhileLoop, DoWhileLoop, EndDoWhileLoop, ForLoop, EndForLoop};
const string regValidTypes = "Bool(?:ean)?|Texte?|Entier|R[ée]el";
map<string, string> variables;
string integratedVars[] = {"true", "false"};
int noline = 0;
smatch sm;
const string regLogic = "OU|\\|\\||ET|&&|==";
const string logics[] = {"OU","||","ET","&&","=="};
enum logicsEnum {OU, OUSYM, ET, ETSYM, EQEQ};


void showError (string const error) {
    cerr << "Error l." << noline << ": " << error << endl;
    exit(1);
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

bool replace (string& str, const string& from, const string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == string::npos) return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void split_str (string const &str, const char delimiter, vector<string> &out) {
    stringstream s(str); string s2; while (getline(s, s2, delimiter)) out.push_back(s2);
}

bool checkVariableExist (string var) {
    for (size_t i = 0; i < sizeof(integratedVars)/sizeof(integratedVars[0]); i++){
        string tempVar = var;
        transform(tempVar.begin(), tempVar.end(), tempVar.begin(), ::tolower);
        if (tempVar == integratedVars[i]) return true;
    }
    
    if (variables.count(var) == 0) {
        showError(var+" is not defined");
        return false;
    }
    return true;
}

string replaceLogic (const string logic) {
    string upperLogic = logic;
    transform(upperLogic.begin(), upperLogic.end(), upperLogic.begin(), ::toupper);
    if (upperLogic == "OU" || upperLogic == "||") return "or";
    if (upperLogic == "ET" || upperLogic == "&&") return "and";
    if (upperLogic == "==") return "==";
    
}

pair<bool,string> checkVariable (string const text) {
    smatch _sm;
    const string originText = text;
    constexpr const char* delimiters = "+-><*/";
    char* tokenValue = strtok((char*)text.c_str(), delimiters);
    while (tokenValue != NULL) {
        string textVar = tokenValue;
        tokenValue = strtok(NULL, delimiters);
        const string var = trimString(textVar, " ");
        if (regex_search(textVar, _sm, regex("^ *\"([^\"]*)\" *$"))) {
            continue;
        } else {
            // cout << var << endl;
            if (regex_match(var, regex("^[0-9]+$"))) continue;
            if (regex_search(var, _sm, regex("^(.*[^ ]) *("+regLogic+") *([^ ].*)$", regex_constants::icase))) {
                if (!checkVariable(_sm[3]).first) return {false, ""};
                string tempVar = _sm[1];
                // cout << _sm[2] << " devient " << replaceLogic(_sm[2]) << endl;
                // cout << "=> " << _sm[1].str()+" "+replaceLogic(_sm[2])+" "+_sm[3].str() << endl;
                return {
                    checkVariable(_sm[1]).first,
                    checkVariable(_sm[1]).second+" "+replaceLogic(_sm[2])+" "+checkVariable(_sm[3]).second
                };
            }
            if (!checkVariableExist(var)) return {false, ""};
        }
    }
    return {true, originText};
}

string checkReading (const string var, const string msg) {
    if (!checkVariableExist(var)) return "";
    if (variables[var] == "Texte") return var+"=input("+msg+")";
    if (variables[var] == "Entier") return var+"=int(input("+msg+"))";
    if (variables[var] == "Réel") return var+"=float(input("+msg+"))";
    showError(var+": Can not convert "+variables[var]+" into Texte");
    return "";
}

int main (int argc, char const *argv[]) {
    int opt = 0;
    string const outputFileName = getCmdOption(argv, argv+argc, "-o");
    short indentSize = stoi(getCmdOption(argv, argv+argc, "-indent"));
    ifstream inputFile; inputFile.open(argv[1]);
    ofstream outputFile; outputFile.open(outputFileName);
    string line;
    string tempWrite = "";

    bool isWriting = false;
    int indent = 0;
    bool variablesDefining = false;
    bool started = false;
    bool finished = false;
    pair<bool,string> newVar, newVar2;
    if (inputFile.is_open()) {
        while (inputFile) {
            noline++;
            getline(inputFile, line);
            if (line == "") continue;

            if (!started) {
                if (!variablesDefining) {
                    if (regex_match(line, regex("VARIABLES?", regex_constants::icase))) {
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
                if (regex_search(line, sm, regex(validLines[Lire], regex_constants::icase))) {
                    // cout << tempWrite << endl;
                    outputFile << checkReading(sm[1], tempWrite) << endl;
                    // outputFile << sm[1] << "=input(" << tempWrite << ")" << endl;
                    continue;
                } else {
                    outputFile << "print(" << tempWrite << ")" << endl;
                }
            }

            for (int i = 0; i < sizeof(validLines)/sizeof(validLines[0]); i++) {
                // cout << validLines[i] << endl;
                if (regex_search(line, sm, regex(validLines[i], regex_constants::icase))) {
                    // cout << i << endl;
                    switch (i) {
                    case Afficher:
                        // cout << sm[1] << endl;
                        newVar = checkVariable(sm[1]);
                        if (!newVar.first) exit(1);
                        tempWrite = newVar.second;
                        isWriting = true;
                        break;
                    
                    case Lire:
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << checkReading(sm[1], "''") << endl;
                        // outputFile << sm[1] << "=input('')" << endl;
                        break;
                    
                    case Arrow:
                        if (!checkVariableExist(sm[1])) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << sm[1] << "=" << sm[2] << endl;
                        break;

                    case IfStatement:
                        // cout << "1" << endl;
                        newVar = checkVariable(sm[1]);
                        if (!newVar.first) exit(1);
                        // cout << "2" << endl;
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << "if " << newVar.second << ":" << endl;
                        indent += 1;
                        break;

                    case WhileLoop:
                        newVar = checkVariable(sm[1]);
                        if (!newVar.first) exit(1);
                        outputFile << string(indent*indentSize, ' ');
                        outputFile << "while " << newVar.second << ":" << endl;
                        indent += 1;
                        break;

                    case DoWhileLoop:
                    case EndDoWhileLoop:
                        showError(": DO WHILE loop are not implemented in python");
                        break;
                        
                    case ForLoop:
                        {
                            newVar = checkVariable(sm[1]);
                            if (!newVar.first) exit(1);
                            newVar2 = checkVariable(sm[2]);
                            if (!newVar2.first) exit(1);
                            bool toIsNum = false; try { stoi(sm[3]); toIsNum = true; } catch (invalid_argument e) {}
                            outputFile << string(indent*indentSize, ' ');
                            outputFile << "for " << newVar.second << " in range(" << newVar2.second << "," << (toIsNum ? to_string(stoi(sm[3])+1) : string(sm[3])+"+1") << "):" << endl;
                            indent += 1;
                        }
                        break;

                    case EndIf:
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
