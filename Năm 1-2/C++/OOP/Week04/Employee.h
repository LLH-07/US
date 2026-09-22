#pragma once
#include <string>
#include <iostream>

using namespace std;

class Employee
{
protected:
    string fullname = "";
    int startYear = 2023;
    int salary = 0;

public:
    // Constructor
    Employee(){};
    Employee(string FULLNAME) : fullname(FULLNAME){};
    Employee(int start, string FULLNAME) : fullname(FULLNAME), startYear(start){};
    Employee(string FULLNAME, int SALARY) : fullname(FULLNAME), salary(SALARY){};
    Employee(string FULLNAME, int start, int SALARY) : fullname(FULLNAME), startYear(start), salary(SALARY){};

    //
    void insertEmployee();
    void getEmployee();
    int calculateSalary();
    string getName();
};

void Employee::insertEmployee()
{
    cout << "NEW EMPLOYEE\n";
    cout << "Fullname: "; getline(cin, fullname);
    cout << "Starting year: "; cin >> startYear;
    cout << "Base salary: "; cin >> salary;
    cin.ignore();
    //cout << endl;
}

void Employee::getEmployee()
{
    cout << "EMPLOYEE BASIC INFORMATION\n";
    cout << "Fullname: " << fullname << endl;
    cout << "Starting year: " << startYear << endl;
    cout << "Base salary: " << salary << endl;
    //cout << endl;
}

int Employee::calculateSalary()
{
    return salary;
}

string Employee::getName()
{
    return fullname;
}