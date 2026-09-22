#include <iostream>
#include <string.h>
#include <cstring>
#include <cctype>
#include <iomanip>
#define MAX 1000

using namespace std;

void Input(char name[][MAX], int numvote[], int &n)
{
    cout << "Nhap vao so luong ung cu vien: ";
    cin >> n;
    cout << "Candidate\n";
    for(int i = 0; i < n; i++)
    {
        cin >> name[i];
    }

    cout << endl;
    cout <<"Votes\n";
    for(int i = 0; i < n; i++)
    {
        cin >> numvote[i];
    }
}

int totalArray(int numvote[], int n, int &total)
{
    for(int i = 0; i < n; i++)
    {
        total += numvote[i];
    }
}

float receivedPercent(int total, int numvotes)
{
    return (numvotes * 1.0 / total) * 100;
}

void outputPercent(int n, int numvote[], int total)
{
    float mx = -1;
    for(int i = 0; i < n; i++)
    {
        float phantram = receivedPercent(total, numvote[i]);
        cout << fixed << setprecision(2) << phantram << endl;
    }
}

int ChooseTheWinner(int numvote[], int n)
{
    int mx = -1,  pos;
    for(int i = 0; i < n; i++)
    {
        if(mx < numvote[i])
        {
            mx = numvote[i];
            pos = i;
        }
    }
    return pos;
}

int main()
{
    int n;
    char name[MAX][MAX];
    int numvote[MAX];
    int total = 0;
    Input(name, numvote, n);
    totalArray(numvote, n, total);

    cout << endl << "Total     " << total << endl;
    
    cout << "Received % of Total Vote" << endl;
    outputPercent(n, numvote, total);

    cout << "The Winner of the Election is ";
    int pos = ChooseTheWinner(numvote, n);
    cout << name[pos];
    return 0;
}