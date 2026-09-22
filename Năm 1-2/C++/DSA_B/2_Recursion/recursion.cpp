#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

int calculateSum(int n)
{
    if (n == 1)
        return 1;
    return n + calculateSum(n - 1);
}

int calculateFactorial(int n)
{
    if (n == 1)
        return 1;
    return n * calculateFactorial(n - 1);
}

int calculatePower(int x, int n)
{
    if (n == 0)
        return 1;
    return x * calculatePower(x, n - 1);
}

int countDigit(unsigned int n)
{
    if (n < 10)
        return 1;
    return 1 + countDigit(n / 10);
}

bool isAllEven(unsigned int n)
{
    if (n == 0)
        return true;
    if (n % 2 != 0)
        return false;
    return isAllEven(n / 10);
}

int countCommonDivisor(int a, int b, int div = 1)
{
    if (div > min(a, b))
        return 0;

    if (a % div == 0 && b % div == 0)
        return countCommonDivisor(a, b, div + 1) + 1;

    return countCommonDivisor(a, b, div + 1);
}

int gcd(int a, int b)
{
    if(b == 0) return a;
    return gcd(b, a % b);
}

int lcm(int a, int b)
{
    return a * b / gcd(a, b);
}

int reverseInteger(int n, int reversed = 0)
{
    if(n == 0) return reversed;
    reversed *= 10;
    reversed += n % 10;
    n /= 10;
    return reverseInteger(n, reversed);
}

int calculateFibonacci(int pos)
{
    if(pos < 2) return pos;
    return calculateFibonacci(pos - 1) + calculateFibonacci(pos - 2);
}

void printPermutation(string str, int start = 0, int end = 3)
{
    if(start == end)
        cout << str << endl;

    for(int i = start; i <= end; i++)
    {
        swap(str[start], str[i]);

        printPermutation(str, start + 1, end);

        swap(str[start], str[i]);
    }
}

int main()
{
    printPermutation("ABCD");
    return 0;
}