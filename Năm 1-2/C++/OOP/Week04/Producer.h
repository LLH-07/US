#pragma once
#include "Employee.h"

class Producer : public Employee
{
private:
    int productNum;

public:
    Producer() : Employee(){};
    Producer(string FULLNAME) : Employee(FULLNAME){};
    Producer(string FULLNAME, int product) : Employee(FULLNAME), productNum(product){};
    Producer(string FULLNAME, int SALARY, int product) : Employee(FULLNAME, SALARY), productNum(product){};
    Producer(int start, string FULLNAME, int product) : Employee(start, FULLNAME), productNum(product){};
    Producer(string FULLNAME, int start, int SALARY, int product) : Employee(FULLNAME, start, SALARY), productNum(product){};

    //
    void insertProducer();
    void getProducer();
    int calculateSalary();
};

void Producer::insertProducer()
{
    insertEmployee();
    cout << "NEW PRODUCER\n";
    cout << "The number of products: ";
    cin >> productNum;
    cout << endl;
    cin.ignore();

}

void Producer::getProducer()
{
    getEmployee();
    cout << "PRODUCER INFORMATION\n";
    cout << "The number of products: " << productNum << endl;
    cout << "Salary: " << calculateSalary() << endl;
    cout << endl;
}

int Producer::calculateSalary()
{
    salary = salary + productNum * 2000;
    return salary;
}