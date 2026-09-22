#include <iostream>

using namespace std;

void inputArr(int a[], int& n)
{
    cout << "Input : ";
    cin >> n;
    for(int i = 0; i < n; i++)
    { 
        cout << "a[" << i << "] = ";
        cin >> a[i];
    }
}

void printArr(int a[], int n)
{
    for(int i = 0; i < n; i++) cout << "a[" << i << "] = " << a[i] << endl; 
}

int main()
{
    int n, a[1000];
    inputArr(a,n);
    printArr(a,n);
    return 0;
}