#pragma once
#include "Employee.h"

class Manager : public Employee
{
private:
    int coefficientPosition;
    int reward;

public:
    Manager() : Employee(){};
    Manager(string FULLNAME) : Employee(FULLNAME){};
    Manager(string FULLNAME, int SALARY, int Position) : Employee(FULLNAME, SALARY), coefficientPosition(Position){};
    Manager(int SALARY, string FULLNAME, int REWARD) : Employee(FULLNAME, SALARY), reward(REWARD){};
    Manager(int Position, int REWARD, string FULLNAME) : Employee(FULLNAME), coefficientPosition(Position), reward(REWARD){};
    Manager(string FULLNAME, int start, int SALARY, int Position, int REWARD) : Employee(FULLNAME, start, SALARY), coefficientPosition(Position), reward(REWARD){};

    //
    void insertManager();
    void getManager();
    int calculateSalary();
};

void Manager::insertManager()
{
    insertEmployee();
    cout << "NEW MANAGER\n";
    cout << "Coefficient position: "; cin >> coefficientPosition;
    cout << "Reward: "; cin >> reward;
    cout << endl;
    cin.ignore();
}

void Manager::getManager()
{
    getEmployee();
    cout << "MANAGER INFORMATION\n";
    cout << "Coefficient position: " << coefficientPosition << endl;
    cout << "Reward: " << reward << endl;
    cout << "Salary: " << calculateSalary() << endl;-
    cout << endl;
}

int Manager::calculateSalary()
{
    salary = salary * coefficientPosition + reward;
    return salary;
}