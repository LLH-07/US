#include "Bai03.h"
#include <iostream>

using namespace std;

int main()
{
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

    course.unenroll();

    cout << "Student left: " << course.getFailedStudentsNumber() + course.getPassedStudentsNumber() << endl;

    course.saveToFile("output.txt");
}