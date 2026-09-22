#pragma once
#include "Employee.h"
#include "Officer.h"
#include "Producer.h"
#include "Manager.h"

class Company
{
private:
    Employee *listEmployee;
    Officer *listOfficer;
    Producer *listProducer;
    Manager *listManager;
    int numEmployee = 1000, numOfficer = 1000, numProducer = 1000,  numManager = 1000;
   
public:
    // Constructor
    Company()
    {
        listEmployee = new Employee[numEmployee];
        listOfficer = new Officer[numOfficer];
        listProducer = new Producer[numProducer];
        listManager = new Manager[numManager];
    }
    
    // Destructor
    ~Company()
    {
        delete[] listEmployee;
        delete[] listOfficer;
        delete[] listProducer;
        delete[] listManager;
    }

    void insertEmployeeList();
    void getEmployeesInfo();
    int calculateTotalSalary();
    Employee findEmployeeByName(string name);

};

void Company::insertEmployeeList()
{
    cout << "Number of employees: "; cin >> numEmployee;
    cin.ignore();
    for(int i = 0; i < numEmployee; i++)
        listEmployee[i].insertEmployee();
}

void Company::getEmployeesInfo()
{
    cout << "EMPLOYEES INFORMATION\n";
    for(int i = 0; i < numEmployee; i++)
        listEmployee[i].getEmployee();
}

int Company::calculateTotalSalary()
{
    int sum = 0;
    for(int i = 0;i < numEmployee; i++)
        sum += listEmployee[i].calculateSalary();

    return sum;
}

Employee Company::findEmployeeByName(string name)
{
    for(int i = 0; i < numEmployee; i++)
        if(listEmployee[i].getName() == name)
        {
            cout << "Employee is found!\n";
            return listEmployee[i];
        }

    cout << "Name doesn't exist!\n";
}