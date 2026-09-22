#include <iostream>

using namespace std;

int main()
{
    int n, tonguoc = 0, souoc = 0;
    cout << "Nhap vao so nguyen duong n: ";
    cin >> n;
    cout << "Uoc so cua n: ";
    for (int i = 1; i <= n; ++i)
    {
        if (n % i == 0)
        {
            cout << i << " ";
            ++souoc;
            tonguoc += i;
        }
    }

    cout << endl;
    cout << "Co " << souoc << " uoc so" << endl;
    cout << "Tong uoc so bang " << tonguoc;

    return 0;
}
