#pragma once
#include <iostream>
#include <math.h>

using namespace std;

class Point
{
private:
    float x, y;

public:
    Point()
    {
        x = y = 0;
    }

    Point(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void input();
    float getX();
    float getY();
    void getPoint();
    float calculateDistance(Point b);
    void getDistance(Point b);
};

void Point::input()
{
    cout << "X = ";
    cin >> x;
    
    cout << "Y = ";
    cin >> y;
}

float Point::getX()
{
    return x;
}

float Point::getY()
{
    return y;
}

void Point::getPoint()
{
    cout << "(X, Y) = (" << x << ", " << y << ")" << endl; 
}

float Point::calculateDistance(Point b)
{
    float x2, y2, d;
    x2 = b.getX();
    y2 = b.getY();
    d = sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));

    return d;
}

void Point::getDistance(Point b)
{
    cout << "Distance between 2 points: " << calculateDistance(b) << endl;
}