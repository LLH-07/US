#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    int n;
    float a[100], b[100];
    char ope[100][1];

    ifstream fin;
    ofstream fout;

    fin.open("input1.txt");
    fout.open("output1.txt");

    if ( ( !fin.is_open() ) || ( !fout.is_open() ) )
    {
        cout << "File couldn't be opened!";
        return 0;
    }

    fin >> n;
    fout << n << endl << "******" << endl << endl;

    for ( int i = 0 ; i < n; i++ )
    {
        fin >> a[i];
        fin.ignore();
        fin >> ope[i];
        fin >> b[i];
        
        if ( ope[i][0] == '+' )
            fout << a[i] + b[i] << endl;
        
        else if ( ope[i][0] == '-' )
            fout << a[i] - b[i] << endl;

        else if ( ope[i][0] == '*' )
            fout << a[i] * b[i] << endl;

        else if ( ope[i][0] == '/' )
            fout << a[i] / b[i] << endl;

        else
            fout << ope[i] << endl;
    }
        
    fin.close();
    fout.close();
}