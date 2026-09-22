#include <iostream>
#include <cmath>

using namespace std;

int snt(int num)
{
    if(num < 2) return 0;
    for(int i = 2; i <= sqrt(num); i++)
    {
        if(num % i == 0) return 0;
    }
    return 1;
}

int findXfirst(int n, int a[], int x)
{
    for (int i = 0; i < n; ++i)
    {
        if(a[i] == x)
        {
            return i;
        }
    }
    return -1;
}

int findXlast(int n, int a[], int x)
{
    int mark = -1;
    for(int i = 0; i  < n; i++)
    {
        if(a[i] == x)
        {
            mark = i;
        }
    }
    return mark;
}

int findXmax(int n, int a[])
{
    int mx = a[0];
    for(int i = 1; i < n; i++) 
    {
        if(mx < a[i])
        {
            mx = a[i];
        }
    }
    return mx;
}

int findMaxPrime(int n, int a[])
{
    int mx = 2, pos= -1;
    for(int i = 0; i < n; i++)
    {
        if(snt(a[i]) == 1)
        {
            if(a[i] > mx )
            {
                mx = a[i];
                pos = i;
            }
        }
    }
    return pos;
}

int main()
{
    int n, a[1000],x;
    cin >> n;
    for(int i = 0; i < n; i++) cin >> a[i];
    cout << "Max = " << findMaxPrime(n,a);
}