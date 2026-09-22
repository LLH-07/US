#include <iostream>
#include <time.h>
#include <stdlib.h>
#define pi 3.1415

using namespace std;

// Problem 4
float Minus(float *a, float *b)
{
    float result;
    result = *a - *b;

    return result;
}

// Problem 6
float circleArea(float *r)
{
    float area;

    area = pi * (*r) * (*r);

    return area;
}

float circlePerimeter(float *r)
{
    float perimeter;

    perimeter = 2 * pi * (*r);

    return perimeter;
}

// Problem 7
float Volume(float *measure, float *Density)
{
    float v;
    v = *measure / *Density;
    *measure = 10;
    return v;
}

// Problem 8
float timeMoving(float *speed, float *distance)
{
    float time;
    time = *distance / *speed;

    return time;
}

// problem 9
float VAT(float *money)
{
    float total;
    total = 1.1 * (*money);
    return total;
}

