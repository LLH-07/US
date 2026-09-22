#pragma once
#include <iostream>

using namespace std;

class Fraction
{
private:
    int numerator = 0;
    int denominator = 1;

public:
    // Constructor
    Fraction(int numerator, int denominator);
    Fraction(int numerator);
    Fraction();
    Fraction(const Fraction &src);

    Fraction &operator=(const Fraction &src);
    Fraction operator+(const Fraction &src);
    Fraction operator+(int b);
    friend Fraction operator+(int b, const Fraction &src);
    Fraction operator-(const Fraction &src);
    Fraction operator-(int b);
    friend Fraction operator-(int b, const Fraction &src);
    Fraction operator*(const Fraction &src);
    friend Fraction operator*(int b, const Fraction &src);
    Fraction operator/(const Fraction &src);
    bool operator==(const Fraction &src);
    bool operator!=(const Fraction &src);
    bool operator>=(const Fraction &src);
    bool operator>(const Fraction &src);
    bool operator<=(const Fraction &src);
    bool operator<(const Fraction &src);
    Fraction &operator+=(const Fraction &src);
    Fraction &operator-=(const Fraction &src);
    Fraction &operator*=(const Fraction &src);
    Fraction &operator/=(const Fraction &src);
    Fraction &operator++();     // ++a
    Fraction &operator--();     // --a
    Fraction operator++(int f); // a++
    Fraction operator--(int f); // a--
    friend istream &operator>>(istream &is, Fraction &ps);
    friend ostream &operator<<(ostream &os, const Fraction &ps);
    operator int() const
    {
        int res = numerator / denominator;
        return res;
    }
    operator float() const
    {
        float res = numerator * 1.0 / denominator;
        return res;
    }

    // Methods
    int getNumerator();
    int getDenominator();
    Fraction addFraction(Fraction b);
    Fraction minusFraction(Fraction b);
    Fraction multiplyFraction(Fraction b);
    Fraction divideFraction(Fraction b);
    void inputFraction();
    void getFraction1();
    void getFraction2();
};

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int lcm(int a, int b)
{
    return (a * b) / gcd(a, b);
}

void simplise(int &numerator, int &denominator)
{
    int tmp = gcd(numerator, denominator);
    numerator /= tmp;
    denominator /= tmp;
}

// Constructor
Fraction::Fraction(int numerator, int denominator)
{
    this->numerator = numerator;
    this->denominator = denominator;
}

Fraction::Fraction(int numerator)
{
    this->numerator = numerator;
    this->denominator = 1;
}

Fraction::Fraction()
{
    numerator = 0;
    denominator = 1;
}

Fraction::Fraction(const Fraction &src)
{
    numerator = src.numerator;
    denominator = src.denominator;
}

Fraction &Fraction::operator=(const Fraction &src)
{
    if (this != &src)
    {
        numerator = src.numerator;
        denominator = src.denominator;
    }

    return *this;
}

