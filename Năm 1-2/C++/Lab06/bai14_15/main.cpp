#include <iostream>
#include <fstream>

using namespace std;

void sortName(string s[1000], int count)
{   
    for ( int k = 0; k < count - 1; k++ )
    {
        for ( int i = 0; i < count - k - 1; i++ )
        {
            int lenMin = min( s[i].length(), s[i + 1].length() );
            int j = 0;
            if ( tolower(s[i][j]) > tolower(s[i + 1][j]) )
                swap(s[i], s[i + 1]);

            else if (s[i][j] == s[i + 1][j] )
            {
                while ( tolower(s[i][j]) <= tolower(s[i + 1][j]) && j < lenMin )
                    j++;

                if ( tolower(s[i][j]) > tolower(s[i + 1][j]) )
                    swap(s[i], s[i + 1]);
            }
        }
    }
}


int main()
{
    ifstream fin;
    ofstream fout;

    fin.open("input.txt");
    fout.open("output.txt");

    if ( ( !fin.is_open() ) || ( !fout.is_open() ) )
    {
        cout << "File could not be opened!";
        return 0;
    }

    int count = 0;
    string s[1000];
    while ( !fin.eof() )
    {
        getline(fin, s[count]);
        count++;
    }

    sortName(s, count);
    
    for ( int i = 0; i < count; i++ )
        fout << s[i] << endl;

    fin.close();
    fout.close();
}