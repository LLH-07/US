#include "CongTy.h"
#include <iostream>

using namespace std;

int main()
{
    // Bai 1
    CongTy ABC;

    // Bai 2
    ABC.insertEmployeeList();

    // Bai 3
    ABC.getEmployeeList();

    // Bai 4
    cout << "Total salary: " << ABC.getTotalSalary() << endl;

    // Bai 5
    cout << "Employee has the highest salary:\n";
    Employee *e = ABC.getHighestSalaryEmployee();
    cout << "Fullname: " << e->getName();

    // Bai 6
    cout << "The number of producer: " << ABC.getNumProducer() << endl;

    // Bai 7
    cout << "The number of officer: " << ABC.getNumOfficer() << endl;

    // Bai 8
    cout << "The average salary: " << ABC.calculateAverageSalary() << endl;

    cout << "\n-----------------------------\n\n";

    // Bai 9
    vector<Employee *> under3mSalaryEmployee = ABC.getUnder3millionSalaryEmployeeList();
    for (int i = 0; i < under3mSalaryEmployee.size(); i++)
    {
        under3mSalaryEmployee[i]->getInformation();
        cout << endl;
    }
    cout << "-----------------------------\n]n";

    // Bai 10
    string id;
    Employee *idEmployee;
    cout << "Search Employee by ID: ";
    getline(cin, id);
    idEmployee = ABC.searchByID(id);
    idEmployee->getInformation();
    cout << endl;

    // Bai 11
    string name;
    Employee *nameEmployee;
    cout << "Search Employee by name: ";
    getline(cin, name);
    nameEmployee = ABC.searchByName(name);
    nameEmployee->getInformation();
    cout << "\n-----------------------------\n\n";

    // Bai 12
    cout << "The number of employee was born in May: " << ABC.countEmployeeBornInMonth(5) << endl;
    cout << "\n-----------------------------\n\n";

    return 0;
}