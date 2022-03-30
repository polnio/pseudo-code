#include <Converter.hpp>
#include <string>
#include <fstream>
#include <regex>

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
Converter::~Converter () {
    // cout << "close" << endl;
    outputFile.close();
}
