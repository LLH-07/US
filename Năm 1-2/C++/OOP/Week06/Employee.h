#pragma once
#include <string>
#include <iostream>
#include "CongTy.h"
#include "DoB.h"

using namespace std;

class Employee : public DoB
{
protected:
    string fullname;
    short enterYear;
    long long salary;
    string id;

public:
    Employee() : fullname(""), enterYear(0), salary(0) {}
    Employee(string HoTen, short namVaoLam, long long luongCoBan, string ma, short d, short m, short y) : fullname(HoTen), enterYear(namVaoLam), salary(luongCoBan), id(ma), DoB(d, m, y)  {}

    virtual ~Employee() {}

    virtual void insertInformation(){};
    virtual void getInformation(){};
    virtual long long calculateSalary(){};
    string getName();
    short getEnterYear();
    string getID();
};

string Employee::getName()
{
    return fullname;
}

short Employee::getEnterYear()
{
    return enterYear;
}

string Employee::getID()
{
    return id;
}