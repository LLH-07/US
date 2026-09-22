#pragma once
#include <string>
#include <exception>
#include <sstream>

using namespace std;

class TimeFormatException : public exception
{
private:
    string time;

public:
    const char* what() throw()
    {
        return "Exception: Time is not valid!\n";
    }
};

bool isValidTime(string time)
{
    if(time[2] != '/' || time[5] != '/')
        return false;

    time[2] = time[5] = ' ';
    stringstream ss(time);
    int h, m, s;
    ss >> h >> m >> s;

    if(h < 0 || h > 23) return false;
    if(m < 0 || m > 59) return false;
    if(s < 0 || s > 59) return false;

    return true;
    

}
