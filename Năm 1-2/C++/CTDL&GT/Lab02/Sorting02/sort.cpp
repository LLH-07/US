#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

void getRandomArray(int a[], int n, int k)
{
    srand(time(NULL));
    for(int i = 0; i < n; i++)
        a[i] = rand() % (k + 1);
}

// Selection sort
void selectionSort(int a[], int n)
{
    int minimum;

    for(int i = 0; i < n - 1; i++)
    {
        minimum = 0;
        for(int j = i + 1; j < n; j++)
        {
            if(a[minimum] > a[j])
                minimum = j;
        }

        if(i != minimum)
            swap(a[i], a[minimum]);
    }
}

// Insertion sort
void InsertionSort(int a[], int n)
{
    for(int i = 1; i < n - 1; i++)
    {
        int temp = a[i];
        int pos = i - 1;
        while(pos >= 0 && a[pos] > temp)
        {
            a[pos + 1] = a[pos];
            pos--;
        }

        a[pos + 1] = temp;
    }
}

// Interchange sort
void interchangeSort(int a[], int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; j < n; j++)
            if(a[i] > a[j])
                swap(a[i], a[j]);
    }
}

// Bubble sort
void bubbleSort(int a[], int n)
{
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n - 1; j++)
            if(a[j] > a[j +1])
                swap(a[j], a[j + 1]);
}

// Shaker sort
void shakerSort(int a[], int n)
{
    int top = 0;
    int bottom = n - 1;
    int temp = top;

    while(top < bottom)
    {
        for(int i = top; i < bottom; i++)
            if(a[i] > a[i + 1])
            {
                swap(a[i], a[i + 1]);
                temp = i;
            }

        top = temp;

        for(int i = bottom - 1; i >= top; i--)
            if(a[i] < a[i - 1])
            {
                swap(a[i], a[i - 1]);
                temp = i;
            }

        bottom = temp;
    }
}

void run()
{
    int n, k;
    int a
    cout << "Input: ";
    cin >> n >> k;

}

