#include <iostream>

using namespace std;

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

int countArrayPrimre(int A[], int n)