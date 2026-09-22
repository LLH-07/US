#include <iostream>
#include <cmath>

using namespace std;

void setArray(int a[], int &n)
{
    cout << "Nhap vao so luong phan tu: ";
    cin >> n;
    for(int i = 0; i < n; i++)
    {
        cout << "Nhap vao a[" << i << "] : ";
        cin >> a[i];
    }
}

void printArray(int a[], int n)
{
    for(int i = 0; i < n; i++)
    {
        cout << "a[" << i << "] = " << a[i];
    }
}

int sumArray(int a[], int n)
{
    int sum = 0;
    for(int i = 0; i < n; i++) sum += a[i];
    return sum;
}

int findMin(int a[], int n)
{
    int mn = 1000000000;
    for(int i = 0; i < n; i++)
    {
        if(mn > a[i]) mn = a[i];
    }
    return mn;
}

int checkPrime(int n)
{
    if(n < 2) return 0;
    for(int i = 2; i <= sqrt(n); i++)
    {
        if(n % i == 0) return 0;
    }
    return 1;
}

int countNumberofPrime(int a[], int n)
{
    int prime = 0;
    for(int i = 0; i < n; i++)
    {
        if(checkPrime(a[i]) == 1) ++prime;
    }
    return prime;
}

int isAscendingOrder(int a[], int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        if (a[i] > a[i + 1]) return 0;
    }
    return 1;
}

int countOccurrences(int a[], int n, int x)
{
    int count = 0;
    for(int i = 0; i < n; i++)
    {
        if(a[i] == x) ++count;
    }
    return count;
}

void deleteItems(int a[], int &n, int x)
{
    int count = 0;
    for(int i = 0; i < n; i++)
    {
        if(a[i] == x)
        {
            ++count;
            for(int j = i; j < n - count; j++)
            {
                a[j] = a[j + 1];
            }
            --i;
        }
    }
    n -= count;
    for(int i = 0; i < n; i++) cout << a[i] << " ";
}

void deleteItemsFromIndex(int a[], int &n, int index, int no_items)
{
    for (int i = index; i < index + no_items; i++) a[i] = 0;
    deleteItems(a,n,0);
}

void concatTwoArrays(int a[], int n, int b[], int m, int c[])
{
    for(int i = 0; i < n; i++) c[i] = a[i];
    for(int i = n; i < m + n; i++) c[i] = b[i];
}

void concatInterspersedlyTwoArray(int a[], int n, int b[], int m, int c[])
{
    for(int i = 0; i < m + n; i+=2)
    {
        c[i] = a[i];
        c[i + 1] = b[i];
    }
}

int main()
{
    int n, a[10000];
    cin >> n;
}