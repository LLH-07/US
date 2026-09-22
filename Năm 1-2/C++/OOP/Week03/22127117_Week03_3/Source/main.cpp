#include "Bai01.h"
#include "Bai02.h"
#include "Bai03.h"
#include "Fraction.h"
#include <iostream>
#include <cstring>

using namespace std;

int main()
{
    // Bai 01
    cout << "----- BAI 01 -----\n";
    Stack<int> integerStack;
    if (integerStack.isEmpty())
        cout << "Stack is empty!\n";
    int i = 0;
    while (!integerStack.isFull())
    {
        integerStack.push(i);
        i++;
    }
    cout << "Stack is full!\n";
    cout << "Top element: " << integerStack.getTop() << endl;
    integerStack.clear();
    if(integerStack.isEmpty()) cout << "Stack is empty\n";

    //----------------------------------------------------------

    cout << endl;
    Fraction f(3, 4);
    Stack<Fraction> fStack;
    fStack.push(f);
    if (fStack.isEmpty())
        cout << "Stack is empty!\n";
    else
        cout << "Stack is not empty!\n";

    while (!fStack.isFull())
    {
        cout << "Top element: " << fStack.getTop() << endl;
        f += fStack.getTop();
        fStack.push(f);
    }
    if(fStack.isFull()) cout << "Top element: " << fStack.getTop() << " - Stack is full!" << endl;
    fStack.pop();
    cout << "Top element after popping top element: " << fStack.getTop() << endl << endl;


    // Bai 02
    cout << "\n----- BAI 02 -----\n";
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

    // Bai 03
    cout << "\n----- BAI 03 -----\n";
    Course course("1111", "Data Science");
    course.enroll();

    vector<Student> excellent = course.getExcellentList();
    vector<Student> good = course.getGoodList();
    vector<Student> prettyGood = course.getPrettyGoodList();
    vector<Student> average = course.getAverageList();
    vector<Student> under = course.getUnderAverageList();

    cout << "Excellent List\n";
    listList(excellent);
    cout << "Good List\n";
    listList(good);
    cout << "Pretty Good List\n";
    listList(prettyGood);
    cout << "Average List\n";
    listList(average);
    cout << "Under Average List\n";
    listList(under);
    
    cout << "Number of passed student: " << course.getPassedStudentsNumber() << endl;
    cout << "Number of failed student: " << course.getFailedStudentsNumber() << endl;

    char opt;
    cout << "Do you want to unenroll students?(y/n)";
    cin >> opt;
    if(tolower(opt) == 'y') course.unenroll();
    else if(tolower(opt) != 'n') cout << "ERROR!\n";

    cout << "Student left: " << course.getFailedStudentsNumber() + course.getPassedStudentsNumber() << endl;

    course.sortDecreasingListByGrade();

    string filename;
    cout << "Input name's file to save: ";
    cin.ignore();
    getline(cin, filename);
    course.saveToFile(filename);
}

