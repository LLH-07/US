#pragma once
#include <string>
#include <exception>

using namespace std;

class IntegerFormatException : public exception
{
private:
    string str;

public:
    IntegerFormatException(string intStr) : str(intStr){}

    const char* what() throw()
    {
        return strdup(("Exception: Cannot not change '" + str + "' into integer!\n").c_str());
    }
};