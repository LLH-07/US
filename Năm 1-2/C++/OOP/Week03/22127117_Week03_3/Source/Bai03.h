#pragma once
#include "Bai02.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Course
{
private:
    string code;
    string subject;
    vector<Student> studentList;

public:
    Course() : code(""), subject(""), studentList({}){};
    Course(string CODE, string SUBJECT) : code(CODE), subject(SUBJECT), studentList({}){};

    void enroll();
    void unenroll();
    vector<Student> getExcellentList();
    vector<Student> getGoodList();
    vector<Student> getPrettyGoodList();
    vector<Student> getAverageList();
    vector<Student> getUnderAverageList();
    int getPassedStudentsNumber();
    int getFailedStudentsNumber();
    void sortIncreasingListByName();
    void sortIncreasingListByGrade();
    void sortDecreasingListByName();
    void sortDecreasingListByGrade();
    void saveToFile(string filename);
    void readFile(string filename);
};

template <typename T>
void quickIncreasingSort(vector<T> arr, int first, int last, vector<Student> s)
{
    T pivot = arr[last];
    int left = first;
    int right = last - 1;

    while (left <= right)
    {
        while (arr[left] < pivot)
            left++;
        while (arr[right] > pivot)
            right--;
        if (left <= right)
        {
            swap(arr[left], arr[right]);
            swap(s[left], s[right]);
            left++;
            right--;
        }
    }

    swap(arr[left], arr[last]);
    swap(s[left], s[last]);

    if (first < right)
        quickIncreasingSort(arr, first, right, s);
    if (left < last)
        quickIncreasingSort(arr, left, last, s);
}

template <typename T>
void quickDecreasingSort(vector<T> arr, int first, int last, vector<Student> s)
{
    T pivot = arr[last];
    int left = first;
    int right = last - 1;

    while (left <= right)
    {
        while (arr[left] > pivot)
            left++;
        while (arr[right] < pivot)
            right--;
        if (left <= right)
        {
            swap(arr[left], arr[right]);
            swap(s[left], s[right]);
            left++;
            right--;
        }
    }

    swap(arr[left], arr[last]);
    swap(s[left], s[last]);

    if (first < right)
        quickDecreasingSort(arr, first, right, s);
    if (left < last)
        quickDecreasingSort(arr, left, last, s);
}

int findPosStudent(vector<Student> list, string id)
{
    for (int pos = 0; pos < list.size(); pos++)
    {
        if (list[pos].getID() == id)
            return pos;
    }

    return -1;
}

int getTypeStudent(Student s)
{
    if (s.getGrade() >= 9 && s.getGrade() <= 10)
        return 1;
    else if (s.getGrade() >= 8)
        return 2;
    else if (s.getGrade() >= 6.5)
        return 3;
    else if (s.getGrade() >= 5.0)
        return 4;
    return 5;
}

void listList(vector<Student> s)
{
    for (int i = 0; i < s.size(); i++)
        cout << s[i].getID() << "-" << s[i].getName() << "-" << s[i].getGrade() << endl;

    cout << endl;
}

void Course::enroll()
{
    short opt;
    while (true)
    {
        Student s;
        string id = "", name = "";
        double grade;

        cout << "New Student Information\n";
        cout << "ID: ";
        cin.ignore();
        getline(cin, id);
        cout << "Fullname: ";
        getline(cin, name);
        cout  << "Grade: ";
        cin >> grade;

        s.setID(id);
        s.setName(name);
        s.setGrade(grade);
        studentList.push_back(s);

        cout << "\nPRESS (0) to finish.\n";
        cout << "PRESS (1) to continue.\n";
        cout << "Your choice: ";
        cin >> opt;
        cout << "----------------------------------" << endl;
        cin.ignore();

        if (opt == 0)
            break;
        else if(opt > 1)
        {
            cout << "SYNTAX ERROR!\n";
            break;
        }
    }
}

