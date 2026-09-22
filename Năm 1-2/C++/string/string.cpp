#include <iostream>
#include <string>
#include <cstring>
#include <string.h>

using namespace std;

void getNum(string s, string& w, int& sum )
{
    int len = s.length();
    for(int i = 0; i < len; i++)
    {
        if(s[i] >= '0' && s[i] <= '9') w += s[i];
    }
    int len1 = w.length();
    int nhan = 1;
    for(int i = len1 - 1; i >= 0; i--)
    {
        sum += (w[i] - 48) * nhan;
        nhan *= 10;
    }
    if(len1 != 0) sum += 1;
}

int main()
{
    string s, w = "";
    int sum = 0;
    cout << "Nhap s: ";
    cin >> s;
    getNum(s, w, sum);
    cout << sum;
}
