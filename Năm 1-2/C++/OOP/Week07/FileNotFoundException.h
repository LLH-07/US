#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <fstream>

using namespace std;

class FileNotFoundException : public exception
{
private:
    string filename;

public:
    FileNotFoundException(string file) : filename(file){}

    const char *what() throw()
    {
        return strdup(("Exception: File '" + filename + "' is not exist!\n").c_str());
    }
};