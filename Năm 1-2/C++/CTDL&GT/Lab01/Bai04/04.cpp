#include <iostream>
#include <fstream>
#include <string.h>
#include <string>

using namespace std;

void getWordsOfLine(string s, string w[], int &n)
{
    int i = 0;
    while (s[i] == ' ')
        i++;

    for (; i < s.size() - 1; i++)
    {
        if (isalnum(s[i]))
            w[n] += s[i];

        else
        {
            if (isalnum(s[i + 1]))
                n++;
        }
    }

    if (isalnum(s[s.size() - 1]))
        w[n] += s[s.size() - 1];

    n++;
}

void compareLines(string line1[], string line2[], int n1, int n2, int numLine)
{
    int temp = -1, n = 0;
    string *temp1 = new string[n1];
    string *temp2 = new string[n2];

    for (int i = 0; i < min(n1, n2); i++)
    {
        if (line1[i] != line2[i])
        {
            temp = i;
            temp1[n] = line1[i];
            temp2[n] = line2[i];
            n++;
        }
    }

    if (temp == -1)
    {
        cout << "Done!" << endl;
        delete[] temp1;
        delete[] temp2;
        return;
    }

    cout << numLine << " // ";
    for (int i = 0; i < n; i++)
        cout << temp1[i] << ' ';

    if (n2 < n1)
        for (int i = temp + 1; i < n1; i++)
            cout << line1[i] << ' ';

    cout << endl;

    cout << numLine << " \\"
         << "\\ ";
    for (int i = 0; i < n; i++)
        cout << temp2[i] << ' ';

    if (n2 > n1)
        for (int i = temp + 1; i < n2; i++)
            cout << line2[i] << ' ';

    cout << endl;

    delete[] temp1;
    delete[] temp2;
}

int main()
{

    ifstream fin1("input1.4.1.txt");
    ifstream fin2("input1.4.2.txt");

    if (!fin1.is_open() || !fin2.is_open())
    {
        cout << "File could not be opened!";

        return 0;
    }

    int numLine = 0;
    while (!fin1.eof() && !fin2.eof())
    {
        numLine++;
        string s1 = "";
        string s2 = "";
        string *w1 = new string[1000];
        string *w2 = new string[1000];
        int n1 = 0, n2 = 0;

        getline(fin1, s1);
        getline(fin2, s2);

        getWordsOfLine(s1, w1, n1);
        getWordsOfLine(s2, w2, n2);

        compareLines(w1, w2, n1, n2, numLine);

        delete[] w1;
        delete[] w2;
    }

    return 0;
}