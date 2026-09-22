#include <iostream>
#include "Bai01.h"
#include "Bai02.h"
#include "Bai03.h"
#include "Bai04.h"
#include "Bai05.h"

using namespace std;

int main()
{

    // PROBLEM 1
    cout << "----- Problem 1 -----\n";
    Fraction ps1, ps2;
    cout << "First fraction:\n";
    ps1.inputFraction();
    cout << "Second fraction:\n";
    ps2.inputFraction();
    
    Fraction res1 = ps1.addFraction(ps2);
    Fraction res2 = ps1.minusFraction(ps2);
    Fraction res3 = ps1.multiplyFraction(ps2);
    Fraction res4 = ps1.divideFraction(ps2);

    cout << "Addition:\n"; res1.getFraction1(); res1.getFraction2();
    cout << "Subtraction:\n"; res2.getFraction1(); res2.getFraction2();
    cout << "Multiple:\n"; res3.getFraction1(); res3.getFraction2();
    cout << "Division:\n"; res4.getFraction1(); res4.getFraction2();

    // PROBLEM 2
    cout << "----- Problem 2 -----\n";
    
    Date d1, d2, previousDay, tomorrow, nDaysBefore, nDaysAfter;
    int n;
    
    cout << "Insert date:\n";
    d1.inputDate();

    previousDay = d1.decrease1Day();
    tomorrow = d1.increase1Day();
    
    cout << "The previous day: "; previousDay.getDate();
    cout << "The day after: "; tomorrow.getDate();

    cout << "Input n = ";
    cin >> n;

    nDaysBefore = d1.decreaseNDay(n);
    nDaysAfter = d1.increaseNDay(n);

    cout << "Before n days: "; nDaysBefore.getDate();
    cout << "After n days: "; nDaysAfter.getDate(); 

    cout << "Insert the second date:\n";
    d2.inputDate();

    int isSameDate = d1.compare(d2);
    if(isSameDate == -1) cout << "The second date is greater!\n";
    else if(isSameDate == 1) cout << "The second date is smaller\n";
    else cout << "Two date is the same!\n";

    // PROBLEM 3
    cout << "----- Problem 3 -----\n";

    Point A, B;

    cout << "Insert the first point:\n";
    A.input(); 
    A.getPoint();

    cout << "Insert the second point:\n";
    B.input();
    B.getPoint();

    A.calculateDistance(B);
    A.getDistance(B);

    // PROBLEM 4
    cout << "----- Problem 4 -----\n";

    Rectangle hcn;
    float P, Area;

    cout << "Insert 4 points of a rectangle:\n";
    hcn.input();
    P = hcn.calculatePerimeter();
    Area = hcn.calculateArea();

    if(P != -1) cout << "Perimeter: " << P << endl;
    if(Area != -1) cout << "Area: " << Area << endl;

    // PROBLEM 5
    cout <<  "----- Problem 5 -----\n";

    Student s;
    cout << "Insert grades of 3 subjects of a student:\n";
    s.getInfo();
    
    return 0;
}