#include <iostream>
#include <cmath>
#include <string.h>
#include <string>
#include <fstream>
#include <queue>
#include <time.h>
#include <iomanip>

#define table_size 60053

using namespace std;

// Node part
struct Node
{
    char data[30];
    Node *next;
};

int numName = 0;

Node *createNode(char *val)
{
    Node *newNode = new Node;
    strcpy(newNode->data, val);
    newNode->next = NULL;
    return newNode;
}

void addTail(Node *&head, char *val)
{
    Node *newNode = createNode(val);
    if (head == NULL)
    {
        head = newNode;
        return;
    }

    Node *temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

bool isExist(Node *head, char *val)
{
    Node *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data, val) == 0)
            return true;

        temp = temp->next;
    }

    return false;
}

bool isEmpty(Node *head)
{
    return head == NULL;
}

void freeAList(Node *head)
{
    while (head != NULL)
    {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
}

void freeTable(Node *hashTable[])
{
    for (int i = 0; i < table_size; i++)
        freeAList(hashTable[i]);
}

// Read file
void readFile(char *name[])
{
    ifstream in("50k.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        exit(0);
    }

    while (!in.eof())
    {
        string str;
        getline(in, str);
        strcpy(name[numName], str.c_str());
        numName++;
    }

    in.close();
}

// Hash function
unsigned long Hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

unsigned long sdbm(char *str, unsigned int length) // another hashing function
{
    unsigned int hash = 0;
    unsigned int i = 0;

    for (i = 0; i < length; str++, i++)
    {
        hash = (*str) + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

// Collision handling
void Chaining(Node *hashTable[], char *a[])
{
    for (int i = 0; i < numName; i++)
    {
        unsigned long pos;
        pos = Hash(a[i]) % table_size;
        addTail(hashTable[pos], a[i]);
    }
}

void linearProbingAdding(Node *hashTable[], char *a[])
{
    unsigned long pos, temp, hashing, tempPos;

    for (int i = 0; i < numName; i++)
    {
        hashing = Hash(a[i]);
        pos = hashing % table_size;
        tempPos = pos;
        temp = 0;

        if (!isEmpty(hashTable[pos]))
        {
            do
            {
                temp++;
                pos = (hashing + temp) % table_size;
            } while (!isEmpty(hashTable[pos]) && pos != tempPos);

            if (pos == tempPos)
            {
                cout << "Can't add!\n";
                return;
            }
        }

        addTail(hashTable[pos], a[i]);
    }
}

void quadraticProbingAdding(Node *hashTable[], char *a[])
{
    unsigned long pos, temp, hashing, tempPos;

    for (int i = 0; i < numName; i++)
    {
        hashing = Hash(a[i]);
        pos = hashing % table_size;
        tempPos = pos;
        temp = 0;

        if (!isEmpty(hashTable[pos]))
        {
            do
            {
                temp++;
                pos = (hashing + temp * temp) % table_size;
            } while (!isEmpty(hashTable[pos]) && pos != tempPos);

            if (pos == tempPos)
            {
                cout << "Can't add!\n";
                return;
            }
        }
        addTail(hashTable[pos], a[i]);
    }
}

void doubleHashingAdding(Node *hashTable[], char *a[])
{
    unsigned long pos1, pos2;
    unsigned long long pos, temp, tempPos;

    for (int i = 0; i < numName; i++)
    {
        pos1 = Hash(a[i]);
        pos2 = sdbm(a[i], strlen(a[i]));
        temp = 0;
        pos = (pos1 + temp * pos2) % table_size;
        tempPos = pos;

        if (!isEmpty(hashTable[pos]))
        {
            do
            {
                temp++;
                pos = (pos1 + temp * pos2) % table_size;
            } while (!isEmpty(hashTable[pos]) && pos != tempPos);

            if (pos == tempPos)
            {
                cout << "Can't add!\n";
                return;
            }
        }
        addTail(hashTable[pos], a[i]);
    }
}

// Sort function
char getCharAtPos(char *a, int pos)
{
    return a[pos];
}

int findMaxLength(char *a[], int n)
{
    int m = strlen(a[0]);
    for (int i = 1; i < n; i++)
    {
        if (m < strlen(a[i]))
            m = strlen(a[i]);
    }

    return m;
}

void radixSort(char *a[], int n)
{
    queue<char *> s[256];
    int m = findMaxLength(a, n);
    for (int i = m - 1; i >= 0; i--)
    {
        for (int j = 0; j < n; j++)
        {
            if (i < strlen(a[j]))
                s[(unsigned char)(getCharAtPos(a[j], i))].push(a[j]);

            else
                s[0].push(a[j]);
        }
        int k = 0;

        for (int j = 0; j <= 255; j++)
        {
            while (!s[j].empty())
            {
                a[k] = s[j].front();
                k++;
                s[j].pop();
            }
        }
    }
}

// Search function
bool linearSearch(char *list[], char *val, int n)
{
    for (int i = 0; i < n; i++)
        if (strcmp(val, list[i]) == 0)
            return true;

    return false;
}

bool binarySearch(char *list[], char *val, int n)
{
    int l = 0;
    int r = n - 1;
    int m;
    long compare;

    while (l <= r)
    {
        m = l + (r - l) / 2;
        compare = strcmp(val, list[m]);

        if (compare < 0)
            r = m - 1;

        else if (compare > 0)
            l = m + 1;

        else
            return true;
    }

    return false;
}

bool ChainingApproach(Node *hashTable[], char *val)
{
    int pos = Hash(val) % table_size;
    if (isExist(hashTable[pos], val))
        return true;

    return false;
}

bool linearProbing(Node *hashTable[], char *val)
{
    unsigned long pos, hashing;
    hashing = Hash(val);
    pos = hashing % table_size;

    if (isExist(hashTable[pos], val))
        return true;

    unsigned long temp = 1, tempPos = pos;
    do
    {
        pos = (hashing + temp) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return false;

    if (isExist(hashTable[pos], val))
        return true;
}

bool quadraticProbing(Node *hashTable[], char *val)
{
    unsigned long pos, hashing;
    hashing = Hash(val);
    pos = hashing % table_size;

    if (isExist(hashTable[pos], val))
        return true;

    unsigned long temp = 1, tempPos = pos;
    do
    {
        pos = (hashing + temp * temp) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return false;

    if (isExist(hashTable[pos], val))
        return true;
}

bool doubleHashing(Node *hashTable[], char *val)
{
    unsigned long pos1, pos2, pos, temp, tempPos;

    pos1 = Hash(val);
    pos2 = sdbm(val, strlen(val));
    pos = pos1 % table_size;

    if (isExist(hashTable[pos], val))
        return true;

    tempPos = pos;
    temp = 1;

    do
    {
        pos = (pos1 + temp * pos2) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return false;

    if (isExist(hashTable[pos], val))
        return true;
}

void Run(int function, char *names[], char* name)
{
    clock_t start, end;
    double time_use;

    if (function == 1)
    {

        cout << "Linear search\n";
        start = clock();

        if (linearSearch(names, name, numName))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;
    }

    else if (function == 2)
    {
        cout << "Binary search\n";
        radixSort(names, numName);

        start = clock();

        if (binarySearch(names, name, numName))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;
    }

    else if (function == 3)
    {
        cout << "Chaining approach\n";

        Node **chainingTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            chainingTable[i] = NULL;

        Chaining(chainingTable, names);

        start = clock();
        if (ChainingApproach(chainingTable, name))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;

        freeTable(chainingTable);
        delete[] chainingTable;
    }

    else if (function == 4)
    {
        cout << "Linear probing\n";

        Node **linearTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            linearTable[i] = NULL;

        linearProbingAdding(linearTable, names);

        start = clock();
        if (linearProbing(linearTable, name))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;

        freeTable(linearTable);
        delete[] linearTable;
    }

    else if (function == 5)
    {
        cout << "Quadratic probing\n";

        Node **quadraticTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            quadraticTable[i] = NULL;

        quadraticProbingAdding(quadraticTable, names);

        start = clock();
        if (quadraticProbing(quadraticTable, name))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;

        freeTable(quadraticTable);
        delete[] quadraticTable;
    }

    else if(function == 6)
    {
        cout << "Double hashing\n";

        Node **doubleTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            doubleTable[i] = NULL;

        doubleHashingAdding(doubleTable, names);

        start = clock();
        if (doubleHashing(doubleTable, name))
            cout << "Found!\n";

        else
            cout << "Not found!\n";

        end = clock();
        time_use = (double)(end - start) / CLOCKS_PER_SEC;

        freeTable(doubleTable);
        delete[] doubleTable;
    }

    cout << "Time run(ms): " << setprecision(3) << fixed << time_use* 1000 << endl
         << endl;
}

int main()
{
    char *names[table_size];
    char name[30];
    for (int i = 0; i < table_size; i++)
        names[i] = new char[30];

    cout << "Enter a name: ";
    cin >> name;

    readFile(names);

    Run(1, names, name);
    Run(2, names, name);
    Run(3, names, name);
    Run(4, names, name);
    Run(5, names, name);
    Run(6, names, name);

    for (int i = 0; i < table_size; i++)
        delete[] names[i];
    return 0;
}