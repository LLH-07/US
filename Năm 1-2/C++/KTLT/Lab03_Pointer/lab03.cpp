#include <iostream>
#include <time.h>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Problem 1
void swap(int* a, int* b)
{
    int* c = new int;
    *c = *a;
    *a = *b;
    *b = *c;
    delete c;
}

// Problem 2
int* sum(int* a, int* b)
{
    int* sum = new int;
    *sum = *a + *b;
    return sum;
}

// Problem 3
void inputArray(int* a, int &n)
{
    cin >> n;
    for ( int i = 0; i < n; i++ )
        cin >> *(a + i);
}

// Problem 4
void printArray(int* a, int n)
{
    for ( int i = 0; i < n; i++ )
        cout << *(a + i) << ' ';
}

// Problem 5
int* findMax(int* arr, int n)
{
    int *result = new int;
    result = arr;
    for ( int i = 1; i < n; i++ )
        *result = max(*result, *(arr + i));

    return result;
}

// Problem 6
int* copyArray(int* arr, int n)
{
    int *a = new int[n];

    for ( int i = 0; i < n; i++ )
        *(a + i) = *(arr + i);

    return a;
}

// Problem 7
int* countEvens(int* arr, int n, int* evens)
{
    *evens = 0;
    for ( int i = 0; i < n; i++ )
        if ( *(arr + i) % 2 == 0 )
            ++(*evens);

    return evens;
}

int* genEvenSubArray(int* arr, int n, int* count)
{
    count = countEvens(arr, n, count);
    int* a = new int[*count];
    int j = 0;
    for ( int i = 0; i < n; i++ )
        if ( *(arr + i) % 2 == 0)
        {
            *(a + j) = *(arr + i);
            j++;
        }

    return a;
}

// Problem 8
int* findLargestTotalSubArray(int* a, int n, int& total, int &length)
{
    int len;
    int start = 0;
    total = a[0];
    for ( int i = 0; i < n - 1; i++ )
    {
        len = i + 1;
        int num = n - len + 1;

        for ( int j = 0; j < num; j++ )
        {
            int sum = 0;
            for ( int k = j; k < j + len; k++ )
            {
                sum += *(a + k);
            }

            if ( sum > total )
            {
                total = sum;
                length = len;
                start = j;
            }
        }
    }

    int* result = new int[length];
    for ( int i = start; i < start + length; i++ )
        *(result + i) = *(a + i);

    return result;
}

// Another way
int sumMatrix(int left, int right, int a[])
{
    if (left == right) return a[left];
    return sumMatrix(left, right - 1, a) + a[right];
}

int* findMax(int a[], int n, int &len, int &total)
{
    total = a[0];
    int pos = 0;
    len = 1;
    int* result = new int[n];

    for ( int i = 0; i < n; i++ )
        for ( int j = i; j < n; j++ )
            if (total < sumMatrix(i,j,a) )
            {
                total = sumMatrix(i,j,a);
                pos = i;
                len = j - i + 1;
            }
    int j = 0;
    for (int i = pos; i < pos + len; i++)
    {
        result[j] = a[i];
        j++;
    }
    
    return result;
}

// Problem 9
int* findLongestAscendingSubArray(int* a, int n, int& length)
{
    int *inc = new int[n];
    int *result = a;
    int  pos = 0;
    length = 1;

    for ( int i = 0; i < n; i++ )
        *(inc + i) = 1;

    for ( int i = 0; i < n - 1; i++ )
    {
        if( *(a + i) < *(a + i + 1) )
        {
            *(inc + i + 1) = *(inc + i) + 1;
            length = max(length, *(inc + i + 1));
            pos = i + 1;
        }
    }

    delete []inc;
    result += pos - length + 1;

    return result;
}

// Problem 10
void swapArrays(int* a, int* b, int &na, int &nb)
{
    int *temp = new int[na];

    for ( int i = 0; i < na; i++ )
        *(temp + i) = *(a + i);

    for ( int i = 0; i < nb; i++ )
        *(a + i) = *(b + i);

    for ( int i = 0; i < na; i++ )
        *(b + i) = *(temp + i);

    swap(na, nb);
    delete []temp;
}

// Problem 11
int* concatenate2Arrays(int* a, int* b, int na, int nb)
{
    int len = na + nb;
    int *result = new int[len];
    for ( int i = 0; i < na; i++ )
        *(result + i) = *(a + i);

    int j = 0;
    for ( int i = na; i < len; i++ )
    {
        *(result + i) = *(b + j);
        j++;
    }

    return result;
}

