#pragma once
#include <iostream>

using namespace std;

class Student
{
private:
    float mathGrade, englishGrade, programmingGrade;

public:
    Student()
    {
        mathGrade = englishGrade = programmingGrade = 0;
    }

    void assignGrade(float math, float english, float programming);
    float calculateAverage();
    bool isExcellentStudent();
    void getInfo();
};

void Student::assignGrade(float math, float english, float programming)
{
    mathGrade = math;
    englishGrade = english;
    programmingGrade = programming;
}

float Student::calculateAverage()
{
    return (mathGrade + englishGrade + programmingGrade) / 3;
}

bool Student::isExcellentStudent()
{
    if (mathGrade < 6.5 || englishGrade < 6.5 || programmingGrade < 6.5)
        return false;

    if (calculateAverage() < 8)
        return false;

    return true;
}

void Student::getInfo()
{
    cout << "Math grade: ";
    cin >> mathGrade;

    cout << "English grade: ";
    cin >> englishGrade;

    cout << "Programming grade: ";
    cin >> programmingGrade;

    cout << "Average grade: " << calculateAverage() << endl;

    if(isExcellentStudent()) cout << "An excellent student!\n";
    else cout << "Not an excellent student!\n"; 
}
