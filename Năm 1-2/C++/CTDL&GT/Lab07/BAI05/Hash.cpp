#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <time.h>
#define table_size 60053

using namespace std;


int numName = 0;
// Node part
struct Node
{
    char data[30];
    Node *next;
};

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
    unsigned long hash = 0;
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

            if (temp == table_size)
            {
                cout << "Can't add! ";
                return;
            }
        }
        addTail(hashTable[pos], a[i]);
    }
}

// Search
int ChainingApproach(Node *hashTable[], char *val)
{
    int pos = Hash(val) % table_size;
    if (isExist(hashTable[pos], val))
        return 1;

    return 0;
}

int linearProbing(Node *hashTable[], char *val)
{
    unsigned long pos, hashing;
    hashing = Hash(val);
    pos = hashing % table_size;

    if (isExist(hashTable[pos], val))
        return 1;

    unsigned long temp = 1, tempPos = pos;
    do
    {
        pos = (hashing + temp) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return 0;

    if (isExist(hashTable[pos], val))
        return 1;
}

int quadraticProbing(Node *hashTable[], char *val)
{
    unsigned long pos, hashing;
    hashing = Hash(val);
    pos = hashing % table_size;

    if (isExist(hashTable[pos], val))
        return 1;

    unsigned long temp = 1, tempPos = pos;
    do
    {
        pos = (hashing + temp * temp) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return 0;

    if (isExist(hashTable[pos], val))
        return 1;
}

int doubleHashing(Node *hashTable[], char *val)
{
    unsigned long pos1, pos2, pos, temp, tempPos;

    pos1 = Hash(val);
    pos2 = sdbm(val, strlen(val));
    pos = pos1 % table_size;

    if (isExist(hashTable[pos], val))
        return 1;

    tempPos = pos;
    temp = 1;

    do
    {
        pos = (pos1 + temp * pos2) % table_size;
        temp++;
    } while (pos != tempPos && !isExist(hashTable[pos], val));

    if (pos == tempPos)
        return 0;

    if (isExist(hashTable[pos], val))
        return 1;
}

// Read file
void readFile(char *names[])
{
    ifstream in("names.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        exit(0);
    }
    while (!in.eof())
    {
        string str;
        getline(in, str);
        strcpy(names[numName], str.c_str());
        numName++;
    }

    in.close();
}

// Time
void countTime(int function, char *names[], char* name)
{
    clock_t start, end;
    double usedTime;
    int temp;

    cout << "Time running(ms) of ";
    if(function == 1)
    {
        cout << "chaining approach: ";
        Node **chainingTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            chainingTable[i] = NULL;

        Chaining(chainingTable, names);

        start = clock();
        temp = ChainingApproach(chainingTable, name);
        end = clock();

        freeTable(chainingTable);
        delete[] chainingTable;
    }

    else if(function == 2)
    {
        cout << "linear probing: ";
        Node **linearTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            linearTable[i] = NULL;

        linearProbingAdding(linearTable, names);

        start = clock();
        temp = linearProbing(linearTable, name);
        end = clock();

        freeTable(linearTable);
        delete[] linearTable;
    }

    else if(function == 3)
    {
        cout << "quadratic probing: ";
        Node **quadraticTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            quadraticTable[i] = NULL;

        quadraticProbingAdding(quadraticTable, names);

        start = clock();
        temp = quadraticProbing(quadraticTable, name);
        end = clock();

        freeTable(quadraticTable);
        delete[] quadraticTable;
    }

    else if(function == 4)
    {
        cout << "double hashing: ";

        Node **doubleTable = new Node *[table_size];
        for (int i = 0; i < table_size; i++)
            doubleTable[i] = NULL;

        doubleHashingAdding(doubleTable, names);

        start = clock();
        temp = doubleHashing(doubleTable, name);
        end = clock();

        freeTable(doubleTable);
        delete[] doubleTable;
    }

    usedTime = double(end - start) / CLOCKS_PER_SEC;
    cout << fixed << setprecision(3);
    cout << usedTime * 1000.0 << endl;
}

int main()
{
    char *names[table_size];
    for (int i = 0; i < table_size; i++)
        names[i] = new char[30];
    readFile(names);


    char foundName[] = "Yuji";
    char unfound[] = "Phoon";

    cout << "Name is existed:\n";
    for(int i = 1; i <= 4; i++)
        countTime(i, names, foundName);

    cout << "\nName is not existed:\n";
    for(int i = 1; i <= 4; i++)
        countTime(i, names, unfound);

    for (int i = 0; i < table_size; i++)
        delete[] names[i];
        
    return 0;
}

