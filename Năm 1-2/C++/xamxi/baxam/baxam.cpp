#include <iostream>

using namespace std;

int Col[10000];
int plusDiagonal[10000], minusDiagonal[10000];
int Position[10000];
int size_Board;
int dong, cot;
int way = 0;

string Lower(string ans)
{
    for( int i = 0; i < ans.size(); i++ )
        if( ans[i] >= 'A' && ans[i] <= 'Z' )
           ans[i] = char(ans[i] + 32);

    return ans;
}

void Initiate()
{
    Col[10000] = {1}; // can be placed
    plusDiagonal[10000] = {1};
    minusDiagonal[10000] = {1};
    Position[10000] = {-1}; // Can't determine position in column
}

void Input()
{
    cout << "Position of the first queen:\n";
    cout << "Row: ";
    cin >> dong;
    cout << "Column: ";
    cin >> cot;

}

void afterPlacingQueen()
{
    string ans;
    cout << "\nContinue? Yes / No";
    cout << "\nYour answer: ";
    cin >> ans;
    if( Lower(ans) == "yes" )
    {
        Initiate();
        Input();
    }

    else
    {
        cout << "Thank you very much!\n";
        cout << "See you again!";
        return;
    }
}

void placeQueen(int& i, int& j)
{
    Position[i] = j;
    Col[j] = 0;
    minusDiagonal[i - j + 7] = 0;
    plusDiagonal[i + j] = 0;
}

void Try(int i)
{
    int j = 0;
    while(j < 8)
    {
        if(Col[i] && minusDiagonal[i - j + 7] && plusDiagonal[i + j])
        {
            placeQueen(i, j);
            if(i < size_Board)
            {
                    Try(i + 1);
            }
            else ++way;
            
            Col[i] = 1;
            minusDiagonal[i - j + 7] = 1;
            plusDiagonal[i + j] = 1;
            Position[i] = -1;
        }

        else ++j;
    }
}

int main()
{
    Initiate();
    cout << "Board's size n x n: ";
    cin >> size_Board;
    Input();
    placeQueen(dong, cot);
    Try(0);
    return 0;
}