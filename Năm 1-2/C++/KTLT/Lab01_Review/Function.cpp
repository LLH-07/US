#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>

using namespace std;

// FUNCTION

void LinearEquation(int a, int b)
{
    //ax + b = 0;
    if ( a == 0 )
    {
        if ( b == 0 )
        {
            cout << "Infinity";
            return;
        }

        cout << "No solution";
        return;
    }

    cout << "x = " << -( (b * 1.0) / a );
}

void QuadraticEquation(int a, int b, int c)
{
    //ax2 + bx + c = 0
    if( a == 0 )
    {
        LinearEquation(b, c);
        return;
    }

    double delta;
    delta = b * b - 4 * a * c;

    if ( delta < 0 )
        cout << "No solution";

    else if ( delta == 0 )
        cout << "x = " << - ( b / ( 2 * 1.0 * a ) );

    else
        cout << "x1 = " << ( ( - b - sqrt(delta) ) / ( 2 * a ) ) << endl
              << "x2 = " << ( ( - b + sqrt(delta) ) / ( 2 * a ) );
}

bool isPrime(int a)
{
    if ( a < 2 ) return false;

    for( int i = 2; i < sqrt(a); i++ )
        if ( a % i == 0 ) return false;

    return true;
}

int countPrime(int a, int b)
{
    int cnt = 0;
    for ( int i = a; i <= b; i++ )
        if ( isPrime(i) ) ++cnt;

    return cnt;
}

int sumDigits(int n)
{
    int sum =  0;

    while ( n != 0 )
    {
        sum += n % 10;
        n /= 10;

    }

    return sum;
}

int countInteger(int N, int k)
{
    int cnt = 0;

    for ( int i = 0; i < N; i++ )
        if ( i % k == 0 )
            ++cnt;

    return cnt;
}

void Triangle(float a, float b, float c)
{
    if ( ( a + b > c ) && ( a + c > b ) && ( b + c > a) )
    {
        if ( a == b && b == c )
            cout << "Equilateral triangle";

        else
        {
            if ( ( a == b ) || ( b == c ) || ( a == c ) )
            {
                if ( ( a * a + b * b == c * c ) || ( b * b + c * c == a * a ) || ( a * a + c * c == b * b ) )
                    cout << "Right isosceles triangle";

                else
                    cout << "Isosceles triangle";
            }

            else
            {
                 if ( ( a * a + b * b == c * c ) || ( b * b + c * c == a * a ) || ( a * a + c * c == b * b ) )
                    cout << "Right triangle";

                else
                    cout << "Normal triangle";
            }
        }

        return;
    }

    cout << "Can't form a triangle";

}

// 1D ARRAY

void inputArray(float A[], int n)
{
    for ( int i = 0; i < n; i++ )
        cin >> A[i];
}

void printArray(float A[], int n)
{
    for ( int i = 0; i < n; i++ )
        cout << A[i] << " ";
}

int countArrayPrime(int A[], int n)
{
    int cnt = 0;

    for ( int i = 0; i < n; i++ )
        if ( isPrime( A[i] ) )
            ++cnt;

    return cnt;
}

float sumArray(float A[], int n)
{
    float sum = 0;

    for ( int i = 0; i < n; i++ )
        sum += A[i];

    return sum;
}

bool isIncreasing(float A[], int n)
{
    for ( int i = 0; i < n - 1; i++ )
        if ( A[i] >= A[i + 1] )
            return false;

    return true;
}

bool isDecreasing(float A[], int n)
{
    for( int i = 0; i < n - 1; i++ )
        if ( A[i] <= A[i + 1] )
            return false;

    return true;
}

// 2D ARRAY

void input2DArray(float A[][100], int& m, int& n)
{
    cin >> m >> n;

    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < n; j++ )
            cin >> A[i][j];
}

void print2DArray(float A[][100], int m, int n)
{
    for ( int i = 0; i < m; i++ )
    {
        for ( int j = 0; j < n; j++ )
            cout << A[i][j] << " ";

        cout << endl;
    }

    cout << endl;
}


void rotate2DArray(float A[][100], int& m, int& n)
{
    int row, col;
    float a[100][100];

    row = n;
    col = m;

    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < n; j++ )
            a[j][m - i -1] = A[i][j];

    for( int i = 0; i < row; i++ )
        for ( int j = 0; j < col; j++ )
            A[i][j] = a[i][j];

    n = col;
    m = row;
}

void sum2DArray(float A[][100], float B[][100], float result[][100], int m, int n)
{
    for ( int i = 0; i < m; i++ )
        for (int j = 0; j < n; j++ )
            result[i][j] = A[i][j] + B[i][j];
}

float multiple1DArray(float a[], float b[], int n)
{
    float sum = 0;
    for ( int i  = 0; i < n; i++ )
        sum += a[i] * b[i];

    return sum;
}

