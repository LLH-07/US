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

void CountPrime(int n, int a[])
{
    int count = 0; 
    cout << "Nhap vao so luong phan tu: ";
    cin >> n;
    for(int i = 0; i < n; i++)
    {
        cout << "a[" << i <<"] : ";
        cin >> a[i];
        if(snt(a[i]) == 1) ++count;
    }
    cout << count;
}
int main()
{
    int n, a[10000];
    CountPrime(n,a);
    return 0;
}