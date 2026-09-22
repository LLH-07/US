#include <iostream>
#include <cmath>

using namespace std;

// LINEAR SEARCH

// If array is not stored in order
//Linear search without sentinel

bool LinearSearch(int a[], int n, int k)
{
    int i = 0;
    while( ( i < n ) && ( a[i] != k ) )
        i++;
    
    return ( a[i] == k && i < n);
}

//Linear search with sentinel
bool linearSearch(int a[], int n, int k)
{
    a[n] = k; // Sentinel
    int i = 0;
    while ( a[i] != k )
        i++;

    return (i < n);
}

// If array is stored in ascending order
bool ascendingLinearSearch(int a[], int n, int k)
{
    a[n] = k + 1;
    int i = 0;
    while ( a[i] < k )
        i++;

    return ( a[i] = k );
}

//Disadvantage: Inefficient

///////////////////////////////////////////////////////////////////////

// BINARY SEARCH
// Disadvantage: Array must be sorted

// If array is sorted in ascending order
bool binarySearch(int a[], int n, int k)
{
    int left, mid, right;
    left = 0;
    right = n - 1;
    int count = 0;
    while ( left <= right )
    {
        mid = ( left + right ) / 2;

        if (a[mid] == k)
            return true;

        else if ( k < a[mid] )
            right = mid - 1;

        else
            left = mid + 1;
    }

    return false;
}

// improvement from Binary search are:

bool recursiveBinarySearch(int a[], int k, int left, int right)
{
    int mid;
    mid = ( left + right ) / 2;
    if ( left > right ) 
        return false;

    if ( a[mid] == k )
        return true;

    else if ( a[mid] > k )
        right = mid - 1;

    else
        left = mid + 1;

    recursiveBinarySearch(a, k, left, right);

}

bool InterpolationSearch(int a[], int n, int k)
{
    int left, right;
    left = 0;
    right = n - 1;

    while ( ( left <= right ) && ( a[left] <= k ) && ( a[right] >= k ))
    {
        int search;
        search = left + ( ( (double)( right - left ) / ( a[right] - a[left] ) ) * ( k - a[left] ) );

        if ( a[search] == k )
            return true;

        else if ( a[search] < k )
            left = search + 1;

        else
            right = search - 1;

    }

    return false;
}
////////////////////////////////////////////////////////////////////////

// FIND MIN, MAX
void findMinMax (int a[], int n, int& small, int& large)
{
    small = a[0];
    large = a[0];

    for ( int i = 1; i < n; i++ )
    {
        if ( large < a[i] )
            large = a[i];

        else
            if ( a[i] < small )
                small = a[i];
    }
}

int main()
{
    int a[1000] =  {16, 31, 40, 53, 68, 80, 95};
    if( recursiveBinarySearch(a, 33, 0, 6) ) cout << 1;
    else cout << 0;
    return 0;
}