void change2DArray(float b[][100], float temp[][100], int n, int p)
{
    // n is row -> col
    // p is col -> row
    for ( int i = 0; i < p; i++ )
        for ( int j = 0; j < n; j++ )
            temp[i][j] = b[j][i];
}

void multiple2DArray(float A[][100], float B[][100], float result[][100], int m, int n, int p)
{
    float temp[100][100];
    change2DArray(B, temp, n, p);

    for ( int i = 0; i < m; i++ )
        for (int j = 0; j < n; j++ )
            result[i][j] = multiple1DArray(A[i], temp[i], n);
}

bool isDiagonalMatrix(float A[][100], int m, int n)
{
    if(m != n) return false;

    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < n; j++ )
        {
            if ( ( i != j ) && ( A[i][j] != 0 ) )
                return false;

            else if ( ( i == j ) && ( A[i][j] == 0 ) )
                return false;
        }

    return true;
}
bool isUpperTriangleMatrix(float A[][100], int m, int n)
{
    if ( m != n ) return false;

    for ( int i = 0; i < m; i++ )
        for ( int j = i; i < n; j++ )
            if ( A[i][j] == 0 ) return false;

    return true;
}

bool isLowerTriangleMatrix(float A[][100], int m, int n)
{
    if (m != n) return false;

    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < i + 1; j++ )
            if ( A[i][j] == 0 ) return false;

    return true;
}

// STRING

void inputString(char C[100])
{
    gets(C);
}

void printString(char C[100])
{
    cout << C;
}

int countCapital(char C[100])
{
    int cnt = 0;
    int len = strlen(C);

    for ( int i = 0; i < len; i++ )
        if ( ( C[i] >= 'A' ) && ( C[i] <= 'Z' ) )
            ++cnt;

    return cnt;
}

int countAppearance(char C[100], char c)
{
    int cnt = 0;
    int len  = strlen(C);

    for ( int i = 0; i < len; i++ )
        if ( C[i] == c )
            ++cnt;

    return cnt;
}

int countWord(char C[100])
{
    int cnt = 0;
    int len  = strlen(C);
    int j = 0;

    while( C[j] == ' ' )
        j++;

    for ( int i = j; i < len; i++ )
        if ( ( C[i] == ' ' ) && ( C[i + 1] != ' ') )
            ++cnt;

    return cnt + 1;
}

// STRUCTURE

// Problem 1

struct Time
{
    int hours;
    int minutes;
    int seconds;
};

void inputPointOfTIme(Time& t)
{
    cout << "Hours: ";
    cin >> t.hours;

    cout << "Minutes: ";
    cin >> t.minutes;

    cout << "Second: ";
    cin >> t.seconds;

}

bool verifyPointOfTime(Time t)
{
    if ( ( t.hours < 0 ) || ( t.hours > 23 ) )
        return false;

    if ( ( t.minutes < 0 ) || ( t.minutes > 59 ) )
        return false;

    if ( ( t.seconds < 0 ) || ( t.seconds > 59 ) )
        return false;

    return true;
}

void timePassed(Time t, int min, int sec)
{
    min = ( t.hours * 60 ) + t.minutes;
    sec = t.seconds;
}

Time isEarlier(Time t1, Time t2)
{
    if ( t1.hours > t2.hours )
        return t2;

    else if ( t1.hours < t2.hours )
        return t1;

    if ( t1. minutes > t2.minutes )
        return t2;

    else if ( t1.minutes < t2.minutes )
        return t1;

    if ( t1.seconds > t2.seconds )
        return t2;

    else
        return t1;
}

void addingMinutes(Time& t, int x)
{
    t.minutes += x;

    while ( t.minutes > 59 )
    {
        t.minutes -= 60;
        t.hours++;
    }
}

void addingTime(Time& t, Time t1)
{
    t.seconds += t1.seconds;

    while ( t.seconds > 59 )
    {
        t.seconds -= 60;
        t.minutes++;
    }

    t.minutes += t1.minutes;

    while ( t.minutes > 59 )
    {
        t.minutes -= 60;
        t.hours++;
    }

    while ( t.hours > 23 )
        t.hours -= 24;

}

// Problem 2

struct Fraction
{
    int numerator; // tu
    int denominator; // mau
};

void inputFraction(Fraction& f)
{
    cin >> f.numerator;
    cin >> f.denominator;
}

bool verifyFraction(Fraction f)
{
    if ( f. denominator == 0)
        return false;

    return true;
}

int gcd(int a, int b)
{
    if ( b == 0 ) return a;
    return gcd(b, a % b);
}

int lcm(int a, int b)
{
    int GCD = gcd(a, b);
    return ( a * b ) / GCD;
}

