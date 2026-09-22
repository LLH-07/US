#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

struct Node
{
    int key;
    Node *next;
};

bool isPrime(int n)
{
    n = abs(n);
    if (n < 2)
        return false;

    for (int i = 2; i <= sqrt(n); i++)
        if (n % i == 0)
            return false;

    return true;
}

Node* getNode(int data)
{
    Node* newNode = new Node;
    newNode->key = data;
    newNode->next = NULL;
    return newNode;
}

void readFile(Node *&head)
{
    ifstream in("input.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    int data;

    if(!in.eof())
    {
        in >> data;
        Node* newNode = getNode(data);
        head = newNode;
    }

    Node* curr = head;
    
    while(!in.eof())
    {
        in >> data;
        Node* newNode = getNode(data);
        curr->next = newNode;
        curr = newNode;
    }

    in.close();
}

void findPrime(Node* head, int a, int b)
{
    Node* curr = head;
    int i = 0;
    while(i < a && curr != NULL)
    {
        curr = curr ->next;
        i++;
    }
    
    for(; i <= b; i++)
    {
        if(isPrime(curr->key))
            cout << curr->key << ' ';

        curr = curr->next;
    }
}

int main()
{
    Node* head = NULL;
    int a, b;

    readFile(head);
    cout << "Input: ";
    cin >> a >> b;
    findPrime(head, a, b);
}