#pragma once
#include <exception>
#include <string>
#include <sstream>

using namespace std;

class DateFormatException : public exception
{
private:
    string date;

public:
    DateFormatException(){}

    const char* what() throw()
    {
        return "Exception: Date is not valid!\n";
    }
};

bool isLeapYear(int y)
{
    return (y % 4 == 0 && y % 100 == 0) || (y % 400 == 0);
}

bool isValidDay(int d, int m, int y)
{
    if(d < 1)
        return false;

    switch(m)
    {
        case 2:
        if(isLeapYear(y)) return d <= 29;
        return d <= 28;

        case 1 : case 3 : case 5: case 7: case 8: case 10 : case 12:
            return d <= 31;

        return d <= 30;
    }

    return false;
}

bool checkValidDate(string date)
{
    if(date[2] != '/' || date[5] != '/')
        return false;

    date[2] = date[5] = ' ';
    stringstream ss(date);
    int d, m, y;
    ss >> d >> m >> y;
    
    return isValidDay(d, m, y);
}
