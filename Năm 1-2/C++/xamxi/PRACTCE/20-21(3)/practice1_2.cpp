#include <iostream>
#include <string.h>
#include <cmath>
#include <iomanip>

using namespace std;

/*
Compare pros and cons when adding an element in a string and array.

PROS
- String :
    + Fast because no need to create more memory to store elements
    + Store documents
- Array :
    + Find elements fast
    + Add elements in same time without creating a new array

CONS
- String :
    + When a new element is added into a old string, it can't be changed,
    except initializing a new string variable
    + Find elements slow
- Array:
    + When adding a new element in available array, it need to initialize
    a new copy with a greater size which can decrease efficiency and
    increase run-time.
    + Besides, when an element is added, other elements need to moving 2 sides
    to create a space for a new one, it's time-consuming.
*/

/*
Compare pros and cons when deleting an elements sin singly linked-list and 1D array.

PROS
    - Singly Linked List
        + No need to copy next elements into a deleted element's position
        it will take less time to perform deleting.
        + Can delete any element in list.
        + No need to allocate again when deleting which minimize memory waste.
    - 1D array
        + Take less time than singly linked list in performing deleting because
        it can access a particular elements.
        + No need to find an deleted elements when its position is known.

CONS
    - Singly Linked List
        + When access an elements take more time because it need to traversal all
        elements previous.
    - 1D array
        + When deleting an element, the next elements need to move left one position
        It takes more time when perform deleting.
        + Can waste memory after deleting an element
*/

struct date
{
    int day;
    int month;
    int year;
};

struct Kid
{
    date d;
    double height = 0;
    double weight = 0;
};

struct Node
{
    Kid data;
    Node *next;
};

void showInfo(Kid k[], int n)
{
    int month[15] = {};
    cout << "Information about the kid:\n";
    for (int i = n - 1; i >= 0; i--)
    {
        if (month[k[i].d.month] == 0)
        {
            cout << k[i].d.day << "/" << k[i].d.month << "/" << k[i].d.year << '\n';
            cout << fixed << setprecision(2) << k[i].height << "m\n";
            cout << fixed << setprecision(1) << k[i].weight << "kg\n";
            cout << endl;
            month[k[i].d.month] = 1;
        }
    }
}

void showEmptyInfo(Kid k[], int n)
{
    int month[15] = {};
    cout << "Months have no information about the kid:\n";
    for (int i = 0; i < n; i++)
    {
        if (month[k[i].d.month] == 0 && k[i].height == 0 && k[i].weight == 0)
        {
            cout << k[i].d.month << endl;
            month[k[i].d.month] = 1;
        }
    }
    cout << endl;
}

bool isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

bool isValidDate(date D)
{
    int d, m, y;
    d = D.day;
    m = D.month;
    y = D.year;

    if (y < 0)
        return false;

    if (m < 1 || m > 12)
        return false;

    if (d < 1)
        return false;

    switch (d)
    {
    case 2:
        if (isLeapYear(y))
            if (d <= 29)
                return true;

            else if (d <= 28)
                return true;

        break;

    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if (d <= 31)
            return true;

        break;

    case 4:
    case 6:
    case 9:
    case 11:
        if (d <= 30)
            return true;

        break;
    }

    return false;
}

void delInvalidInfo(Node *&list, int &n)
{
    return;
}