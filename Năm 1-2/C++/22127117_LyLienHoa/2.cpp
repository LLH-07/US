#include <iostream>
#include <time.h>
#include <queue>

#define max_size 10000

using namespace std;


void printArr(int a[], int n)
{
    for(int i = 0; i < n; i++)
        cout << a[i] << ' ';

    cout << endl;
}


// descending
void heapify(int a[], int n, int parent)
{
    int left = 2 * parent + 1;
    int right = left + 1;
    int tempParent = parent;

    if (left < n && a[parent] > a[left])
        tempParent = left;

    if (right < n && a[parent] > a[right] && a[left] > a[right])
        tempParent = right;

    if (tempParent != parent)
    {
        swap(a[tempParent], a[parent]);
        heapify(a, n, tempParent);
    }
}

void heapSort(int a[], int n)
{
    for (int i = (n / 2) - 1; i >= 0; i--)
        heapify(a, n, i);

    for (int i = n - 1; i >= 0; i--)
    {
        swap(a[i], a[0]);
        heapify(a, i, 0);
    }
}

// ascending
void quickSort(int a[], int left, int right)
{
    int l = left, r = right;
    int x = a[right];
    while(l <= r)
    {
        while(l <= r && a[l] < x)
            l++;

        while(l <= r && a[r] > x)
            r--;

        if(l <= r)
        {
            swap(a[l], a[r]);
            l++;
            r--;
        }
    }

    if(l < right) quickSort(a, l ,right);
    if(r > left) quickSort(a, left, r);
}

int getLength(int a)
{
    int res = 0;
    while(a > 0)
    {
        res++;
        a /= 10;
    }

    return res;
}
int getMaxLength(int a[], int n)
{
    int res = 0;
    for(int i = 0; i < n; i++)
        res = max(res, getLength(a[i]));
}

void getNum(int a, int t)
{
    int x = 0;
    int res = 0;
    while(x < t)
    {
        res = a % 10;
        a /= 10;
    }
}



void sort(int a[], int b[], int c[], int m, int n)
{
    int i = 0;
    int j = n - 1;
    int pos = 0;

    while(i < m && j >= 0)
    {
        if(a[i] > b[j])
        {
            c[pos] = a[i];
            pos++;
            i++;  
        }

        else
        {
            c[pos] = b[j];
            pos++;
            j--;
        }
    }

    while(i < m)
    {
        c[pos] = a[i];
        pos++;
        i++;
    }

    while(j >= 0)
    {
        c[pos] = b[j];
        pos++;
        j--;
    }
}

void run()
{
    int m, n, sc;
    cout << "Size of array a(max: 10000): ";
    cin >> m;
    cout << "Size of array b(max: 10000): ";
    cin >> n;

    int* a = new int[m];
    int* b = new int[n];
    int* c = new int[m + n];

    
    srand(time(NULL));
    for (int i = 0; i < m; i++)
        a[i] = rand();

    for (int i = 0; i < n; i++)
        b[i] = rand();

    sc = m + n;
    heapSort(a, m);
    quickSort(b, 0, n - 1);

    cout << "First array:\n";
    printArr(a, m);

    cout << "Second array:\n";
    printArr(b, n);

    cout << "Descending array after merging 2 array: \n";
    sort(a, b, c, m, n);
    printArr(c, sc);

    delete[] a;
    delete[] b;
    delete[] c;
}

int main()
{
    run();
    return 0;
}