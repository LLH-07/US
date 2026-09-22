#include <iostream>
#include "Bai01.h"

using namespace std;

int main()
{
    Fraction a(5, 6);
    Fraction b(3, 2);
    Fraction c = a.divideFraction(b);
    c.getFraction1();
    c.getFraction2();
}
