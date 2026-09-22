#pragma once
#include <iostream>

using namespace std;

class Fraction
{
private:
    int numerator;
    int denominator;
public:
    Fraction(int numerator, int denominator)
    {
        this->numerator = numerator;
        this->denominator = denominator;

        if(this->denominator == 0)
        {
            cout << "Fraction is invalid\n";
            exit(0);
        }
    }

    Fraction(){
        numerator = 1;
        denominator = 1;
    }

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
    if(b == 0)
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
    int nb, db, resD, resN;

    nb = b.getNumerator();
    db = b.getDenominator();

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

    if(b == 0)
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
