#include "Company.h"

int main()
{
    // BAI 1 -> BAI 4
    Employee e;
    Officer o;
    Producer p;
    Manager m;
    
    // BAI 5 -> BAI 8
    // Input each kind of employee information
    e.insertEmployee();
    cout << endl;
    o.insertOfficer();
    p.insertProducer();
    m.insertManager();

    // Output each kind of employee information
    e.getEmployee();
    cout << endl;
    o.getOfficer();
    p.getProducer();
    m.getManager();

    // BAI 9
    Company abc;

    // BAI 10
    abc.insertEmployeeList();

    // BAI 11
    abc.getEmployeesInfo();

    // BAI 12
    cout << "Total salary: " << abc.calculateTotalSalary() << endl;
    
    // BAI 13
    string name;
    Employee foundEmployee;
    cout << "FIND AN EMPLOYEE\n";
    cout << "Name: "; getline(cin, name);
    foundEmployee = abc.findEmployeeByName(name);

}