void Course::unenroll()
{
    short opt;
    while (true)
    {
        Student s;
        string id = "";
        int pos;

        cout << "Unenroll Student\n";
        cout << "ID: ";
        getline(cin, id);

        pos = findPosStudent(studentList, id);
        if (pos != -1)
            studentList.erase(studentList.begin() + pos);
        else
            cout << "Student is not found!\n";

        cout << "\nPRESS (0) to finish.\n";
        cout << "PRESS (1) to continue.\n";
        cout << "Your choice: ";
        cin >> opt;
        cout << "----------------------------------" << endl;

        if (opt == 0)
            break;
    }
}

vector<Student> Course::getExcellentList()
{
    vector<Student> excellentList;
    for (int i = 0; i < studentList.size(); i++)
    {
        if (getTypeStudent(studentList[i]) == 1)
            excellentList.push_back(studentList[i]);
    }
    return excellentList;
}

vector<Student> Course::getGoodList()
{
    vector<Student> goodList;
    for (int i = 0; i < studentList.size(); i++)
    {
        if (getTypeStudent(studentList[i]) == 2)
            goodList.push_back(studentList[i]);
    }
    return goodList;
}

vector<Student> Course::getPrettyGoodList()
{
    vector<Student> prettyGoodList;
    for (int i = 0; i < studentList.size(); i++)
    {
        if (getTypeStudent(studentList[i]) == 3)
            prettyGoodList.push_back(studentList[i]);
    }
    return prettyGoodList;
}

vector<Student> Course::getAverageList()
{
    vector<Student> averageList;
    for (int i = 0; i < studentList.size(); i++)
    {
        if (getTypeStudent(studentList[i]) == 4)
            averageList.push_back(studentList[i]);
    }
    return averageList;
}

vector<Student> Course::getUnderAverageList()
{
    vector<Student> underAverageList;
    for (int i = 0; i < studentList.size(); i++)
    {
        if (getTypeStudent(studentList[i]) == 5)
            underAverageList.push_back(studentList[i]);
    }
    return underAverageList;
}

int Course::getPassedStudentsNumber()
{
    int cnt = 0;
    for (int i = 0; i < studentList.size(); i++)
        if (getTypeStudent(studentList[i]) != 5)
            cnt++;

    return cnt;
}

int Course::getFailedStudentsNumber()
{
    return studentList.size() - getPassedStudentsNumber();
}

void Course::sortIncreasingListByName()
{
    vector<string> nameList;
    int n = studentList.size();
    for(int i = 0; i < n; i++)
        nameList.push_back(studentList[i].getName());
    
    quickIncreasingSort(nameList, 0, n - 1, studentList);
}

void Course::sortIncreasingListByGrade()
{
    vector<double> gradeList;
    int n = studentList.size();
    for(int i = 0; i < n; i++)
        gradeList.push_back(studentList[i].getGrade());
    
    quickIncreasingSort(gradeList, 0, n - 1, studentList);
}

void Course::sortDecreasingListByName()
{
    vector<string> nameList;
    int n = studentList.size();
    for(int i = 0; i < n; i++)
        nameList.push_back(studentList[i].getName());
    
    quickDecreasingSort(nameList, 0, n - 1, studentList);
}

void Course::sortDecreasingListByGrade()
{
    vector<double> gradeList;
    int n = studentList.size();
    for(int i = 0; i < n; i++)
        gradeList.push_back(studentList[i].getGrade());
    
    quickDecreasingSort(gradeList, 0, n - 1, studentList);
}

void Course::saveToFile(string filename)
{
    ofstream out(filename);
    if(!out.is_open())
    {
        cout << "ERROR!\n";
        return;
    }

    out << code << endl;
    out << subject << endl;

    int n = studentList.size();
    out << n << endl;
    for(int i = 0; i < n; i++)
        out << studentList[i].getID() <<  "-" << studentList[i].getName() << "-" << studentList[i].getGrade() << endl;

    out.close();
}

void Course::readFile(string filename)
{
    ifstream in(filename);
    if(!in.is_open())
    {
        cout << "ERROR!\n";
        return;
    }

    getline(in, code);
    getline(in, subject);
    int n;
    in >> n;
    for(int i = 0; i < n; i++)
    {
        string id = "", name = "";
        double grade;

        getline(in, id, '-');
        getline(in, name, '-');
        in >> grade;

        studentList[i].setID(id);
        studentList[i].setName(name);
        studentList[i].setGrade(grade);
    }

    in.close();
}