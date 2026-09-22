#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
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
    Node *curr = NULL;
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

    in.close();
}

int countNode(Node *head)
{
    int num = 0;
    if (head == NULL)
        return num;

    Node *curr = head;
    while (curr != NULL)
    {
        num++;
        curr = curr->next;
    }

    return num;
}

int compare2Lists(Node *headA, Node *headB)
{
    if (headA == NULL && headB == NULL)
        return 0;

    if (headA == NULL)
        return 2;

    if (headB == NULL)
        return 1;

    Node *currA = headA;
    Node *currB = headB;

    while (currA != NULL && currB != NULL)
    {
        if (currA->key > currB->key)
            return 1;

        else if (currA->key < currB->key)
            return 2;

        currA = currA->next;
        currB = currB->next;
    }

    if (countNode(headA) == countNode(headB))
        return 0;

    if (currA == NULL)
        return 2;

    if (currB == NULL)
        return 1;
}

void deleteList(Node *&head)
{
    Node *curr = head;
    while (curr != NULL)
    {
        Node *temp = curr->next;
        delete curr;
        curr = temp;
    }
}

int main()
{
    Node *headA = NULL;
    Node *headB = NULL;

    readFile("listA.txt", headA);
    readFile("listB.txt", headB);

    int result = compare2Lists(headA, headB);
    if (result == 0)
        cout << "IDENTICAL";
    else if (result == 1)
        cout << "BIGGER";
    else
        cout << "SMALLER";
        
    return 0;
}