#include <iostream>
#include <cmath>

using namespace std;

void insert(int pos, int x, int a[], int &n)
{
    ++n;
    for (int i = n - 1; i > pos - 1; i--)
        a[i] = a[i - 1];

    a[pos] = x;
}

void print(int n, int a[])
{
    for (int i = 0; i < n; i++)
        cout << a[i] << " ";

    cout << endl;
}

//////////////////////////////////////////////////////////////////////////

// Because the largest number in array has been put in right order
// so to avoid checking again the last number
// we should minus the numbers have been sorted
// by checking from beginning to postion n - i - 1;
// n is quantity of numbers in array
// i is the number of numbers sorted

void bubbleSort(int a[], int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - i - 1; i++)
            if (a[j] > a[j + 1])
                swap(a[j], a[j + 1]);
}

// improved Bubble sort
void BubbleSort(int a[], int n)
{
    cout << "Bubble sort: \n";
    for (int i = 0; i < n; i++)
    {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1])
            {
                swap(a[j], a[j + 1]);
                swapped = true;
            }

        print(n, a);

        if (swapped == false)
            break;
    }
    cout << endl;
}

///////////////////////////////////////////////////////////////////////////

// is an algorithm improved from Bubble Sort
void shakerSort(int a[], int n)
{
    int left, right, lastPos;
    left = 0;
    right = n - 1; // Because check a[i + 1]

    while (left <= right)
    {
        // From left to right
        for (int i = left; i < right; i++)
            if (a[i] > a[i + 1])
            {
                swap(a[i], a[i + 1]);
                lastPos = i;
                // to mark the last position which has been swept
                // it means the rest right elements in order
            }

        right = lastPos;

        // From right to left
        for (int i = right; i > left; i--)
            if (a[i - 1] > a[i])
            {
                swap(a[i - 1], a[i]);
                lastPos = i;
            }

        left = lastPos;

        print(n, a);
    }
}

/////////////////////////////////////////////////////////////////////////////

void selectionSort(int a[], int n)
{
    cout << "Selection sort: \n";

    int smallestPos;
    for (int i = 0; i < n; i++)
    {
        smallestPos = i;
        for (int j = i + 1; j < n; j++)
        {
            if (a[smallestPos] > a[j])
                smallestPos = j;
        }

        if (smallestPos != i)
            swap(a[smallestPos], a[i]);
        print(n, a);
    }
    cout << endl;
}

//////////////////////////////////////////////////////////////////////////

// Without sentinel
void insertionSort(int a[], int n)
{

    cout << "Insertion sort: \n";
    int v, j;
    for (int i = 1; i < n; i++)
    {
        v = a[i];
        j = i - 1;

        while ((j >= 0) && (v < a[j]))
        {
            a[j + 1] = a[j];
            j--;
        }

        a[j + 1] = v;
        print(n, a);
    }

    cout << endl;
}

// With sentinel
void InsertionSort(int a[], int n)
{
    for (int i = n - 1; i >= 0; i--)
        a[i + 1] = a[i];

    int v, j;

    for (int i = 2; i <= n; i++)
    {
        a[0] = a[i]; // sentinel
        v = a[i];
        j = i - 1;

        while (a[j] > v)
        {
            a[j + 1] = a[j];
            j--;
        }
    }

    a[j + 1] = v;

    for (int i = 0; i < n; i++)
        a[i] = a[i + 1];
}

void binaryInsertionSort(int a[], int n)
{
    int v, left, right, mid;

    for (int i = 1; i < n; i++)
    {
        v = a[i];
        left = 0;
        right = i - 1;

        while (left <= right)
        {
            mid = (left + right) / 2;

            if (a[mid] < v)
                left = mid + 1;

            else
                right = mid - 1;
        }

        for (int j = right; j > left; j--)
            a[j] = a[j - 1];

        a[left] = v;
    }
}

//////////////////////////////////////////////////////////////////////////

void interchangeSort(int a[], int n)
{
    cout << "Interchange sort: \n";
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
            if (a[i] > a[j])
                swap(a[i], a[j]);

        print(n, a);
    }
    cout << endl;
}

///////////////////////////////////////////////////////////////////////////////

int partition(int a[], int left, int right)
{
    // 'checked' is a variable to mark an element that need checking
    int checked = a[right];
    int high = right;
    right--;

    while (true)
    {
        // To find an element that greater or equal to 'checked'
        while ((left <= right) && (a[left] < checked))
            left++; // To move to the next position to check

        // To find an element that smaller or equal to 'checked'
        while ((left <= right) && (a[right] > checked))
            right--; // To move to the previous position to check

        // If array in order then stop
        if (right <= left)
            break;

        // Because the left value is greater than 'checked';
        // On the other hand, the right value is smaller than 'checked'
        // So these two value need to be changed position
        swap(a[left], a[right]);

        // After having been checked,
        // Need to increase left position, decrease right position
        // To continue to check the new postion
        left++;
        right--;
    }

    swap(a[left], a[high]);
    return left;
}

void quickSort(int a[], int left, int right)
{
    int checkedPos = partition(a, left, right);
    quickSort(a, left, checkedPos - 1);
    quickSort(a, checkedPos, right);
}

///////////////////////////////////////////////////////////////////////////////

void merge(int a[], int l, int m, int r)
{
    // Number of elements of 2 sub arrays
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays to store 2 sub arrays
    int L[n1], R[n2];

    // Copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = a[l + i];

    for (int i = 0; i < n2; i++)
        R[i] = a[m + 1 + i];
    
    // Merge the temp arrays back into a[l..r]
    
    // Initial index of the first and the second sub array respectively
    int i = 0, j = 0;

    // Initial index of merged sub array
    int k = l;

    while(i < n1 && j < n2)
    {
        if(L[i] < R[j])
        {
            a[k] = L[i];
            i++;
        }

        else
        {
            a[k] = R[j];
            j++;
        }

        k++;
    }

    // Copy the rest elements of 2 sub arrays if there are any
    while(i < n1)
    {
        a[k] = L[i];
        i++;
        k++;
    }

    while(j < n2)
    {
        a[k] = R[j];
        j++;
        k++;
    }

}


void mergeSort(int a, int l, int r)
{
    if(l >= r)
        return;

    // Same as (l + r) / 2, but avoids overflow for large l and r
    int m = l + (r - l) / 2;

    // Sort first and second halves
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m , r);
}

int main()
{
    int a[100] = {91, 67, 20, 83, 45};
    int n = 5;
    // selectionSort(a,n);
    // insertionSort(a,n);
    // bubbleSort(a,n);
    interchangeSort(a, n);
    return 0;
}