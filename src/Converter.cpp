#include <Converter.hpp>
#include <string>
#include <fstream>
#include <regex>
#include <constants.hpp>

using namespace std;

Converter::Converter (string outputFileName, short _indentSize):
    variablesDefining(false),
    started(false),
    finished(false),
    isWriting(false),
    indent(0),
    indentSize(_indentSize),
    outputFile()
{
    outputFile.open(outputFileName);
}

bool Converter::isVariablesDefining () { return variablesDefining; }
bool Converter::isStarted () { return started; }

bool Converter::init (const string line) {}
bool Converter::write (const string line, const string tempWrite) {}
std::pair<bool, string> Converter::convert (const smatch sm, int type) {}
string Converter::replaceIntegratedVar (string var) { return "aa"; }
Converter::~Converter () {
    // cout << "close" << endl;
    outputFile.close();
}
pair<bool,string> Converter::checkVariable (string const text) {
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
    return {true, replaceIntegratedVar(originText)};
}
