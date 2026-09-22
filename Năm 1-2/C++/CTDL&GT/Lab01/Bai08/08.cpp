#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <stack>

using namespace std;

bool isPalindrome(string str)
{
    stack<char> s;
    string temp = "";
    string STR = "";

    for (int i = 0; i < str.size(); i++)
    {
        if(isalpha(str[i]))
            str[i] = tolower(str[i]);
        if(isalnum(str[i]))
        {
            s.push(str[i]);
            STR += str[i];
        }
    }

    while (!s.empty())
    {
        temp += s.top();
        s.pop();
    }


    return STR == temp;
}

int main()
{
    ifstream fin("input1.8.1.txt");

    if (!fin.is_open())
    {
        cout << "File could not be opened!";
        return 0;
    }

    int n = 1;
    while (!fin.eof())
    {
        string s = "";
        getline(fin, s);

        cout << n << ". ";
        if (isPalindrome(s))
            cout << "It is a palindrome!\n";

        else
            cout << "It is not a palindrome!\n";

        n++;
    }

    fin.close();
}