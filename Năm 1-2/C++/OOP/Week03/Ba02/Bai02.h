#pragma once
#include <iostream>
#include <string>

using namespace std;

class Student
{
private:
    string studentID;
    double grade;
    string fullname;

public:
    Student() : studentID(""), grade(0), fullname(""){};
    Student(string ID, double GRADE, string FULLNAME) : studentID(ID), grade(GRADE), fullname(FULLNAME){};

    void insertInfo();
    void getInfo(); 
    void setID(string ID);
    void setGrade(double GRADE);
    void setName(string FULLNAME);
    string getID();
    double getGrade();
    string getName();
};

void Student::insertInfo()
{
    cout << "Get Student Information\n";
    cout << "ID: "; 
    getline(cin, studentID);

    cout << "Fullname: ";
    getline(cin, fullname);

    cout << "Grade: ";
    cin >> grade;

    cout << endl;
}

void Student::getInfo()
{
    cout << "Student Information\n";
    cout << "ID: " << studentID << endl;
    cout << "Fullname: " << fullname << endl;
    cout << "Grade: " << grade << endl << endl;
}

void Student::setID(string ID)
{
    studentID = ID;
}

void Student::setGrade(double GRADE)
{
    grade = GRADE;
}

void Student::setName(string FULLNAME)
{
    fullname = FULLNAME;
}

string Student::getID()
{
    return studentID;
}

double Student::getGrade()
{
    return grade;
}

string Student::getName()
{
    return fullname;
}



