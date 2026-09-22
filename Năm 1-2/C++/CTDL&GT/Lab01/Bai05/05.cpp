#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int countLines(string filename)
{
    int n = 0;
    ifstream fin(filename);
    if (!fin.is_open())
    {
        cout << "File doesn't exit! Please try again!";
        return 0;
    }

    while (!fin.eof())
    {
        string s = "";
        getline(fin, s);
        n++;
    }

    fin.close();

    return n;
}

int main()
{
    int a, n;
    string filename;

    cin >> a >> n;
    getline(cin, filename);

    ifstream fin(filename);
    if (!fin.is_open())
    {
        cout << "File doesn't exit! Please try again!";
        return 0;
    }

    long long count = 0;

    if (a == 0)
    {
        while (!fin.eof())
        {
            string s = "";
            getline(fin, s);
            count++;
            cout << s << endl;
            if (count == n)
                break;
        }
    }

    else if (a == 1)
    {
        long long numLines = countLines(filename);
        while (!fin.eof())
        {
            string s = "";
            getline(fin, s);
            count++;
            if (count > numLines - n)
                cout << s << endl;
        }
    }

    fin.close();
}