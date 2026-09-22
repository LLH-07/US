#include "Bai02.h"
#include <iostream>

using namespace std;

int main()
{
    Student s1("22127117", 8.5,"Ly Lien Hoa");
    Student s2;
    Student s3;

    s2.setID("21125057");
    s2.setGrade(9.9);
    s2.setName("NTP");

    cout << "Student 1\n";
    s1.getInfo();

    cout << "Student 2\n";
    s2.getInfo();

    s3.insertInfo();
    cout << "Student 3\n";
    s3.getInfo();



}