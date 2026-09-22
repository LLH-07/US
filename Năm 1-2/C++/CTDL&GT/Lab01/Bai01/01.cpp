#include <iostream>
#include <string.h>
#include <string>
#include <fstream>

using namespace std;

int countSentences(string s)
{
    int res = 0;
    for (int i = 0; i < s.length(); i++)
        if (s[i] == '.' || s[i] == '?' || s[i] == '!')
            res++;

    return res;
}

bool isAvailable(string words[], string word, int num)
{
    for (int i = 0; i < num; i++)
        if (word == words[i])
            return true;

    return false;
}

void countDistinctWords(string s, string words[], int& numWords)
{
    for (int i = 0; i < s.length() - 1; i++)
    {
        if (!isalnum(s[i]))
        {
            if (isalnum(s[i + 1]))
            {
                if (!isAvailable(words, words[numWords], numWords))
                    numWords++;

                else
                    words[numWords] = "";
            }
        }
        else
            words[numWords] += s[i];
    }
}

int main()
{
    ifstream fin("input1.1.txt");
    if (!fin.is_open())
    {
        cout << "File could not be opened!";
        return 0;
    }

    string words[1000] = {};
    int sentences = 0, numWords = 0;

    while (!fin.eof())
    {
        string s = "";
        getline(fin, s);
        sentences += countSentences(s);
        countDistinctWords(s, words, numWords);
    }

    fin.close();

    cout << "Number of sentences: " << sentences << endl;
    cout << "Number of distinct words: " << numWords << endl;
}