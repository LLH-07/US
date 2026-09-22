#pragma once
#include "Employee.h"

using namespace std;

class Producer : public Employee
{
private:
    int numberOfProducts;

public:
    Producer(string HoTen, short namVaoLam, long long luongCoBan, string ma, short d, short m, short y) : Employee(HoTen, namVaoLam, luongCoBan, ma, d, m, y){}

    void insertInformation();
    void getInformation();
    long long calculateSalary();
};

void Producer::insertInformation()
{
    cout << "The number of products: ";
    cin >>  numberOfProducts;
}

void Producer::getInformation()
{
    cout << "The number of products: " << numberOfProducts << endl;
}

long long Producer::calculateSalary()
{
    return salary + numberOfProducts * 2000;
}