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
        smatch sm;
        /* string tempVar = var;
        transform(tempVar.begin(), tempVar.end(), tempVar.begin(), ::tolower);
        if (tempVar == integratedVars[i]) return true; */
        if (regex_search(var, sm, regex(integratedVars[i], regex_constants::icase))) {
            if (i == 2) return checkVariableExist(sm[1]);
            return true;
        }
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