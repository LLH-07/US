#include <iostream>
#include <cmath>

using namespace std;

// Problem 1
// Calculate the sum of S = 1 + 2 + 3 + ... + n
int consecutiveSum(int n)
{
    if (n == 1)
        return n;

    return n + consecutiveSum(n - 1);
}

// Problem 2
// Calculate the factorial n! = 1 ∗ 2 ∗ 3 ∗ ... ∗ n
int factorial(int n)
{
    if (n == 1 || n == 2)
        return n;

    return n * factorial(n - 1);
}

// Problem 3
// Calculate x^n
double POW(double x, int n)
{
    if (n == 0)
        return 1;

    if (n < 0)
        return 1 / (POW(x, -n));

    return x * POW(x, n - 1);
}

// Problem 4
// Count the number of digits of a given integer
int countDigitsOf(int n)
{
    n = abs(n);

    if (n < 10)
        return 1;

    return 1 + countDigitsOf(n / 10);
}

// Problem 5
// Count the number of odd digits of a given integer
int countOddDigitsOf(int n)
{
    n = abs(n);

    if (n == 0)
        return 0;

    else if (n % 2 == 1)
        return 1 + countOddDigitsOf(n / 10);

    else
        return countOddDigitsOf(n / 10);
}

// Problem 6
// Verify if every digit of integer is even
bool isAllEven(int n)
{
    if (n % 2 == 1)
        return false;

    if (n == 0)
        return true;

    return isAllEven(n / 10);
}

// Problem 7
// Count the n umber of common divisor of 2 given integers
int countCommon(int a, int b, int i)
{
    if (i == 1 || i == 0)
        return i;

    if (a % i == 0 && b % i == 0)
        return 1 + countCommon(a, b, i - 1);

    countCommon(a, b, i - 1);
}

int countCommonDivisors(int a, int b)
{
    return countCommon(a, b, min(a, b));
}

// Problem 8
// Calculate the Greatest common divisor and Least common multiple of 2 given integers
int GCD(int a, int b)
{
    if (b == 0)
        return a;

    return GCD(b, a % b);
}

int LCM(int a, int b)
{
    return (a * b) / GCD(a, b);
}

// Problem 9
// Calculate the reverse value of a given integer
int reverse(int n)
{
    if (n < 10)
        return n;

    int lastDigit = n % 10;
    int numDigit = countDigitsOf(n);

    return lastDigit * POW(10, numDigit - 1) + reverse(n / 10);
}

// Problem 10
// Calculate the binary value of a given integer
long int decimalToBinary(long n)
{
    if (n == 0 || n == 1)
        return n;

    return n % 2 + 10 * decimalToBinary(n / 2);
}

// Problem 11
// Calculate the ith Fibonacci number
int Fibonacci(int n)
{
    if (n == 0 || n == 1)
        return n;

    return Fibonacci(n - 1) + Fibonacci(n - 2);
}

// Problem 12
// Given 4 single distinguish characters. Print put all possible permutation
void printPermutation(string s, int l, int r)
{
    if (l == r)
        cout << s << endl;

    else
    {
        for (int i = l; i <= r; i++)
        {
            swap(s[l], s[i]);

            printPermutation(s, l + 1, r);

            swap(s[l], s[i]);
        }
    }
}

// Problem 13
// Create a Singly linked list from a given array
struct Node
{
    int data;
    Node *next;
};

Node *createLinkedList(int *a, int n)
{
    if(n == 0)
        return NULL;

    Node* head = new Node;
    head->data = a[0];
    head->next = createLinkedList(a + 1, n -1);
    return head;
}

// Problem 14
// Find and remove the first node with a given value
Node* removeFirstValue(Node* &head, int val)
{
    if(head == NULL)
        return NULL;

    if(head->data == val)
    {
        Node* newHead = head->next;
        delete head;
        return newHead;
    } 

    head->next = removeFirstValue(head->next, val);
    return head; // if there aren't any Node have data = val
}

int main()
{
    return 0;
}