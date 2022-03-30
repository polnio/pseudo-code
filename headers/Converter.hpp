#ifndef CONVERTER_HPP_INCLUDED
#define CONVERTER_HPP_INCLUDED

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <regex>

class Converter {
    protected:
        bool variablesDefining;
        bool started;
        bool finished;
        bool isWriting;
        int indent;
        short indentSize;
        // std::map<std::string, std::string> variables;
        std::ofstream outputFile;
    public:
        Converter (std::string outputFileName, short _indentSize);
        virtual ~Converter ();
        bool isVariablesDefining ();
        bool isStarted ();
        virtual bool init (const std::string line);
        virtual bool write (const std::string line, const std::string tempWrite);
        virtual std::pair<bool, std::string> convert (const std::smatch sm, int type);
};


#endif // CONVERTER_HPP_INCLUDED