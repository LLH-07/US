#pragma once
#include <iostream>
#include <string>
#include <exception>

using namespace std;

class Fraction
{
private:
    int numerator;
    int denominator;

public:
    Fraction() {}
    Fraction(int a) : numerator(a), denominator(1) {}

    void setValue(int a, int b)
    {
        numerator = a;
        denominator = b;
    }
};

class DividedByZeroException : public exception
{
public:
    const char *what() const throw()
    {
        return "Exception: Cannot be divided by zero!";
    }
};
