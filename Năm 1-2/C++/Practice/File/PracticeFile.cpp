#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

// Problem 1
void writeFile()
{
    ofstream out("Snowball.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    out << "i made my self a snowball\n";
    out << "as perfect as could be\n";
    out << "i thought i would keep it as a pet\n";
    out << "and let it sleep with me\n";
    out << "i made it some pajamas\n";
    out << "and a pillow for its head\n";
    out << "then last night it ran away\n";
    out << "but first it wet the bed";

    out.close();
}

// Problem 2
bool isAlpha(char ch)
{
    ch = tolower(ch);
    if (ch >= 'a' && ch <= 'z')
        return true;

    return false;
}

void getSentences(string sentence[], int &numS)
{
    ifstream in("Snowball.txt");

    if (!in.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    numS = 0;

    while (!in.eof())
    {
        getline(in, sentence[numS]);
        numS++;
    }

    in.close();
}

void getWord(string sentence, string word[], int &numW)
{
    int len = sentence.length();

    for (int i = 0; i < len; i++)
    {
        bool have = false;
        while (isAlpha(sentence[i]) && i < len)
        {
            have = true;
            word[numW] += sentence[i];
            i++;
        }

        if (have)
            numW++;
    }
}

void getWord(string sentence[], string word[], int &numW, int &numS)
{
    getSentences(sentence, numS);
    numW = 0;
    for (int i = 0; i < numS; i++)
        getWord(sentence[i], word, numW);
}

int countFreq(string word[], int numW, int freq[])
{
    sort(word, word + numW);
    int dif = 0;
    for (int i = 0; i < numW - 1; i++)
    {
        if (word[i] == word[i + 1])
            freq[dif]++;

        else
            dif++;
    }

    return dif + 1;
}

void saveFreq()
{
    ofstream out("Freq.txt");

    string sentence[100];
    string word[100];
    int numW, numS;
    int freq[100];

    for (int i = 0; i < 100; i++)
        freq[i] = 1;

    getWord(sentence, word, numW, numS);
    int dif = countFreq(word, numW, freq);
    int j = 0;

    for (int i = 0; i < dif; i++)
    {
        out << "Word: " << word[j] << '\n';
        out << "Frequency: " << freq[i] << " times" << endl << endl;
        j += freq[i];
    }

    out.close();
}

// Problem 3
void replaceIt(string sentence[], string word[], int &numW, int &numS)
{
    getWord(sentence, word, numW, numS);

    for(int i = 0; i < numW; i++)
        if(word[i] == "it")
            word[i] = "snowball";
}

int countWord(string sentence)
{
    int count = 0;
    if(sentence[0] != ' ')
        count++;
    
    for( int i = 0; i < sentence.size() - 1; i++)
    {
        if(sentence[i] == ' ' && sentence[i + 1] != ' ')
            count++;
    }

    return count;

}

void changeFile()
{
    string sentence[100];
    string word[100];
    int numW, numS;

    replaceIt(sentence, word, numW, numS);

    ofstream out("newSnowball.txt");

    int j = 0;
    int numWord = 0;

    for(int i = 0; i < numS; i++)
    {
        for(; j < numWord + countWord(sentence[i]); j++)
            out << word[j] << ' ';

        out << endl;

        numWord += countWord(sentence[i]);
    }

    out.close();
}

int main()
{
    writeFile();
    saveFreq();
    changeFile();
    return 0;

}