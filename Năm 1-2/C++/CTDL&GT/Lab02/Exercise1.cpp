#include <iostream>

using namespace std;

void bubbleSort(int *&a, int n)
{
    bool sorted = true;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1])
            {
                swap(a[j], a[j + 1]);
                sorted = false;
            }

        if (sorted == true)
            return;
    }
}

void getOddEven(int a[], int n, int *&odd, int *&even, int& no, int& ne)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] % 2 == 0)
        {
            even[ne] = a[i];
            ne++;
        }

        else
        {
            odd[no] = a[i];
            no++;
        }
    }
}

void AscendingEvenDescendingOdd(int a[], int n)
{
    int *odd = new int[n];
    int *even = new int[n];
    int no = 0, ne = 0;

    getOddEven(a, n, odd, even, no, ne);

    bubbleSort(odd, no);
    bubbleSort(even, ne);

    int o = no - 1, e = 0;

    for (int i = 0; i < n; i++)
    {
        if (a[i] % 2 == 0)
        {
            a[i] = even[e];
            e++;
        }

        else
        {
            a[i] = odd[o];
            o--;
        }
    }

    delete[] odd;
    delete[] even;
}

void run()
{
    int a[10000];
    int n;

    cout << "Number of elements: ";
    cin >> n;
    cout << "Array include: ";
    for(int i = 0; i < n; i++)
        cin >> a[i];
    
    AscendingEvenDescendingOdd(a, n);
    
    cout << "After sorting: ";
    for(int i = 0; i < n; i++)
        cout << a[i] << ' ';
}

int main()
{
    run();
}