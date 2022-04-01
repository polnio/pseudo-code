#ifndef CONSTANTS_HPP_INCLUDED
#define CONSTANTS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <map>
#include <vector>

enum OutLines { Afficher, Lire, Arrow, IfStatement, EndIf, WhileLoop, EndWhileLoop, DoWhileLoop, EndDoWhileLoop, ForLoop, VerboseForLoop, EndForLoop };
const std::string validLines[] = {
    "AFFICHER +(.+)",
    "LIRE +(.+)",
    "^ *(?!POUR)([^ ]+) *<- *(.+)",
    "SI +(.+[^ ]) +ALORS",
    "FIN *SI",
    "TANT *QUE +(.+[^ ]) +FAIRE",
    "FIN *TANT *QUE",
    "R[EÉ]P[EÉ]TER",
    "JUSQU'[AÀ] +(.+)",
    "POUR +([^ ]+) *<- *([^ ,]+) *, *([^ ]+) +FAIRE",
    "POUR +([^ ]+) +ALLANT *DE +([^ ]+) +[AÀ] +([^ ]+) +FAIRE",
    "FIN *POUR"
};
const std::string integratedVars[] = {"vrai", "faux", "longueur *de *([^ ].*)"};
enum IntegratedVars { TRUE, FALSE, LENGTH_OF };
const std::string regLogic = "OU|\\|\\||ET|&&|==";
const std::string logics[] = {"OU", "||", "ET", "&&", "=="};
const std::string regValidTypes = "Bool(?:ean)?|Texte?|Entier|R[ée]el";

void showError(const std::string error);
char *getCmdOption(const char **begin, const char **end, const std::string &option);
bool cmdOptionExists(const char **begin, const char **end, const std::string &option);
std::string trimString(std::string str, std::string chars);
bool replace(std::string &str, const std::string &from, const std::string &to);
void split_str(std::string const &str, const char delimiter, std::vector<std::string> &out);
bool checkVariableExist(const std::string var /* , const std::map<std::string, std::string> &variables */);
std::string replaceLogic(const std::string logic);
// std::pair<bool, std::string> checkVariable(std::string const text);
std::string checkReading(const std::string var, const std::string msg);
void insertVariable(const std::string var, const std::string type);
void indentNoline();
int getNoline();
#endif // CONSTANTS_HPP_INCLUDED