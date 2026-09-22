#pragma once
#include "Employee.h"

using namespace std;

class Officer : public Employee
{
private:
    int workDays;
    int subsidy;

public:
    Officer(string HoTen, short namVaoLam, long long luongCoBan, string ma, short d, short m, short y) : Employee(HoTen, namVaoLam, luongCoBan, ma, d, m, y){}

    void insertInformation();
    void getInformation();
    long long calculateSalary();
};

void Officer::insertInformation()
{
    cout << "The number of work-days: ";
    cin >> workDays;
    cout << "Subsidy: ";
    cin >> subsidy;
}

void Officer::getInformation()
{
    cout << "The number of work-days: " << workDays << endl;
    cout << "Subsidy: " << subsidy << endl;
}

long long Officer::calculateSalary()
{
    return salary + workDays * 100000 + subsidy;
}