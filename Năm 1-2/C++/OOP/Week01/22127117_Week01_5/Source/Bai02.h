#pragma once
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Date
{
private:
    int day, month, year;

public:
    Date()
    {
        day = month = year = 1;
    }

    Date(int day, int month, int year)
    {
        this->day = day;
        this->month = month;
        this->year = year;
    }


    ~Date(){}

    int getDay();
    int getMonth();
    int getYear();
    void inputDate();
    void getDate();
    Date increase1Day();
    Date increaseNDay(int n);
    Date decrease1Day();
    Date decreaseNDay(int n);
    int compare(Date d2);
};

int Date::getDay()
{
    return day;
}

int Date::getMonth()
{
    return month;
}
int Date::getYear()
{
    return year;
}

void Date::inputDate()
{
    cout << "Day: ";
    cin >> day;

    cout << "Month: ";
    cin >> month;

    cout << "Year: ";
    cin >> year;

}

void Date::getDate()
{
    if(day < 10) cout << 0;
    cout << day << "/";

    if(month < 10) cout << 0;
    cout << month << "/";


    stringstream ss;
    ss << year;
    string YEAR = "";
    ss >> YEAR;

    while(YEAR.size() < 4)
        YEAR = '0' + YEAR;

    cout << YEAR;

    cout << endl;

}

bool isLeapYear(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

int getMonthDay(int m, int y)
{
    switch(m)
    {
    case 2:
        if(isLeapYear(y)) return 29;
        return 28;

    case 1 : case 3 : case 5 : case 7: case 8 : case 10 : case 12:
        return 31;

    }
    return 30;
}

void makeValidDate(int &d, int &m, int &y)
{

    if(d > 0)
    {
        while(d > getMonthDay(m, y) && d > 0)
        {
            d -= getMonthDay(m, y);
            m++;
            if(m > 12)
            {
                m = 1;
                y++;
            }
        }
    }

    else if(d <= 0)
    {
        while(d <= 0)
        {
            --m;
            if(m > 12)
            {
                m = 1;
                y++;
            }
            d += getMonthDay(m, y);
        }
    }
}

Date Date::increase1Day()
{
    int d, m, y;
    d = day + 1;
    m = month;
    y = year;
    makeValidDate(d, m, y);
    return Date(d, m, y);
}

Date Date::increaseNDay(int n)
{
    int d, m, y;
    d = day + n;
    m = month;
    y = year;
    makeValidDate(d, m, y);
    return Date(d, m, y);
}

Date Date::decrease1Day()
{
    int d, m, y;
    d = day - 1;
    m = month;
    y = year;
    makeValidDate(d, m, y);
    return Date(d, m, y);
}

Date Date::decreaseNDay(int n)
{
    int d, m, y;
    d = day - n;
    m = month;
    y = year;
    makeValidDate(d, m, y);
    return Date(d, m, y);
}

int Date::compare(Date d2)
{
    int day2 = d2.getDay();
    int month2 = d2.getMonth();
    int year2 = d2.getYear();

    if(year < year2) return -1;
    else if(year > year2) return 1;

    if(month < month2) return -1;
    else if(month > month2) return 1;

    if(day < day2) return -1;
    else if(day > day2) return 1;

    return 0;
}
