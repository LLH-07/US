#include <iostream>

using namespace std;

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
    for(int i = 1; i < n; i++) if(mx < a[i]) mx = a[i];
    return mx;
}
int main()
{
    int n, a[1000],x;
    cout << "Nhap x, y: ";
    cin >> n >> x;
    for(int i = 0; i < n; i++) cin >> a[i];
}