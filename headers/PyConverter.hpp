#ifndef PY_HPP_INCLUDED
#define PY_HPP_INCLUDED

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <Converter.hpp>

class PyConverter: public Converter {
    private:
        //
    public:
        PyConverter (std::string outputFileName, short _indentSize);
        ~PyConverter ();
        bool init (const std::string line);
        bool write (const std::string line, const std::string tempWrite);
        std::pair<bool, std::string> convert (const std::smatch sm, int type);
        std::string replaceIntegratedVar (const std::string var);
};

#endif