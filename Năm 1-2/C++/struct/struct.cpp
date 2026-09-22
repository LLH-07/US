#include <iostream>
#include <cmath>

using namespace std;

struct Point
{
    float x;
    float y;
};

struct Triangle
{
    Point A;
    Point B;
    Point C;
};

void inputTriangle(Triangle &t)
{
    cout << "Toa do diem A: ";
    cin >> t.A.x >> t.A.y;

    cout << "Toa do diem B: ";
    cin >> t.B.x >> t.B.y;

    cout << "Toa do diem C: ";
    cin >> t.C.x >> t.C.y;
}

float binhphuong(float a)
{
    return a * a;
}

float getDistance(Point a, Point b)
{
    float dodai;
    dodai = sqrt (binhphuong(b.x - a.x) + binhphuong(b.y - a.y));
}

int isTriangle(Triangle t)
{
    float AB = getDistance(t.A, t.B);
    float BC = getDistance(t.B, t.C);
    float AC = getDistance(t.A, t.C);

    if((AB + BC > AC) && (AB + AC > BC) && (BC + AC > AC)) return 1;
    return 0;
}

float getAreaTriangle(Triangle t)
{
    float AB = getDistance(t.A, t.B);
    float BC = getDistance(t.B, t.C);
    float AC = getDistance(t.A, t.C);
    float p = (AB + BC + AC) / 2;
    float S = sqrt (p * (p - AB) * (p - BC) * (p - AC));
    return S;
}

float getCirTriangle(Triangle t)
{
    float AB = getDistance(t.A, t.B);
    float BC = getDistance(t.B, t.C);
    float AC = getDistance(t.A, t.C);
    return AB + BC + AC;
}

int main()
{
    Triangle t;
    inputTriangle(t);
}