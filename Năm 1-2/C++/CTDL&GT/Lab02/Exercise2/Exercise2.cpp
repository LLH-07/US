#include <iostream>
#include <cmath>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>

using namespace std;

struct employee
{
    string id;
    string name;
    string birthYear;
};

// Heap sort
void heapify(string a[], int n, int parent, vector<employee> &e)
{
    int left = parent * 2 + 1;
    int right = parent * 2 + 2;
    int tempParent = parent;

    if (left < n && a[left] > a[tempParent])
        tempParent = left;

    if (right < n && a[right] > a[tempParent])
        tempParent = right;

    // if tempParent == parent
    // --> parent have no children or children is greater.
    if (tempParent != parent)
    {
        swap(a[tempParent], a[parent]);
        swap(e[tempParent], e[parent]);
        heapify(a, n, tempParent, e);
    }
}

void heapSort(string a[], int n, vector<employee> &e)
{
    // First phase : Create heap
    for (int i = (n / 2) - 1; i >= 0; i--)
        heapify(a, n, i, e);

    // Second phase:
    for (int i = n - 1; i > 0; i--)
    {
        swap(a[0], a[i]);
        swap(e[0], e[i]);
        heapify(a, i, 0, e);
    }
}

// Merge sort
void merge2Part(int left, int mid, int right, vector<employee> &e)
{
    int sizeLeft = mid - left + 1;
    int sizeRight = right - mid;

    vector<employee> le;
    vector<employee> re;

    // Initialize
    for (int i = 0; i < sizeLeft; i++)
        le.push_back(e[left + i]);

    for (int i = 0; i < sizeRight; i++)
        re.push_back(e[mid + 1 + i]);

    // Compare 2 sub-array
    int index_l = 0;
    int index_r = 0;
    int i = left;

    while (index_l < sizeLeft && index_r < sizeRight)
    {
        if (le[index_l].name > re[index_r].name)
        {
            e[i] = re[index_r];
            index_r++;
        }

        else
        {
            e[i] = le[index_l];
            index_l++;
        }
        i++;
    }

    while (index_l < sizeLeft)
    {
        e[i] = le[index_l];
        index_l++;
        i++;
    }

    while (index_r < sizeRight)
    {
        e[i] = re[index_r];
        index_r++;
        i++;
    }
}

void mergeSort(int left, int right, vector<employee> &e)
{
    if (left >= right)
        return;

    int mid = (left + right) / 2;
    mergeSort(left, mid, e);
    mergeSort(mid + 1, right, e);
    merge2Part(left, mid, right, e);
}

// Quick sort
void quickSort(vector<employee> &e, int left, int right)
{
    string x = e[right - 1].name;
    int i = left;
    int j = right;

    while (i <= j)
    {
        while (e[i].name < x)
            i++;
        while (e[j].name > x)
            j--;

        if (i <= j)
        {
            swap(e[i], e[j]);
            i++;
            j--;
        }
    }

    // recursion for left and right part
    if (left < j)
        quickSort(e, left, j);

    if (i < right)
        quickSort(e, i, right);
}

vector<employee> readFile()
{
    ifstream in("employee.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        return {};
    }

    vector<employee> e;
    string tmp = "";
    getline(in, tmp);

    while (!in.eof())
    {
        string id = "";
        string name = "";
        string birth = "";
        employee temp;

        getline(in, id, ',');
        getline(in, name, ',');
        getline(in, birth, '\n');

        temp.id = id;
        temp.name = name;
        temp.birthYear = birth;

        e.push_back(temp);
    }

    in.close();

    return e;
}

void print(vector<employee> e)
{
    ofstream out("'sortedemponage.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    for (int i = 0; i < e.size(); i++)
        out << e[i].id << ", " << e[i].name << ", " << e[i].birthYear << endl;

    out.close();
}

void run()
{
    vector<employee> e;
    e = readFile();

    int n = e.size();
    string *age = new string[n];

    for (int i = 0; i < n; i++)
        age[i] = e[i].birthYear;

    heapSort(age, n, e);

    int start = 0, end = 0;
    for (int i = 0; i < n - 1; i++)
    {
        if (age[i] != age[i + 1])
        {
            if (start != end)
            {
                // merge sort or quick sort
                //mergeSort(start, end, e);
                //quickSort(e, start, end);
                end++;
                start = end;
            }

            else
            {
                start++;
                end++;
            }
        }

        else
            end++;
    }

    print(e);

    delete[] age;
}

int main()
{
    run();
}