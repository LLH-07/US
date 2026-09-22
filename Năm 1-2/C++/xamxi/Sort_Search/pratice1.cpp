#include <iostream>
#include <cmath>
#include <queue>

using namespace std;

void printArray(int a[], int n)
{
    for (int i = 0; i < n; i++)
        cout << a[i] << ' ';

    cout << endl;
}

// Middle postion have to be put in the loop
int binarySearch(int a[], int left, int right, int val)
{
    int mid;
    while (left <= right)
    {
        mid = left + (right - left) / 2;

        if (a[mid] < val)
            left = mid + 1;

        else if (a[mid] > val)
            right = mid - 1;

        else
            return mid;
    }

    return left;
}

// Finding is done remember to swap it
void selectionSort(int a[], int n)
{
    int posMin = 0;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
            if (a[posMin] > a[j])
                posMin = j;

        swap(a[posMin], a[i]);
    }
}

void insertionSort(int a[], int n)
{
    int pos, x;
    for (int i = 1; i < n; i++)
    {
        pos = i - 1;
        x = a[i];

        while (pos >= 0 && a[pos] > x)
        {
            a[pos + 1] = a[pos];
            pos--;
        }

        a[pos + 1] = x;
    }
}

void interchangeSort(int a[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i] > a[j])
                swap(a[i], a[j]);
}

// The boolean sorted have to be put in the first loop
void bubbleSort(int a[], int n)
{
    bool sorted;
    for (int i = 0; i < n - 1; i++)
    {
        sorted = true;
        for (int j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1])
            {
                sorted = false;
                swap(a[j], a[j + 1]);
            }

        if (sorted)
            return;
    }
}

// The condition should be (top < bottom) for the first loop
void shakerSort(int a[], int n)
{
    int top = 0, bottom = n - 1;
    int pos = n - 1;

    while (top < bottom)
    {
        for (int j = bottom; j > top; j--)
        {
            if (a[j] < a[j - 1])
            {
                pos = j;
                swap(a[j], a[j - 1]);
            }
        }

        top = pos;

        for (int j = top; j < bottom; j++)
        {
            if (a[j] > a[j + 1])
            {
                pos = j;
                swap(a[j], a[j + 1]);
            }
        }

        bottom = pos;
    }
}

// if you can't find val return left position
void binaryInsertionSort(int a[], int n)
{
    int pos, x;
    for (int i = 1; i < n; i++)
    {
        x = a[i];
        pos = binarySearch(a, 0, i - 1, x);
        for (int j = i - 1; j >= pos; j--)
            a[j + 1] = a[j];

        a[pos] = x;
    }
}

void shellSort(int a[], int n)
{
    for (int gap = n / 2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < n; i++)
        {
            int x = a[i];
            int pos;

            for (pos = i; pos >= gap; pos -= gap)
            {
                if (a[pos - gap] > x)
                    a[pos] = a[pos - gap];

                else
                    break;
            }
            a[pos] = x;
        }
    }
}

void heapify(int a[], int parent, int n)
{
    int left = parent * 2 + 1;
    int right = left + 1;
    int tempParent = parent;

    if (left < n && a[parent] < a[left])
        tempParent = left;

    if (right < n && a[parent] < a[right])
        tempParent = right;

    if (tempParent != parent)
    {
        swap(a[tempParent], a[parent]);
        heapify(a, tempParent, n);
    }
}

void heapSort(int a[], int n)
{
    for (int i = (n - 1) / 2; i >= 0; i--)
        heapify(a, i, n);

    for (int i = n - 1; i >= 1; i--)
    {
        swap(a[0], a[i]);
        heapify(a, 0, i);
    }
}

void quickSort(int a[], int left, int right)
{
    int x = a[right];
    int l = left, r = right;

    while (l <= r)
    {
        while (a[l] < x)
            l++;

        while (a[r] > x)
            r--;

        if (l <= r)
        {
            swap(a[l], a[r]);
            l++;
            r--;
        }
    }

    if (left < r)
        quickSort(a, left, r);

    if (l < right)
        quickSort(a, l, right);
}

void merge(int a[], int left, int mid, int right)
{
    int nl = mid - left + 1;
    int nr = right - mid;
    int *l = new int[nl];
    int *r = new int[nr];

    for (int i = 0; i < nl; i++)
        l[i] = a[left + i];

    for (int i = 0; i < nr; i++)
        r[i] = a[mid + i + 1];

    int i = 0, j = 0, pos = left;
    while (i < nl && j < nr)
    {
        if (l[i] < r[j])
        {
            a[pos] = l[i];
            i++;
        }

        else
        {
            a[pos] = r[j];
            j++;
        }
        pos++;
    }

    while (i < nl)
    {
        a[pos] = l[i];
        pos++;
        i++;
    }

    while (j < nr)
    {
        a[pos] = r[j];
        pos++;
        j++;
    }

    delete[] l;
    delete[] r;
}

void mergeSort(int a[], int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(a, left, mid);
    mergeSort(a, mid + 1, right);
    merge(a, left, mid, right);
}

int getLength(int a)
{
    int res = 0;
    while (a > 0)
    {
        res++;
        a /= 10;
    }

    return res;
}

int findMaxLength(int a[], int n)
{
    int res = 0;
    for (int i = 0; i < n; i++)
        res = max(res, getLength(a[i]));

    return res;
}

int getNumAt(int a, int pos)
{
    int res = -1;
    for (int i = 0; i < pos; i++)
    {
        res = a % 10;
        a /= 10;
    }

    return res;
}

void radixSort(int a[], int n)
{
    queue<int> q[10];
    int len = findMaxLength(a, n);
    for (int k = 1; k <= len; k++)
    {
        for(int i = 0; i < n; i++)
            q[getNumAt(a[i], k)].push(a[i]);

        int j = 0;
        for(int i = 0; i < 10; i++)
        {
            while(!q[i].empty() && j < n)
            {
                a[j] = q[i].front();
                j++;
                q[i].pop();
            }
        }
    }
}

int main()
{
    int a[10] = {2, 9, 7, 0, 4, 3, 5, 2, 6, 8};
    int n = 10;

    radixSort(a, n);
    printArray(a, n);
}