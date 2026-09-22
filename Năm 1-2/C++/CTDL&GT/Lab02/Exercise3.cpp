#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

void convert2DTo1D(int a[][100], int n, int *&b)
{
    int cnt = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            b[cnt] = a[i][j];
            cnt++;
        }
    }
}

void get2d(int a[][100], int n)
{
    int cnt = 1;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i][j] = cnt;
            cnt++;
        }
    }
}

void convertToSpiralOrder(int a[][100], int n)
{
    int si = 0, ei = n - 1;
    int sj = 0, ej = n - 1;
    int *b = new int[n * n];
    int k = 0;

    convert2DTo1D(a, n, b);
    sort(b, b + n * n);

    while (k < n * n)
    {
        // left to right
        for (int j = sj; j <= ej; j++)
            if (k < n * n)
            {
                a[si][j] = b[k];
                k++;
            }

        // top to bottom
        for (int i = si + 1; i <= ei; i++)
            if (k < n * n)
            {
                a[i][ej] = b[k];
                k++;
            }

        // right to left
        for (int j = ej - 1; j >= sj; j--)
            if (k < n * n)
            {
                a[ei][j] = b[k];
                k++;
            }

        // bottom to top
        for (int i = ei - 1; i > si; i--)
            if (k < n * n)
            {
                a[i][sj] = b[k];
                k++;
            }

        si++;
        sj++;
        ei--;
        ej--;
    }

    delete[] b;
}

void print2DArray(int a[][100], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            cout << a[i][j] << ' ';

        cout << endl;
    }
}

int main()
{
    int a[100][100];
    int n = 4;
    get2d(a, n);
    print2DArray(a, n);

    cout << endl;
    convertToSpiralOrder(a, n);
    print2DArray(a, n);
}
