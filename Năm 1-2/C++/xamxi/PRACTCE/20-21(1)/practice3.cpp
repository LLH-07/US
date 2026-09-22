#include <iostream>
#include <fstream>

using namespace std;

struct Node
{
    int key;
    Node *next;
};

Node *createNode(int k)
{
    Node *newNode = new Node;
    newNode->key = k;
    newNode->next = NULL;
    return newNode;
}

void readFile(string filename, Node *&head)
{
    ifstream in(filename);
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    int k;
    Node *curr;
    while (!in.eof())
    {
        in >> k;
        Node *temp = createNode(k);
        if (head == NULL)
        {
            head = temp;
            curr = head;
        }

        else
        {
            curr->next = temp;
            curr = temp;
        }
    }
}

bool isPrime(int n)
{
    if (n < 2)
        return false;

    for (int i = 2; i <= sqrt(n); i++)
        if (n % i == 0)
            return false;

    return true;
}

void removeFirstPrime(Node*& head)
{
    if(head == NULL)
        return;

    if(isPrime(head->key))
    {
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    Node* prev = NULL;
    Node* curr = head;

    while(curr != NULL)
    {
        if(isPrime(curr->key))
        {
            prev->next = curr->next;
            delete curr;
            return;
        }
    }
}