void simplyFraction(Fraction& f)
{
    int GCD = gcd(f.numerator, f.denominator);
    f.numerator /= GCD;
    f.denominator /= GCD;
}

Fraction sum2Fraction(Fraction f1, Fraction f2)
{
    Fraction result;
    int LCM;

    LCM = lcm(f1.denominator, f2.denominator);
    result.denominator = LCM;
    result.numerator = ( LCM / f1.denominator ) * f1.numerator + ( LCM / f2.denominator ) * f2.numerator;

    return result;
}

Fraction compare2Fraction(Fraction f1, Fraction f2)
{
    int LCM;
    LCM = lcm(f1.denominator, f2.denominator);

    int tu1, tu2;
    tu1 = ( LCM / f1.denominator ) * f1.numerator;
    tu2 = ( LCM / f2.denominator ) * f2.numerator;

    if ( tu1 > tu2 )
        return f1;

    return f2;
}

// Problem 3
struct ToaDo
{
    float x;
    float y;
};

int Quadrant(ToaDo td)
{
    if ( td.y > 0 )
    {
        if ( td. x < 0 )
            return 1;
        
        return 2;
    }

    else
    {
        if ( td.x < 0 )
            return 3;

        return 4;
    }
}

float isDistance(ToaDo td1, ToaDo td2)
{
    float x, y;
    x = td1.x - td2.x;
    y = td1.y - td2.y;
    return sqrt( ( x * x ) -( y * y ) );
}

ToaDo isMidPoint(ToaDo td1, ToaDo td2)
{
    ToaDo result;
    
    result.x = ( td1.x + td2.x ) / 2;
    result.y = ( td1.y + td2.y ) / 2;

    return result;
}

bool isCollinear(ToaDo td1, ToaDo td2, ToaDo td3)
{
    float d1, d2, d3;
    d1 = isDistance(td1, td2);
    d2 = isDistance(td1, td3);
    d3 = isDistance(td2, td3);

    if ( ( d1 + d2 == d3 ) || ( d1 + d3 == d2 ) || ( d2 + d3 == d1 ) )
        return true;
    
    return false;
}

bool Iscollinear(ToaDo td1, ToaDo td2, ToaDo td3)
{
    ToaDo k1, k2;

    k1.x = td1.x - td2.x;
    k1.y = td1.y - td2.y;

    k2.x = td2.x - td3.x;
    k2.y = td2.y - td3.y;

    if ( k1.y * k2.x == k1.x * k2.y)
        return true;

    return false;
}

// FILE

// Problem 1

void resultOfEquation(int n, float a[], float b[], char ope[][1])
{
    ifstream fin;
    ofstream fout;

    fin.open("input1.txt");
    fout.open("output1.txt");

    if ( ( !fin.is_open() ) || ( !fout.is_open() ) )
    {
        cout << "File couldn't be opened!";
        return;
    }

    fin >> n;

    for ( int i = 0 ; i < n; i++ )
    {
        fin >> a[i];
        fin.ignore();
        fin >> ope[i];
        fin >> b[i];
        
        if ( ope[i][0] == '+' )
            fout << a[i] + b[i] << endl;
        
        else if ( ope[i][0] == '-' )
            fout << a[i] - b[i] << endl;

        else if ( ope[i][0] == '*' )
            fout << a[i] * b[i] << endl;

        else if ( ope[i][0] == '/' )
            fout << a[i] / b[i] << endl;

        else
            fout << ope[i] << endl;
    }
        
    fin.close();
    fout.close();
}

// Problem 2

void lower(string& str)
{
    int len = str.length();

    for ( int i = 0; i < len; i++ )
        if ( ( str[i] >= 'A' ) && ( str[i] <= 'Z' ) )
            str[i] += 32;

    
}

int isLetter(char c)
{
    if ( ( c >= 'a' ) && ( c <= 'z' ) )
        return 1;

    return 0;
}

int countAppearanceWord(string word)
{
    ifstream fin;
    fin.open("input2.txt");

    if ( !fin.is_open() )
    {
        cout << "File couldn't be opened!";
        return 0;
    }

    string sentence, temp;
    int cnt = 0;

    while ( !fin.eof() )
    {
        sentence = "";
        getline(fin, sentence);
        lower(sentence);

        int len = sentence.length();
        
        for ( int i = 0; i < len - 1; i++ )
        {
            temp = "";
            while ( isLetter(sentence[i]) == 1 && isLetter(sentence[i + 1]) == 1)
            {
                temp += sentence[i];
                i++;
            }

            if ( isLetter(sentence[i]) )
                temp += sentence[i];

            if ( temp == word ) cnt++; 
        }
    }

    fin.close();

    return cnt;
}

int main()
{
    char C[] = "Hello world!";
    cout << countAppearance(C, 'l');
}