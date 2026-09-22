#include <iostream>
#include <cstring>
#include <string>

using namespace std;

string lastWord (string strArr[5])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = i + 1; j < 5; j++)
        {
            if(strArr[i] > strArr[j])
                swap(strArr[i], strArr[j]);
        }
    }

    return strArr[4];
}

int main()
{
    string s[5] = {"kiwi", "banana", "orange", "apple", "grapes"};
    cout << lastWord(s);
}