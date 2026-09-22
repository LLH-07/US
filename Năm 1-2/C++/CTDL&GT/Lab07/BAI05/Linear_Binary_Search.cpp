#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <iomanip>
#include <algorithm>

using namespace std;

void readFile(string names[], int &n)
{
    ifstream in("50k.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        return;
    }

    while (!in.eof())
    {
        in >> names[n];
        n++;
    }

    in.close();
}

int linearSearch(string names[], int n, string name)
{
    for (int i = 0; i < n; i++)
        if (name == names[i])
            return 1;

    return 0;
}

int binarySearch(string names[], int n, string name)
{
    int left = 0, right = n - 1;
    int mid;

    while (left <= right)
    {
        mid = left + (right - left) / 2;

        if (name < names[mid])
            right = mid - 1;
        else if (name > names[mid])
            left = mid + 1;
        else
            return 1;
    }

    return 0;
}

int main()
{
    string names[50001];
    int n = 0, temp;
    readFile(names, n);

    string foundName = "Yuji";
    string unfound = "Phoon";

    clock_t start, end;
    double usedTime1, usedTime2;

    // Linear search
    start = clock();
    temp = linearSearch(names, n, foundName);
    end = clock();
    usedTime1 = double(end - start) / CLOCKS_PER_SEC;

    // Binary search
    sort(names, names + n);
    start = clock();
    temp = binarySearch(names, n, foundName);
    end = clock();
    usedTime2 = double(end - start) / CLOCKS_PER_SEC;

    // Time run
    cout << fixed << setprecision(3);
    cout << "Running time(ms) of linear search and binary search if name exists are\n"
         << usedTime1 * 1000.0 << endl
         << usedTime2 * 1000.0 << endl;


    // Linear search
    start = clock();
    temp = linearSearch(names, n, unfound);
    end = clock();
    usedTime1 = double(end - start) / CLOCKS_PER_SEC;

    // Binary search
    sort(names, names + n);
    start = clock();
    temp = binarySearch(names, n, unfound);
    end = clock();
    usedTime2 = double(end - start) / CLOCKS_PER_SEC;

    // Time run
    cout << fixed << setprecision(3);
    cout << "Running time(ms) of linear search and binary search if name is not existed are\n"
         << usedTime1 * 1000.0 << endl
         << usedTime2 * 1000.0 << endl;

    return 0;
}