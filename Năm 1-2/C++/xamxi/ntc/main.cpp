#include <iostream>
#include <string.h>
#include <cstring>

using namespace std;

int dot[100][100] = {};
int x[1000] = {}, y[1000] = {};
int step = 0;

void Input(int& row, int& col, char a[][100])
{
    cout << "How many rows? ";
    cin >> row;

    cout << "How many column? ";
    cin >> col;

    cout << "Input characters arrays without space: \n";

    cin.ignore();
    for ( int i = 0; i < row; i++ )
    {
        gets(a[i]);
        if ( strlen(a[i]) != col )
        {
            cout << "Column is invalid!";
            return;
        }
    }

}

//PROBLEM 1

void checkDot(int row, int col, char a[][100])
{
    for ( int i = 0; i < row; i++ )
       for ( int j = 0; j < col; j++ )
            if ( a[i][j] == '.' )
                dot[i][j] = 1;
}

void saveDot(int row, int col, char a[][100])
{
    int count = 0;
    for ( int i = 0; i < row; i++ )
        for ( int j = 0; j < col; j++ )
            if ( a[i][j] == '.' )
            {
                ++count;
                x[count] = i;
                y[count] = j;
            }
}

void printPositionOfDot(int row, int col)
{
    cout << "Two positions of dot are:\n";
    for ( int i = 0; i < row; i++ )
        for ( int j = 0 ; j < col; j++)
            if ( dot[i][j] == 1 ) cout << i << " " << j << endl;
}

//PROBLEM 2

void checkMove(int& i, int& j, char a[][100], int posDot[][100], int& cont)
{
    //Move right
    if (( a[i][j + 1] == '*' ) && ( posDot[i][j + 1] == 0 ))
    {
        x[0] = i;
        y[0] = j;
        j = j + 1;
        posDot[i][j] = 1;
        cont = 1;
    }

    //Move left
    else if (( a[i][j - 1] == '*' ) && ( posDot[i][j - 1] == 0 ))
    {
        x[0] = i;
        y[0] = j;
        j = j - 1;
        posDot[i][j] = 1;
        cont = 1;
    }

    //Move up
    else if (( a[i - 1][j] == '*' ) && ( posDot[i - 1][j] == 0 ))
    {
        x[0] = i;
        y[0] = j;
        i = i - 1;
        posDot[i][j] = 1;
        cont = 1;
    }

    //Move down
    else if(( a[i + 1][j] == '*' ) && ( posDot[i + 1][j] == 0 ))
    {
        x[0] = i;
        y[0] = j;
        i = i + 1;
        posDot[i][j] = 1;
        cont = 1;
    }
}

void Move(int row, int col, char a[][100])
{
    int posDot[100][100] = {};
    int i, j;
    int cont = 0;
    int times = 0;

    i = x[1];
    j = y[1];

    posDot[i][j] = 1;
    checkMove(i, j, a, posDot, cont);

    if(cont == 1) ++step;

    while ( dot[i][j] != 1 )
    {
        cont = 0;
        checkMove(i, j, a, posDot, cont);
        if(cont == 0)
        {
            i = x[0];
            j = y[0];
            ++times;
        }
        else if(cont == 1) ++step;

        if(times > 3) break;
    }
}

void printNumberOfSteps()
{
    cout << "Number of steps move from first dot to second dot:\n" << step << endl;
}

int main()
{
    int row, col;
    char a[100][100];

    Input(row, col, a);
    checkDot(row, col, a);
    saveDot(row, col, a);
    printPositionOfDot(row, col);
    Move(row, col, a);
    printNumberOfSteps();

    return 0;
}
/*
0123456
#######
#*  **.
#   * #
#* ** #
# ** *#
##.####
*/