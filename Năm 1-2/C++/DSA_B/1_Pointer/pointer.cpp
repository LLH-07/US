#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int *sum(int *a, int *b)
{
    int *s = new int;
    *s = *a + *b;
    return s;
}

void inputArray(int *a, int &n)
{
    n = 0;
    while (cin >> a[n])
        n++;
}

void printArray(int *a, int n)
{
    for (int i = 0; i < n; i++)
        cout << a[i] << ' ';
}

int *findMax(int *arr, int n)
{
    int *MAX = arr;
    for (int i = 1; i < n; i++)
        if (arr[i] > *MAX)
            MAX = arr + i;
    return MAX;
}

int *findLongestAscendingSubArray(int *a, int n, int &length)
{
    int end = 0;
    int *l = new int[n];
    length = 1;
    for (int i = 1; i < n; i++)
    {
        l[i] = 0;
        for (int j = 0; j < i; j++)
        {
            if (a[j] < a[i])
                l[i] = max(l[i], l[j] + 1);
        }

        if (length < l[i])
        {
            length = l[i];
            end = i;
        }
    }

    int *subArray = new int[length];
    for (int i = 0; i < length; i++)
    {
        subArray[i] = a[end - length + 1 + i];
    }

    delete[] l;
    return subArray;
}

void swapArrays(int *a, int *b, int &na, int &nb)
{
    int len = max(na, nb);
    int *tmp = new int[len];

    for (int i = 0; i < na; i++)
        tmp[i] = a[i];

    for (int i = 0; i < nb; i++)
        a[i] = b[i];

    for (int i = 0; i < na; i++)
        b[i] = tmp[i];

    swap(na, nb);

    delete[] tmp;
}

int *concatenate2Array(int *a, int *b, int na, int nb)
{
    int len = na + nb;
    int *c = new int[len];

    int i = 0;
    for (; i < na; i++)
        c[i] = a[i];

    for (; i < len; i++)
        c[i] = b[i - na];

    return c;
}

int *merge2Arrays(int *a, int *b, int na, int nb, int &nc)
{
    nc = na + nb;
    int *c = new int[nc];

    int i = 0, j = 0, k = 0;
    while (i < na && j < nb)
    {
        if (a[i] < b[j])
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

    while (i < na)
    {
        c[k] = a[i];
        k++;
        i++;
    }

    while (j < nb)
    {
        c[k] = b[j];
        k++;
        j++;
    }

    return c;
}

void generateMatrix1(int **A, int &length, int &width)
{
    cin >> length >> width;
    for (int i = 0; i < width; i++)
        for (int j = 0; j < length; j++)
        {
            srand(time(NULL));
            A[i][j] = rand() % 1000;
        }
}

int **generateMatrix2(int *a, int *b, int na, int nb)
{
    int **c = new int *[na];
    for (int i = 0; i < na; i++)
        c[i] = new int[nb];

    for (int i = 0; i < na; i++)
        for (int j = 0; j < nb; j++)
            c[i][j] = a[i] * b[j];

    return c;
}

void swapRows(int **a, int length, int width)
{
    int row1, row2;
    cin >> row1 >> row2;

    swapArrays(a[row1], a[row2], length, length);
}

void swapColumn(int **a, int length, int width)
{
    int col1, col2;
    cin >> col1 >> col2;

    for (int i = 0; i < width; i++)
    {
        swap(a[i][col1], a[i][col2]);
    }
}

int **transposeMatrix(int **a, int length, int width)
{
    // W x L
    int **arr = new int *[length];
    for (int i = 0; i < length; i++)
        arr[i] = new int[width];

    for (int j = 0; j < length; j++)
    {
        for (int i = 0; i < width; i++)
            arr[j][i] = a[i][j];
    }

    return arr;
}

int **concatenate2MatricesH(int **a, int **b, int length, int width)
{
    int **c = new int *[width];
    for (int i = 0; i < width; i++)
        c[i] = new int[2 * length];

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < length; j++)
        {
            c[i][j] = a[i][j];
            c[i][j + length] = b[i][j];
        }
    }

    return c;
}

int **concatenate2MatricesV(int **a, int **b, int length, int width)
{
    int **c = new int *[2 * width];
    for (int i = 0; i < 2 * width; i++)
        c[i] = new int[length];

    for (int i = 0; i < width; i++)
        for (int j = 0; j < length; j++)
        {
            c[i][j] = a[i][j];
            c[i + width][j] = b[i][j];
        }

    return c;
}

bool multiple2Matrices(int **a, int **b, int lengtha, int widtha, int lengthb, int widthb, int**c)
{
    // W x L
    if (lengtha != widthb)
        return false;

    for(int i = 0; i < widtha; i++)
    {
        for(int j = 0; j < lengthb; j++)
        {
            int total = 0;

            for(int k = 0; k < lengtha; k++)
                total += a[i][k] * b[k][j];

            c[i][j] = total;
        }
    }

    return true;
}

int **findSubMatrix(int **a, int length, int width, int &length_, int &width_)
{
    // W x L
    cin >> width_ >> length_;
}

int LinearSearch(int* a, int n, int key)
{
    for(int i = 0; i < n; i++)
        if(a[i] == key)
            return i;

    return -1;
}

int SentineLinearSearch(int* a, int n, int key)
{
    int flag = -1;
    for(int i = 0; i < n; i++)
        if(a[i] == key)
        {
            flag = i;
            break;
        }

    return flag;
}

int BinarySearch(int* a, int n, int key)
{
    int left = 0, right = n - 1;
    int mid;

    while(left <= right)
    {
        mid = left + (right - left) / 2;
        if(a[mid] < key) left = mid + 1;
        else if(a[mid] > key) right = mid - 1;
        else return mid;
    }

    return -1;
}

int RecursiveBinarySearch(int* a, int left, int right, int key)
{
    int mid = left + (right - left) / 2;
    if(left > right) return -1;
    if(a[mid] < key)
        return RecursiveBinarySearch(a, mid + 1, right, key);
    else if(a[mid] > key)
        return RecursiveBinarySearch(a, left, mid - 1, key);
    return mid;
}




