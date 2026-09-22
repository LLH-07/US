#include <iostream>

using namespace std;

void insert(int pos, int x, int a[], int& n)
{
    ++n;
    for( int i = n - 1; i > pos - 1; i-- )
        a[i] = a[i - 1];
    
    a[pos] = x;
}

void bubbleSort(int a[], int n)
{
    for( int i = 0; i < n; i++ )
        for( int j = 0; j < n - i - 1; j++ )
            if( a[j] > a[j + 1] )
                swap(a[j], a[j + 1]);
}

void selectionSort(int a[], int n)
{
    int smallest;
    for( int i = 0; i < n; i++ )
    {
        smallest = i;
        for( int j = i + 1; j < n; j++ )
        {
            if( a[smallest] > a[j] )
                smallest = j;
        }
        
        if(smallest != i)
            swap(a[smallest], a[i]);
    }
}

void insertionSort(int a[], int n)
{
    int key;
    for( int i = 1; i < n; i++ )
    {
        // Travel from 2nd elements
        key = a[i];

        // Represent for previous position of i 
        j = i - 1;

        // if previous value is greater than current value (key)?
        while(( j > 0 ) && ( a[j] > key ))
        {
            // Move elements of a[0,..,i - 1] that greater then key
            // to one position ahead of their current position
            a[j + 1] = a[j];
            j--;
        }
        // if it (j) can't move, it means position (j + 1) is key
        // because the previous value is smaller or j = -1
        a[j + 1] = key;
    }
}

void interchangeSort(int a[], int n)
{
    for( int i = 0; i < n - 1; i++ )
        for(int j = i + 1; j < n; j++ )
            if(a[i] > a[j])
                swap(a[i], a[j]);
}


int main()
{
    int a[] = { 5, 3, 8, 4, 6 };
    int n = 5;

    for( int i = 0; i < n; i++)
        cout << a[i] << " ";

    return 0;
}