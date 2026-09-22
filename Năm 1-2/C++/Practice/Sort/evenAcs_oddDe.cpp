#include <iostream>

using namespace std;

void ascendingSort(int a[], int start, int end)
{
    for ( int i = start; i < end - 1; i++ )
        for ( int j = i + 1; j < end; j++ )
            if ( a[i] > a[j] )
                swap(a[i], a[j]);
}

void descendingSort(int a[], int start, int end)
{
    for ( int i = start; i < end - 1; i++ )
        for ( int j = i + 1; j < end; j++ )
            if ( a[i] > a[j] )
                swap(a[i], a[j]);
}

void even_Asc_odd_Des(int a[], int n)
{
    int left, right;
    left = 0;
    right = n - 1;

    while(true)
    {
        while ( ( left <= right ) && ( a[left] % 2 == 0 ) )
            left++;

        while ( ( left <= right ) && ( a[right] % 2 != 0 ) )
            right--;

        if ( left >= right ) break;
        swap(a[left], a[right]);
        left++;
        right--;
    }

    ascendingSort(a, 0 , left);
    descendingSort(a, left, n);
}