#include <constants.hpp>
#include <iostream>
#include <regex>
#include <vector>
#include <map>

using namespace std;
int noline = 0;
std::map<std::string, std::string> variables;

/* const string validLines[] = {
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
const string integratedVars[] = {"true", "false"};
const string regLogic = "OU|\\|\\||ET|&&|==";
const string logics[] = {"OU","||","ET","&&","=="};
const string regValidTypes = "Bool(?:ean)?|Texte?|Entier|R[ée]el"; */

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
bool checkVariableExist (const string var/* , const map<string, string> &variables */) {
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
void insertVariable (const string var, const string type) {
    variables.insert(pair<string,string>(var, type));
}
void indentNoline () { ++noline; }
int getNoline () { return noline; }