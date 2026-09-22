#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>

using namespace std;

// Problem 2
struct TempScale
{
    double fahrenheit;
    double centigrade;
};

struct Reading
{
    int windSpeed;
    double humidity;
    TempScale temperature;
};

void showReading(Reading weather)
{
    cout << "Wind speed: " << weather.windSpeed << endl;
    cout << "Humidity: " << weather.humidity << endl;
    cout << "Fahrenheit: " << weather.temperature.fahrenheit << endl;
    cout << "Centigrade: " << weather.temperature.centigrade << endl;
}

void referReading(Reading &weather)
{
    cout << "Wind speed: ";
    cin >> weather.windSpeed;

    cout << "Humidity: ";
    cin >> weather.humidity;

    cout << "Fahrenheit: ";
    cin >> weather.temperature.fahrenheit;

    cout << "Centigrade: ";
    cin >> weather.temperature.centigrade;
}

Reading referReading()
{
    Reading weather;

    cout << "Wind speed: ";
    cin >> weather.windSpeed;

    cout << "Humidity: ";
    cin >> weather.humidity;

    cout << "Fahrenheit: ";
    cin >> weather.temperature.fahrenheit;

    cout << "Centigrade: ";
    cin >> weather.temperature.centigrade;

    return weather;
}

void recordReading(Reading *&weather)
{
    cout << "Wind speed: ";
    cin >> weather->windSpeed;

    cout << "Humidity: ";
    cin >> weather->humidity;

    cout << "Fahrenheit: ";
    cin >> weather->temperature.fahrenheit;

    cout << "Centigrade: ";
    cin >> weather->temperature.centigrade;
}

void main2()
{
    Reading weather;
    weather.windSpeed = 37;
    weather.humidity = 32;
    weather.temperature.fahrenheit = 32;
    weather.temperature.centigrade = 0;
}

// Problem 3
struct complex
{
    double real;
    double imag;
};

void getComplex(complex c1, complex c2)
{
    cin >> c1.real;
    cin >> c1.imag;

    cin >> c2.real;
    cin >> c2.imag;
}

void sumComplex(complex c1, complex c2)
{
    complex c3;
    c3.real = c1.real + c2.real;
    c3.imag = c1.imag + c2.imag;

    cout << c3.real;
    if (c3.imag < 0)
        cout << " - ";
    else
        cout << " + ";
    cout << abs(c3.imag) << 'i';
}

void multiComplex(complex c1, complex c2)
{
    complex c3;
    c3.real = c1.real * c2.real - c1.imag * c2.imag;
    c3.imag = c1.real * c2.imag + c1.imag * c2.real;

    cout << c3.real;
    if (c3.imag < 0)
        cout << " - ";
    else
        cout << " + ";
    cout << abs(c3.imag) << 'i';
}

// Problem 4
struct fraction
{
    int a;
    int b;
};

int GCD(int x, int y)
{
    if (x == 0)
        return y;
    return GCD(y % x, x);
}

int LCM(int x, int y)
{
    return (x * y) / GCD(x, y);
}

void getFrac(fraction &f)
{
    cin >> f.a;
    cin >> f.b;
}

void subtract2Frac(fraction f1, fraction f2)
{
    fraction f3;
    f3.b = LCM(f1.b, f2.b);
    f3.a = (f1.a * (f3.b / f1.b)) - (f2.a * (f3.b / f2.b));

    f3.a = f3.a / GCD(f3.a, f3.b);
    f3.b = f3.b / GCD(f3.a, f3.b);

    cout << f3.a << " / " << f3.b << endl;
}

// Problem 5
struct Employee
{
    string name;
    int salary;
    int hours;
};

void get10Employees(Employee *&e)
{
    srand(time(NULL));
    for (int i = 0; i < 10; i++)
    {
        e[i].name = char(i + 65);
        e[i].salary = rand() % 501 + 500;
        e[i].hours = rand() % 25;
    }
}

void increaseSalary(Employee *&e)
{
    for (int i = 0; i < 10; i++)
    {
        if (e[i].hours >= 8 && e[i].hours < 10)
            e[i].salary += 50;

        else if (e[i].hours >= 10 && e[i].hours < 12)
            e[i].salary += 100;

        else if (e[i].hours >= 12)
            e[i].salary += 150;
    }
}

void showFinalSalary(Employee *e)
{
    for (int i = 0; i < 10; i++)
        cout << e[i].name << '\t' << e[i].salary;
}

// Problem 6
struct Point
{
    double x;
    double y;
};

struct Triangle
{
    Point a;
    Point b;
    Point c;
};

