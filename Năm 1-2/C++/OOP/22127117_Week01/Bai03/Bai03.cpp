#include "Bai03.h"

using namespace std;

int main()
{
    Point a;
    cout << "First point:\n";
    a.input();
    a.getPoint();

    Point b;
    cout << "Second point:\n";
    b.input();
    b.getPoint();

    a.calculateDistance(b);
    a.getDistance(b);
}