Fraction Fraction::operator+(const Fraction &src)
{
    Fraction res;
    res.denominator = lcm(denominator, src.denominator);
    res.numerator = numerator * (res.denominator / denominator) + src.numerator * (res.denominator / src.denominator);
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction Fraction::operator+(int b)
{
    Fraction res;
    res.numerator = numerator + b * denominator;
    res.denominator = denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction operator+(int b, const Fraction &src)
{
    Fraction res;
    res.numerator = src.numerator + b * src.denominator;
    res.denominator = src.denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction Fraction::operator-(const Fraction &src)
{
    Fraction res;
    res.denominator = lcm(denominator, src.denominator);
    res.numerator = numerator * (res.denominator / denominator) - src.numerator * (res.denominator / src.denominator);
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction Fraction::operator-(int b)
{
    Fraction res;
    res.numerator = numerator - b * denominator;
    res.denominator = denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction operator-(int b, const Fraction &src)
{
    Fraction res;
    res.numerator = src.numerator - b * src.denominator;
    res.denominator = src.denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction Fraction::operator*(const Fraction &src)
{
    Fraction res;
    res.numerator = numerator * src.numerator;
    res.denominator = denominator * src.denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction operator*(int b, const Fraction &src)
{
    Fraction res;
    res.numerator = b * src.numerator;
    res.denominator = src.denominator;
    simplise(res.numerator, res.denominator);
    return res;
}

Fraction Fraction::operator/(const Fraction &src)
{
    if (src.numerator == 0)
        cout << "Cannot divide by zero!\n";

    else
    {
        Fraction res;
        Fraction tmp(src.denominator, src.numerator);
        res = (*this) * tmp;

        return res;
    }

    return *this;
}

bool Fraction::operator==(const Fraction &src)
{
    int tmp = numerator * src.denominator - src.numerator * denominator;
    if (tmp == 0)
        return true;

    return false;
}

bool Fraction::operator!=(const Fraction &src)
{
    if ((*this) == src)
        return false;
    return true;
}

bool Fraction::operator>=(const Fraction &src)
{
    if ((numerator * 1.0 / denominator) < (src.numerator * 1.0 / src.denominator))
        return false;

    return true;
}

bool Fraction::operator>(const Fraction &src)
{
    if ((numerator * 1.0 / denominator) > (src.numerator * 1.0 / src.denominator))
        return true;

    return false;
}

bool Fraction::operator<=(const Fraction &src)
{
    if ((numerator * 1.0 / denominator) <= (src.numerator * 1.0 / src.denominator))
        return true;

    return false;
}

bool Fraction::operator<(const Fraction &src)
{
    if((numerator * 1.0 / denominator) < (src.numerator * 1.0 / src.denominator))
        return true;

    return false;
}

Fraction &Fraction::operator+=(const Fraction &src)
{
    Fraction res;
    res = (*this) + src;
    numerator = res.numerator;
    denominator = res.denominator;
    return *this;
}

Fraction &Fraction::operator-=(const Fraction &src)
{
    Fraction res;
    res = (*this) - src;
    numerator = res.numerator;
    denominator = res.denominator;
    return *this;
}

Fraction &Fraction::operator*=(const Fraction &src)
{
    Fraction res;
    res = (*this) * src;
    numerator = res.numerator;
    denominator = res.denominator;
    return *this;
}

Fraction &Fraction::operator/=(const Fraction &src)
{
    if (src.numerator == 0)
        cout << "Cannot divide by zero!\n";

    else
    {
        Fraction res;
        res = (*this) / src;
        numerator = res.numerator;
        denominator = res.denominator;
    }

    return *this;
}

Fraction &Fraction::operator++()
{
    numerator += denominator;
    return *this;
}

Fraction &Fraction::operator--()
{
    numerator -= denominator;
    return *this;
}

Fraction Fraction::operator++(int f)
{
    Fraction res(*this);
    numerator += denominator;
    return res;
}

Fraction Fraction::operator--(int f)
{
    Fraction res(*this);
    numerator -= denominator;
    return res;
}

istream &operator>>(istream &is, Fraction &src)
{
    cout << "Numerator: ";
    is >> src.numerator;

    cout << "Denominator: ";
    is >> src.denominator;

    return is;
}

ostream &operator<<(ostream &os, const Fraction &src)
{
    os << src.numerator;
    if(src.denominator != 1) os << " / " << src.denominator;
    return os;
}

int Fraction::getNumerator()
{
    return numerator;
}

int Fraction::getDenominator()
{
    return denominator;
}

Fraction Fraction::addFraction(Fraction b)
{
    int nb, db, resD, resN;

    nb = b.getNumerator();
    db = b.getDenominator();
    resD = lcm(denominator, db);
    resN = numerator * (resD / denominator) + nb * (resD / db);
    simplise(resN, resD);

    return Fraction(resN, resD);
}

Fraction Fraction::minusFraction(Fraction b)
{
    int nb, db, resD, resN;

    nb = b.getNumerator();
    db = b.getDenominator();
    resD = lcm(denominator, db);
    resN = numerator * (resD / denominator) - nb * (resD / db);
    simplise(resN, resD);

    return Fraction(resN, resD);
}

Fraction Fraction::multiplyFraction(Fraction b)
{
    int nb, db, resD, resN;

    nb = b.getNumerator();
    db = b.getDenominator();
    resN = numerator * nb;
    resD = denominator * db;
    simplise(resN, resD);

    return Fraction(resN, resD);
}

Fraction Fraction::divideFraction(Fraction b)
{
    int nb, db;

    nb = b.getNumerator();
    db = b.getDenominator();

    if (nb == 0)
    {
        cout << "Cannot divide by zero!\n";
        return b;
    }

    Fraction res(db, nb);
    return multiplyFraction(res);
}

void Fraction::inputFraction()
{
    int a, b;

    cout << "Numerator: ";
    cin >> a;

    cout << "Denominator: ";
    cin >> b;

    if (b == 0)
    {
        cout << "Fraction is invalid!\n";
        exit(0);
    }

    numerator = a;
    denominator = b;
}

void Fraction::getFraction1()
{
    cout << "a / b = ";
    cout << getNumerator() << " / " << getDenominator() << endl;
}

void Fraction::getFraction2()
{
    float res = getNumerator() * 1.0 / getDenominator();
    cout << "a / b = " << res << endl;
}
