#include <iostream>
#include <string>
#include <fstream>
#include <PyConverter.hpp>
#include <regex>
#include <constants.hpp>

using namespace std;

PyConverter::PyConverter (string outputFileName, short _indentSize):
    Converter(outputFileName, _indentSize) {}

pair<bool, string> PyConverter::convert(const smatch sm, int type) {
    pair<bool,string> newVar, newVar2;
    // const smatch sm;
    switch (type) {
        case Afficher:
            // cout << sm[1] << endl;
            newVar = checkVariable(sm[1]);
            if (!newVar.first) exit(1);
            isWriting = true;
            return {true, newVar.second };

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
    return { false, ""};
}

bool PyConverter::init (const string line) {
    smatch sm;
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
                    // variables.insert(pair<string,string>(sm[i], sm[sm.size()-1]));
                    insertVariable(sm[i], sm[sm.size()-1]);
                }
            }
        }
        return true;
    }
}

bool PyConverter::write (const string line, const string tempWrite) {
    smatch sm;
    if (isWriting) {
        isWriting = false;
        outputFile << string(indent*indentSize, ' ');
        if (regex_search(line, sm, regex(validLines[Lire], regex_constants::icase))) {
            // cout << tempWrite << endl;
            // cout << checkReading(sm[1], tempWrite) << endl;
            outputFile << checkReading(sm[1], tempWrite) << endl;
            // outputFile << sm[1] << "=input(" << tempWrite << ")" << endl;
            // continue;
            return true;
        } else {
            outputFile << "print(" << tempWrite << ")" << endl;
        }
    }
    return false;
}

PyConverter::~PyConverter () {}
