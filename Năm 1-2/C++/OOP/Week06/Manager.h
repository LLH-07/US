#pragma once
#include "Employee.h"

using namespace std;

class Manager : public Employee
{
private:
    short position;
    long long bonus;

public:
    Manager(string HoTen, short namVaoLam, long long luongCoBan, string ma, short d, short m, short y) : Employee(HoTen, namVaoLam, luongCoBan, ma, d, m, y){}

    void insertInformation();
    void getInformation();
    long long calculateSalary();
};

void Manager::insertInformation()
{
    cout << "Position: ";
    cin >> position;
    cout << "Bonus: ";
    cin >> bonus;
}

void Manager::getInformation()
{
    cout << "Position: " << position << endl;
    cout << "Bonus: " << bonus << endl;
}

long long Manager::calculateSalary()
{
    return salary * position + bonus;
}