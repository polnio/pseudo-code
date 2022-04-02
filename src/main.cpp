#include <iostream>
#include <fstream>
#include <regex>
#include <string>
// #include <Converter.hpp>
#include <PyConverter.hpp>
#include <constants.hpp>

using namespace std;


// map<string, string> variables;
// int noline = 0;
enum logicsEnum {OU, OUSYM, ET, ETSYM, EQEQ};

int main (int argc, char const *argv[]) {
    smatch sm;
    int opt = 0;
    string const outputFileName = getCmdOption(argv, argv+argc, "-o");
    short indentSize = stoi(getCmdOption(argv, argv+argc, "-indent"));
    ifstream inputFile; inputFile.open(argv[1]);
    // ofstream outputFile; outputFile.open(outputFileName);
    string line;
    string tempWrite = "";

    /* bool isWriting = false;
    int indent = 0;
    bool variablesDefining = false;
    bool started = false;
    bool finished = false; */
    PyConverter converter(outputFileName, indentSize);
    pair<bool, string> convertResult;
    if (inputFile.is_open()) {
        while (inputFile) {
            // noline++;
            indentNoline();
            // cout << getNoline() << " ";
            getline(inputFile, line);
            // cout << line << endl;
            if (line == "") continue;

            converter.init(line);
            if (converter.write(line, tempWrite)) continue;

            for (int i = 0; i < sizeof(validLines)/sizeof(validLines[0]); i++) {
                if (regex_search(line, sm, regex(validLines[i], regex_constants::icase))) {
                    convertResult = converter.convert(sm, i);
                    if (convertResult.first) tempWrite = convertResult.second;
                    break;
                }
            }
        }
        if (!converter.isVariablesDefining()) showError("No variable declaration found. Add VARIABLES statement at the top of the file");
        if (!converter.isStarted()) showError("No main block found. Please englobe your code with DEBUT and FIN statements");
    }

    // outputFile.close();
    inputFile.close();
    return 0;
}