// Problem 12
int* merge2Arrays(int* a, int* b, int na, int nb, int& nc)
{
    int* c;
    int i = 0, j = 0, k = 0;
    nc = na + nb;
    c = new int[nc];

    while ( i < na && j < nb && k < nc )
    {
        if (a[i] <= b[j])
        {
            c[k] = a[i];
            i++;
        }
        else
        {
            c[k] = b[j];
            j++;
        }
        k++;
    }

    while ( i < na )
    {
        c[k] = a[i];
        k++;
        i++;
    }

    while ( j < nb )
    {
        c[k] = b[j];
        k++;
        j++;
    }
    
    return c;
}


// Problem 13
void generateMatrix1(int** A, int &length, int &width)
{
    cin >> length >> width;
    
    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            cin >> A[i][j];
}

int** generateMatrix2(int* a, int* b, int na, int nb)
{
    int **arr;
    arr = new int*[na];
    for ( int i = 0; i < na; i++ )
        arr[i] = new int[nb];

    for ( int i = 0; i < na; i++ )
        for ( int j = 0; j < nb; j++ )
            arr[i][j] = a[i] * b[j];

    return arr;
}

// Problem 15
void swapRows (int** a, int length, int width)
{
    int *temp = new int[width];
    int row1, row2;

    for ( int j = 0; j < width; j++ )
        *(temp + j) = *(*(a + row1) + j);

    for ( int j = 0; j < width; j++ )
        *(*(a + row1) + j) = *(*(a + row2) + j);

    for ( int j = 0; j < width; j++ )
        *(*(a + row2) + j) = *(temp + j);

    delete []temp;
}

void swapColumns (int** a, int length, int width)
{
    int *temp = new int[length];
    int col1, col2;

    for ( int i = 0; i < length; i++ )
        *(temp + i) = *(*(a + i) + col1);

    for ( int i = 0; i < length; i++ )
        *(*(a + i) + col1) = *(*(a + i) + col2);

    for ( int i = 0; i < length; i++ )
        *(*(a + i) + col2) = *(temp + i);

    delete []temp;
}

// Problem 16
int** transposeMatrix(int** a, int length, int width)
{
    int** result;
    result = new int* [width];
    for ( int i = 0; i < length; i++ )
        result[i] = new int[length];

    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            *(*(result + j) + i) = *(*(a + i) + j);

    return result;
}

// 17. Concatenate 2 given size-equal matrices, horizontally / vertically.
int** concatenate2MatricesH(int** a, int** b, int length, int width)
{
    int** result;
    int w = width * 2; //row
    result = new int* [length];
    for ( int i = 0; i < length; i++ )
        result[i] = new int [w];
    
    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            *(*(result + i) + j) = *(*(a + i) + j);

    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            *(*(result + i) + j + width) = *(*(b + i) + j);

    return result;
}
int** concatenate2MatricesV(int** a, int** b, int length, int width)
{
    int **result;
    int len = length * 2;
    result = new int* [len];
    for ( int i = 0; i < len; i++ )
        result[i] = new int [width];

    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            *(*(result + i) + j) = *(*(a + i) + j );

    for ( int i = 0; i < length; i++ )
        for ( int j = 0; j < width; j++ )
            *(*(result + i + length) + j) = *(*(b + i) + j);

    return result;
}

// Problem 18
int** multiple2Matrices(int** a, int** b, int lengtha, int widtha, int lengthb, int widthb)
{
    if (widtha != lengthb)
        return NULL;

    int** result;
    result = new int* [lengtha];
    for ( int i = 0; i < lengtha; i++ )
        result[i] = new int [widthb];

    for ( int i = 0; i < lengtha; i++ )
        for ( int j = 0; j < widthb; j++ )
        {
            int total = 0;

            for ( int k = 0; k < widtha; k++ )
                total += a[i][k] * b[k][j];

            result[i][j] = total;
        }

    return result;
}

// Problem 19
//Given matrix a. Find the sub-matrix of a which satisfy keyboard input size and has the largest total value of its elements.
void prefixMatrix(int **a, int row, int col, int **prefix)
{
    for ( int i = 0; i < row; i++ )
        prefix[i][0] = a[i][0];

    for ( int j = 0; j < col; j++ )
        prefix[0][j] = a[0][j];

    for ( int i = 0; i < row; i++ )
        for ( int j = 0; j < col; j++ )
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

            for ( int ii = 0; ii < length - m + 1; ii++ )
                for ( int jj = 0; jj < width - n + 1; jj++ )
                {
                    int sum = prefix[ii + m - 1][jj + n - 1] - prefix[ii + m - 1][jj] - prefix[ii][jj + n - 1] + prefix[ii - 1][jj - 1];
                    if ( Largest < sum )
                    {
                        Largest = sum;
                        startR = ii;
                        startC = jj;
                        length_ = m;
                        width_ = n;
                    }
                }
        }
    }

    int **result = new int* [length_];
    for ( int i = 0; i < length_; i++ )
        result[i] = new int [width_];

    for ( int i = 0; i < length_; i++)
        for ( int j = 0; i < width_; j++ )
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
    cin >> m >> n;
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