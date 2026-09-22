#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

// Problem 19
//Given matrix a. Find the sub-matrix of a which satisfy keyboard input size and has the largest total value of its elements.
void prefixMatrix(int **a, int row, int col, int **prefix)
{
    for ( int i = 0; i < row; i++ )
        prefix[i][0] = a[i][0];

    for ( int j = 0; j < col; j++ )
        prefix[0][j] = a[0][j];

    for ( int i = 1; i < row; i++ )
        for ( int j = 1; j < col; j++ )
            prefix[i][j] = a[i][j] + prefix[i][j - 1] + prefix[i - 1][j] - prefix[i - 1][j - 1];

}

int** findSubMatrix(int** a, int length, int width, int &length_, int &width_)
{
    int **prefix = new int* [length];
    for ( int i = 0; i < length; i++ )
        prefix[i] = new int [width];

    prefixMatrix(a, length, width, prefix);

    int Largest = **a;
    int startR = 0, startC = 0;
    int m, n;

    for ( int i = 0; i < length; i++ )
    {
        m = i;
        for ( int j = 0; j < width; j++ )
        {
            n = j;

            for ( int ii = 0; ii < length - m; ii++ )
                for ( int jj = 0; jj < width - n; jj++ )
                {
                    int sum = prefix[ii+m][jj+n] - prefix[ii+m][jj] - prefix[ii][jj+n] + prefix[ii][jj];
                    
                    if ( Largest < sum )
                    {
                        Largest = sum;
                        startR = ii;
                        startC = jj;
                        length_ = m + 1;
                        width_ = n + 1;
                    }
                }
        }
    }

    int **result = new int* [length_];
    for ( int i = 0; i < length_; i++ )
        result[i] = new int [width_];

    for ( int i = 0; i < length_; i++)
        for ( int j = 0; j < width_; j++ )
            result[i][j] = a[startR + i][startC + j];

    for ( int i = 0; i < length; i++ )
        delete[] prefix[i];
    
    delete[] prefix;
    
    return result;
}

void print(int **a, int m, int n)
{
    for ( int i = 0; i < m; i++ )
    {
        for ( int j = 0; j < n; j++ )
            cout << a[i][j] << " ";
        cout << endl;
    }
    }

int main()
{
    int m, n;
    //cin >> m >> n;
    m = 2;
    n = 3;
    int **a = new int* [m];
    for ( int i = 0; i < m; i++ )
        a[i] = new int [n];

    srand(time(NULL));
    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < n; j++ )
            a[i][j] = rand() % 20;

    print(a, m, n);
    cout << endl;

    int **result = new int* [m];
    for ( int i = 0; i < m; i++ )
        result[i] = new int [n];
    
    int length_, width_;
    result = findSubMatrix(a,m,n,length_,width_);
    print(result,length_,width_);
}