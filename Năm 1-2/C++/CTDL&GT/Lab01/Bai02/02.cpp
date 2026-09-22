#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <string>

using namespace std;

struct fraction
{
    int a;
    int b;
};

int gcd(int a, int b)
{
    if(b == 0)
        return a;
    
    return gcd(b, a % b);
}

void reduceFraction(int &a, int &b)
{
    int GCD = gcd(a, b);
    a /= GCD;
    b /= GCD;
}

int main()
{
    ifstream fin("input1.2.txt");
    ofstream fout("output1.2.txt");
    if (!fin.is_open() || !fout.is_open())
    {
        cout << "File could not be opened!";
        return 0;
    }

    int n, m, num = 0;
    fraction *f = new fraction[1000];

    fin >> n >> m;
    while (!fin.eof())
    {
        stringstream ss;
        string temp;
        char c;
        fin >> temp;

        ss << temp;

        ss >> f[num].a;
        ss >> c;
        ss >> f[num].b;

        reduceFraction(f[num].a, f[num].b);
        num++;
    }
    num = 0;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            fout << f[num].a << '/' << f[num].b << ' ';
            num++;
        }
        fout << endl; 
    }

    fin.close();
    fout.close();

    delete[] f;
}