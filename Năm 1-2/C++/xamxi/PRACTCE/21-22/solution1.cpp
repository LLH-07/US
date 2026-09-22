#include <iostream>
#include <string.h>

using namespace std;

bool isAlpha(char &c)
{
    c = tolower(c);
    if (c >= 'a' && c <= 'z')
        return true;

    return false;
}

void getWord(string sentence, string words[], int &num)
{
    int len = sentence.length();
    int i = 0;
    num = 0;
    while (sentence[i] == ' ')
        i++;

    for (; i < len - 1; i++)
    {
        if (isAlpha(sentence[i]))
            words[num] += sentence[i];

        if (sentence[i] == ' ' && isAlpha(sentence[i + 1]))
            num++;
    }

    if(isAlpha(sentence[i]))
        words[num] += sentence[i];

    num++;
}

bool question1(string s)
{
    string words[1000] = {};
    int num = 0;
    getWord(s, words, num);

    for(int i = 0; i < num / 2; i++)
        if(words[i] != words[num - 1 - i])
            return false;

    return true;
}

int main()
{
    string s = "Information school graduate seeks graduate school information.";
    string s1 = "Herb the sage eats sage, the herb.";
    if(question1(s1))
        cout << 1;

    else cout << 0;
    return 0;
}