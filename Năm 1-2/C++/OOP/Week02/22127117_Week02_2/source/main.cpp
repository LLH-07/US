#include "Bai01.h"
#include "Bai02.h"
#include "Bai03.h"
#include "Bai04.h"
#include <iostream>

using namespace std;

int main()
{
    // Problem 1
    cout << "\n----- PROBLEM 1 -----\n";
    Fraction f1, f2;     // 0/1
    Fraction f3(1, -7);  // -1/7
    Fraction f4(f3);     // Copy constructor
    Fraction f5 = f2;    // Copy constructor
    Fraction f6, f7, f8; // Default constructor
    f6 = f3;             // Operator =
    f7 = f1 + f5;
    f8 = f2 - f4;
    f3 = f1 * f7;
    f5 = f6 / f2; // Cannot divide by zero!

    if (f2 == f3)
        cout << "f2==f3" << endl;
    if (f3 != f1)
        cout << "f3!=f1" << endl;
    if (f2 >= f5)
        cout << "f2>=f5" << endl;
    if (f2 > f5)
        cout << "f2>f5" << endl;
    if (f5 <= f3)
        cout << "f5<=f3" << endl;
    if (f5 < f3)
        cout << "f5<f3" << endl;
    f1 = f2 + 3;
    f3 = -7 + f1;
    f5 = 7 * f3;
    f6 = f4 - 6;
    cout << f3 << endl;
    cout << f6 << endl;
    f1 += f5;
    f6 -= f7;
    f8 *= f1;
    f8 /= f2; // Cannot divide by zero!
    cout << f8++ << endl;
    cout << ++f7 << endl;
    cout << f8-- << endl;
    cout << --f7 << endl;
    f3 += Fraction(11, 2);
    int x = int(f3);     // 3/2 => 1
    float f = (float)f3; // 3/2 => 1.5
    cout << x << endl;
    cout << f << endl;

    // Problem 2
    cout << "\n----- PROBLEM 2 -----\n";
    Stack s(10);

    if(s.isEmpty())
        cout << "Stack is empty!\n";

    s.push(0);
    if(s.isEmpty())
        cout << "Stack is empty!\n";
    else cout << "Stack is not empty!\n";

    for(int i = 1; i < 10; i++)
        s.push(i);
    if(s.isFull()) cout << "Stack is full!\n";

    s.pop();
    if(s.isFull()) cout << "Stack is full!\n";
    else cout << "Stack is not full!\n";

    cout << "Top value: " << s.getTopValue() << endl;

    // Problem 3
    cout << "\n----- PROBLEM 3 -----\n";
    Queue q(10);

    if(q.isEmpty()) cout << "Queue is empty!\n";
    else cout << "Queue is not empty!\n";

    for(int i = 0; i < 10; i++)
        q.enqueue(i);

    if(q.isFull()) cout << "Queue is full!\n";
    else cout << "Queue is not full!";

    q.dequeue();
    if(q.isFull()) cout << "Queue is full!\n";
    else cout << "Queue is not full!\n";

    cout << "Front value: " << q.getFrontValue() << endl;

    // Problem 4
    cout << "\n----- PROBLEM 4 -----\n";
    Student student;
    cin >> student;
    cout << endl << student;

    return 0;
}
