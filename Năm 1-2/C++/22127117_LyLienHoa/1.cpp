#include <iostream>
#include <time.h>
#define max_size 10000

using namespace std;


void printArr(int a[], int n)
{
    for(int i = 0; i < n; i++)
        cout << a[i] << ' ';

    cout << endl;
}

void insertionSort(int a[], int n)
{
    int x, pos;
    for(int i = 1; i < n; i++)
    {
        x = a[i];
        pos = i - 1;
        while(pos >= 0 && a[pos] > x)
        {
            a[pos + 1] = a[pos];
            pos--;
        }

        a[pos + 1] = x;
    }
}

void heapify(int a[], int n, int parent)
{
    int left = 2 * parent + 1;
    int right = left + 1;
    int tempParent = parent;

    if(left < n && a[parent] > a[left])
        tempParent = left;

    if (right < n && a[parent] > a[right] && a[left] > a[right])
        tempParent = right;

    if(tempParent != parent)
    {
        swap(a[tempParent], a[parent]);
        heapify(a, n, tempParent);
    }
}

void heapSort(int a[], int n)
{
    for(int i = (n / 2) - 1; i >= 0; i--)
        heapify(a, n, i);

    for(int i = n - 1; i >= 0; i--)
    {
        swap(a[i], a[0]);
        heapify(a, i, 0);
    }
}

void run()
{
    int n;
    cout << "Size of array(max: 10000): ";
    cin >> n;

    int a[max_size];
    srand(time(NULL));
    for(int i = 0; i < n; i++)
        a[i] = rand();

    cout << "Array before sorting:\n";
    printArr(a, n);

    cout << "Ascending array:\n";
    insertionSort(a, n);
    printArr(a, n);

    cout << "Descending array:\n";
    heapSort(a, n);
    printArr(a, n);
}

int main()
{
    run();
    return 0;
}