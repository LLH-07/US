#pragma once
// #include "Employee.h"
#include "Officer.h"
#include "Producer.h"
#include "Manager.h"
#include <vector>
#include <iostream>

using namespace std;

class CongTy
{
private:
    vector<Employee *> listEmployee;
    int numTypes[3] = {};

public:
    void insertEmployeeList();
    void getEmployeeList();
    long long getTotalSalary();
    Employee *getHighestSalaryEmployee();
    int getNumProducer();
    int getNumOfficer();
    double calculateAverageSalary();
    vector<Employee*> getUnder3millionSalaryEmployeeList();
    Employee* searchByID(string ID);
    Employee* searchByName(string name);
    int countEmployeeBornInMonth(short m);
};

void CongTy::insertEmployeeList()
{
    int n;
    cout << "Insert the number of employee: ";
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        short typeEmployee;
        string hoTen;
        short namVaoLam;
        long long luongCoBan;
        string ma;
        short d, m, y;
        Employee *employee;

        cout << "Information of employee " << i + 1 << endl;
        cout << "1. Officer\n"
             << "2. Producer\n"
             << "3. Manager\n";
        cout << "Type of employee(1/2/3): ";
        cin >> typeEmployee;

        cin.ignore();
        cout << "ID: ";
        getline(cin, ma);

        cout << "Fullname: ";
        getline(cin, hoTen);

        cout << "Date of birth:\n";
        cout << "Day: "; cin >> d;
        cout << "Month: "; cin >> m;
        cout << "Year: "; cin >> y;

        cout << "Entering year: ";
        cin >> namVaoLam;

        cout << "Basic salary: ";
        cin >> luongCoBan;

        if (typeEmployee == 1)
            employee = new Officer(hoTen, namVaoLam, luongCoBan, ma, d, m, y);
        else if (typeEmployee == 2)
            employee = new Producer(hoTen, namVaoLam, luongCoBan, ma, d, m, y);
        else if (typeEmployee == 3)
            employee = new Manager(hoTen, namVaoLam, luongCoBan, ma, d, m, y);
        else
        {
            cout << "Type of employee is not suitable!\n";
            return;
        }

        numTypes[typeEmployee - 1]++;
        employee->insertInformation();
        listEmployee.push_back(employee);
        cout << endl;
    }
}

void CongTy::getEmployeeList()
{
    cout << "EMPLOYEE LIST\n";
    for (int i = 0; i < listEmployee.size(); i++)
    {
        cout << "Fullname: " << listEmployee[i]->getName() << endl;
        cout << "Enter year: " << listEmployee[i]->getEnterYear() << endl;
        cout << "Basic salary: " << listEmployee[i]->calculateSalary() << endl;
        listEmployee[i]->getInformation();
        cout << endl;
    }
}

long long CongTy::getTotalSalary()
{
    long long total = 0;
    for (int i = 0; i < listEmployee.size(); i++)
        total += listEmployee[i]->calculateSalary();

    return total;
}

Employee *CongTy::getHighestSalaryEmployee()
{
    int pos = 0;
    long long maxSalary = listEmployee[0]->calculateSalary();

    for (int i = 1; i < listEmployee.size(); i++)
    {
        if (maxSalary < listEmployee[i]->calculateSalary())
        {
            pos = i;
            maxSalary = listEmployee[i]->calculateSalary();
        }
    }

    return listEmployee[pos];
}

int CongTy::getNumProducer()
{
    return numTypes[1];
}

int CongTy::getNumOfficer()
{
    return numTypes[0];
}

double CongTy::calculateAverageSalary()
{
    return (getTotalSalary() * 1.0) / listEmployee.size();
}

vector<Employee*> CongTy::getUnder3millionSalaryEmployeeList()
{
    vector<Employee*> list;
    for(int i = 0; i < listEmployee.size(); i++)
    {
        if(listEmployee[i]->calculateSalary() < 3000000)
            list.push_back(listEmployee[i]);
    }

    return list;
}

Employee* CongTy::searchByID(string ID)
{
    for(int i = 0; i < listEmployee.size(); i++)
        if(listEmployee[i]->getID() == ID)
            return listEmployee[i];

    return nullptr;
}

Employee* CongTy::searchByName(string name)
{
    for(int i = 0; i < listEmployee.size(); i++)
        if(listEmployee[i]->getName() == name)
            return listEmployee[i];

    return nullptr;
}

int CongTy::countEmployeeBornInMonth(short m)
{
    int res = 0;
    for(int i = 0; i < listEmployee.size(); i++)
        if(listEmployee[i]->getBirthMonth() == m)
            res++;

    return res;
}