double getDistance(Point a, Point b)
{
    return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

double perimeterTriangle(Triangle t)
{
    double P;
    double d1 = getDistance(t.a, t.b);
    double d2 = getDistance(t.a, t.c);
    double d3 = getDistance(t.b, t.c);

    P = d1 + d2 + d3;

    return P;
}

double areaTriangle(Triangle t)
{
    double s, A;
    double d1 = getDistance(t.a, t.b);
    double d2 = getDistance(t.a, t.c);
    double d3 = getDistance(t.b, t.c);

    s = perimeterTriangle(t) / 2;
    A = sqrt(s * (s - d1) * (s - d2) * (s - d3));

    return A;
}

bool isTriangle(Triangle t)
{
    double d1 = getDistance(t.a, t.b);
    double d2 = getDistance(t.a, t.c);
    double d3 = getDistance(t.b, t.c);

    if ((d1 + d2 > d3) && (d1 + d3 > d2) && (d2 + d3 > d1))
        return true;

    return false;
}

void getTriangle(Triangle &t)
{
    cout << "First point: ";
    cin >> t.a.x >> t.a.y;

    cout << "Second point: ";
    cin >> t.b.x >> t.b.y;

    cout << "Third point: ";
    cin >> t.c.x >> t.c.y;

    if (!isTriangle(t))
    {
        cout << endl;
        cout << "Three points can't form a triangle! Please input again!\n";
        getTriangle(t);
    }
}

// Problem 7
struct date
{
    int day;
    int month;
    int year;
};

bool isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

int getDaysInMonth(int month, int year)
{
    switch (month)
    {
    case 2:
        return isLeapYear(year) ? 29 : 28;

    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;

    default:
        return 30;
    }
}

void after45Days(date &D)
{
    D.day += 45 - getDaysInMonth(D.month, D.year);
    D.month += 1;
    if (D.month > 12)
    {
        D.month = 1;
        D.year++;
    }

    cout << D.day << '/' << D.month << '/' << D.year;
}

void getCurrDate(date &D)
{
    cout << "Please input date: ";
    cin >> D.day >> D.month >> D.year;
}

// Problem 8
struct card
{
    int suit; // 0: spade, 1: club, 2: diamond, 3: heart 
    int rank; // 0: Joker, 1: Ace, 2- 10, 11: Jack, 12: Queen, 13: King
};

void sortCard(card* &c)
{
    bool swapped = false;
    for(int i = 0; i < 5 - 1; i++)
    {
        for (int j = 0; j < 5 - i - 1; j++)
            if(c[j].rank > c[j + 1].rank)
            {
                swap(c[j], c[j + 1]);
                swapped = true;
            }

        if(swapped == false)
            return;
    }
}

bool isFiveOfAKind(card* c)
{
    if(c[0].rank != 0)
        return false;

    if(c[1].rank == c[4].rank && c[1].rank == 1)
        return true;

    return false;
}

bool isStraightFlush(card* c)
{
    for(int i = 0; i < 5 - 1; i++)
    {
        if(c[i + 1].rank - c[i].rank != 1)
            return false;

        if(c[i].suit != c[i + 1].suit)
            return false;
    }

    return true;
}

bool isFourOfAKind(card* c)
{
    if(c[0].rank == c[3].rank)
        return true;

    if(c[1].rank == c[4].rank)
        return true;

    return false;
}

bool isFullHouse(card* c)
{
    if(c[0].rank == c[2].rank && c[3].rank == c[4].rank)
        return true;

    if(c[0].rank == c[1].rank && c[2].rank == c[4].rank)
        return true;

    return false;
}

bool isFlush(card* c)
{
    for(int i = 0; i < 5 - 1; i++)
        if(c[i].suit != c[i + 1].suit)
            return false;

    return true;
}

bool isStraight(card* c)
{
    for(int i = 0; i < 5 - 1; i++)
        if((c[i + 1].rank - c[i].rank != 1) || (c[i].rank == 0))
            return false;

    return true;
}

bool isThreeOdfAKind(card* c)
{
    if(c[0].rank == c[2].rank)
        return true;

    if(c[2].rank == c[4].rank)
        return true;

    return false;
}

bool isTwoPairs(card* c)
{
    if(c[0].rank == c[1].rank)
        if(c[2].rank == c[3].rank || c[3].rank == c[4].rank)
            return true;

    else
        if(c[1].rank == c[2].rank && c[3].rank == c[4].rank)
            return true;

    return false;
}

bool isOnePair(card* c)
{
    for(int i = 0; i < 5 - 1; i++)
        if(c[i].rank == c[i + 1].rank)
            return true;

    return false;
}

void typeHighRanking(card*& c)
{

    if(isFiveOfAKind(c))
        cout << "Five of a kind";
    
    else if(isStraightFlush(c))
        cout << "Straight flush";

    else if(isFourOfAKind(c))
        cout << "Four of a kind";

    else if(isFullHouse(c))
        cout << "Full house";

    else if(isFlush(c))
        cout << "Flush";

    else if(isStraight(c))
        cout << "Straight";

    else if(isThreeOdfAKind(c))
        cout << "Three of a kind";

    else if(isTwoPairs(c))
        cout << "Two pairs";

    else if(isOnePair(c))
        cout << "One pair";

    else cout << "High card";

    return;
}

// Assume that 5 cards are valid
void get5cards(card* &c)
{
    for (int i = 0; i < 5; i++)
    {
        cout << "Input card " << i + 1 << " (suit, rank): ";
        cin >> c[i].suit >> c[i].rank;
    }

    sortCard(c);
    cout << endl;
    typeHighRanking(c);
}

int main()
{
    card* c = new card[5];
    get5cards(c);
    delete[] c;
    return 0;
}
