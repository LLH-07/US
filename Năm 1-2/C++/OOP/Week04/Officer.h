#pragma once
#include "Employee.h"

class Officer : public Employee
{
private:
    int workDays = 0;
    int subsidy = 0;

public:
    // Constructor
    Officer() : Employee(){};
    Officer(string FULLNAME) : Employee(FULLNAME){};
    Officer(string FULLNAME, int SALARY, int WORK_DAY) : Employee(FULLNAME, SALARY), workDays(WORK_DAY){};
    Officer(int SALARY, string FULLNAME, int SUBSIDY) : Employee(FULLNAME, SALARY), subsidy(SUBSIDY){};
    Officer(int WORK_DAY, int SUBSIDY, string FULLNAME) : Employee(FULLNAME), workDays(WORK_DAY), subsidy(SUBSIDY){};
    Officer(string FULLNAME, int start, int SALARY, int WORK_DAY, int SUBSIDY) : Employee(FULLNAME, start, SALARY), workDays(WORK_DAY), subsidy(SUBSIDY){};

    //
    void insertOfficer();
    void getOfficer();
    int calculateSalary();
};

void Officer::insertOfficer()
{
    insertEmployee();
    cout << "NEW OFFICER\n";
    cout << "The number of work day: "; cin >> workDays;
    cout << "Subsidy: "; cin >> subsidy;
    cin.ignore();

    cout << endl;
}

void Officer::getOfficer()
{
    getEmployee();
    cout << "OFFICER INFORMATION\n";
    cout << "The number of work day: " << workDays << endl;
    cout << "Subsidy: " << subsidy << endl;
    cout << "Salary: " << calculateSalary() << endl;
    cout << endl;

}

int Officer::calculateSalary()
{
    salary = salary + workDays * 100000 + subsidy;
    return salary;
}