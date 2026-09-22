#include <iostream>
#include "Bai01.h"

using namespace std;

int main()
{
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
}