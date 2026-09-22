#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>

using namespace std;

struct day
{
    double temperature[12];
};

double calculateAverage(double a[], int n)
{
    double ave = 0;
    for (int i = 0; i < n; i++)
        ave += a[i];

    return ave / n;
}

double MAX(double a[], int n)
{
    double res = a[0];
    for (int i = 1; i < n; i++)
        res = max(res, a[i]);

    return res;
}

double MIN(double a[], int n)
{
    double res = a[0];
    for (int i = 1; i < n; i++)
        res = min(res, a[i]);

    return res;
}

int main()
{
    ifstream fin("input1.3.txt");
    ofstream fout("output1.3.txt");

    if (!fin.is_open() || !fout.is_open())
    {
        cout << "File could not be opened!";
        return 0;
    }

    int n, num = 0;
    day d[1000];
    double ave[10000], minimum[1000], maximum[1000];

    fin >> n;
    while (!fin.eof())
    {
        for (int i = 0; i < 12; i++)
            fin >> d[num].temperature[i];

        ave[num] = calculateAverage(d[num].temperature, 12);
        minimum[num] = MIN(d[num].temperature, 12);
        maximum[num] = MAX(d[num].temperature, 12);
        num++;
    }

    for(int i = 0; i < num; i++)
        fout << ave[i] << ' ' << maximum[i] << ' ' << minimum[i] << endl;

    fin.close();
    fout.close();

    return 0;
}