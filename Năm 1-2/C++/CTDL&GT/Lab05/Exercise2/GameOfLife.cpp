#include <iostream>
#include <fstream>

using namespace std;

void readFile(int initial[12][12])
{
    ifstream fin("InitialState.txt");
    if (!fin.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (i == 0 || i == 11 || j == 0 || j == 11 || (i == 0 && j == 0) || (i == 11 && j == 11))
                initial[i][j] = 0;

            else
                fin >> initial[i][j];
        }
    }

    fin.close();
}

void printArray(int cell[][12])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
            cout << cell[i][j] << " ";

        cout << endl;
    }

    cout << endl;
}

void copyArray(int copied[][12], int initial[][12], int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            copied[i][j] = initial[i][j];
}

void countSurroundingFriend(int initial[][12], int cell[][12])
{
    for (int i = 1; i <= 10; i++)
    {
        for (int j = 1; j <= 10; j++)
        {
            cell[i - 1][j - 1] = initial[i - 1][j - 1] + initial[i - 1][j] + initial[i - 1][j + 1];
            cell[i - 1][j - 1] += initial[i][j - 1] + initial[i][j + 1];
            cell[i - 1][j - 1] += initial[i + 1][j - 1] + initial[i + 1][j] + initial[i + 1][j + 1];
        }
    }
}

bool isAllDead(int cell[][12], int initial[][12])
{
    int cnt0 = 0, cnt1 = 0;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (cell[i][j] == 0 || (cell[i][j] == 2 && initial[i + 1][j + 1] == 0))
                cnt0++;

            else if (cell[i][j] == 1)
                cnt1++;
        }
    }

    return (cnt0 + cnt1 == 100);
}

bool isTheSameCell(int initialCell[][12], int cell[][12])
{
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            if (initialCell[i][j] != cell[i][j])
                return false;

    return true;
}

bool isAvailable(int form[][12][12], int cell[][12], int cnt)
{
    for (int k = 0; k < cnt; k++)
    {
        if (isTheSameCell(form[k], cell))
            return true;
    }

    return false;
}

void updateFormOfCell(int form[][12][12], int cell[][12], int &cnt)
{
    copyArray(form[cnt], cell, 10);
    cnt++;
}

void update(int initial[12][12], int tempInitial[][12], int cell[][12])
{
    copyArray(tempInitial, initial, 12);
    countSurroundingFriend(initial, cell);
}

void findAllSubCell(int initial[12][12], int tempInitial[][12], int initialCell[][12], int cell[][12], int form[][12][12], int& cnt)
{
    if (isAllDead(cell, tempInitial))
    {
        cout << "ALL DEAD!\n";
        exit(0);
    }

    int temp[12][12] = {};

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (cell[i][j] < 2 || cell[i][j] > 3)
                temp[i + 1][j + 1] = 0;

            else if (cell[i][j] == 2)
            {
                if (tempInitial[i + 1][j + 1] == 0)
                    temp[i + 1][j + 1] = 0;

                else
                    temp[i + 1][j + 1] = 1;
            }

            else if (cell[i][j] == 3)
                temp[i + 1][j + 1] = 1;
        }
    }

    update(temp, tempInitial, cell);

    if (isAvailable(form, cell, cnt))
        exit(0);

    printArray(cell);
    updateFormOfCell(form, cell, cnt);
    findAllSubCell(temp, tempInitial, initialCell, cell, form, cnt);
}

int main()
{
    int initial[12][12] = {};
    int tempInitial[12][12] = {};
    int initialCell[12][12] = {};
    int cell[12][12] = {};
    int form[30][12][12] = {};
    int cnt = 0;

    readFile(initial);

    copyArray(tempInitial, initial, 12);
    countSurroundingFriend(initial, initialCell);
    copyArray(cell, initialCell, 10);
    updateFormOfCell(form, cell, cnt);

    if (isAllDead(cell, initial))
    {
        cout << "ALL DEAD!\n";
        return 0;
    }

    printArray(initialCell);

    findAllSubCell(initial, tempInitial, initialCell, cell, form, cnt);

    printArray(initialCell);
}