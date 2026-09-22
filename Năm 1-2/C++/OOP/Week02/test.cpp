#include <iostream>

using namespace std;

int SUM(int a, int b)
{
    int c = a + b;
    return c;
}

int &sum(int a, int b)
{
    return a + b;
}

int main()
{
    int c1, c2;
    c1 = SUM(5, 2);
    c2 = sum(5, 2) = 6;
    cout << c1 << endl
         << c2;
}