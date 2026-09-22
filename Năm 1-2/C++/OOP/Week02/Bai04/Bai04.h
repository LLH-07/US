#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>

using namespace std;

class Student
{
private:
    char *studentId;
    double gpa;
    char *fullname;
    char *address;

public:
    // Constructor
    Student();
    Student(char *ID, double GPA, char *FULLNAME, char *ADDRESS);

    // Copy constructor
    Student(const Student &src);

    // Destructor
    ~Student();

    friend ostream &operator<<(ostream &os, const Student &src);
    friend istream &operator>>(istream &is, Student &src);
};

Student::Student()
{
    studentId = nullptr;
    gpa = 0;
    fullname = nullptr;
    address = nullptr;
}

Student::Student(char *ID, double GPA, char *FULLNAME, char *ADDRESS)
{
    studentId = new char[strlen(ID) + 1];
    fullname = new char[strlen(FULLNAME) + 1];
    address = new char[strlen(ADDRESS) + 1];

    strcpy(studentId, ID);
    gpa = GPA;
    strcpy(fullname, FULLNAME);
    strcpy(address, ADDRESS);
}

Student::Student(const Student &src)
{
    studentId = new char[strlen(src.studentId) + 1];
    fullname = new char[strlen(src.fullname) + 1];
    address = new char[strlen(src.address) + 1];

    strcpy(studentId, src.studentId);
    gpa = src.gpa;
    strcpy(fullname, src.fullname);
    strcpy(address, src.address);
}

Student::~Student()
{
    delete[] studentId;
    delete[] fullname;
    delete[] address;
}

ostream &operator<<(ostream &os, const Student &src)
{
    os << "Student's information:\n";
    os << "ID: " << src.studentId << endl;
    os << "Fullname: " << src.fullname << endl;
    os << "Address: " << src.address << endl;
    os << "GPA: " << src.gpa << endl;

    return os;
}

istream &operator>>(istream &is, Student &src)
{
    char ID[30];
    char FULLNAME[100];
    char ADDRESS[100];

    cout << "Insert student's information:\n";
    cout << "Enter ID: ";
    is.getline(ID, sizeof(ID));

    cout << "Enter fullname: ";
    is.getline(FULLNAME, sizeof(FULLNAME));

    cout << "Enter address: ";
    is.getline(ADDRESS, sizeof(ADDRESS));

    cout << "Enter GPA: ";
    is >> src.gpa;

    src.studentId = new char[sizeof(ID) + 1];
    src.fullname = new char[sizeof(FULLNAME) + 1];
    src.address = new char[sizeof(ADDRESS) + 1];

    strcpy(src.studentId, ID);
    strcpy(src.fullname, FULLNAME);
    strcpy(src.address, ADDRESS);

    return is;
}
