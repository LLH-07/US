#include "Bai03.h"
#include <iostream>

using namespace std;

class Rectangle
{
private:
    Point a, b, c, d;

public:
    void input();
    void output();
    bool isValid();
    float calculatePerimeter();
    float calculateArea();
};

void Rectangle::input()
{
    cout << "First point:\n";
    a.input();

    cout << "Second point:\n";
    b.input();

    cout << "Third point:\n";
    c.input();

    cout << "Fourth point:\n";
    d.input();
}

void Rectangle::output()
{
    cout << "First point: ";
    a.getPoint();

    cout << "Second point: ";
    b.getPoint();

    cout << "Third point: ";
    c.getPoint();

    cout << "Fourth point: ";
    d.getPoint();

    cout << "Perimeter: " << calculatePerimeter() << endl;
    
    cout << "Area: " << calculateArea() << endl;
}

bool isValidHorizontalStraightLine(Point a, Point b)
{
    if (a.getY() != b.getY())
        return false;
    else if (a.getX() == b.getX())
        return false;

    return true;
}

bool isValidVerticalStraightLine(Point a, Point b)
{
    if (a.getX() != b.getX())
        return false;
    else if (a.getY() == b.getY())
        return false;

    return true;
}

bool Rectangle::isValid()
{
    return isValidHorizontalStraightLine(a, b) && isValidVerticalStraightLine(b, c) 
    && isValidHorizontalStraightLine(c, d) && isValidVerticalStraightLine(d, a);
}

float Rectangle::calculatePerimeter()
{
    if(!isValid())
    {
        cout << "the rectangle is not valid!\n";
        return -1;
    }

    float d1, d2;
    d1 = a.calculateDistance(b);
    d2 = b.calculateDistance(c);

    return (d1 + d2) * 2;
}

float Rectangle::calculateArea()
{
    if(!isValid())
    {
        cout << "Rectangle is not valid!\n";
        return -1;
    }

    float d1, d2;
    d1 = a.calculateDistance(b);
    d2 = b.calculateDistance(c);

    return d1 * d2;
}