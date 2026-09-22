#include <iostream>
#include <cmath>

using namespace std;

struct Node
{
    int data;
    Node *next;
};

struct List
{
    Node *head;
};

int Count(List ls)
{
    if (ls.head == NULL || ls.head->next == NULL || ls.head->next->next == NULL)
        return 0;

    Node *prev = ls.head;
    Node *curr = prev->next;
    Node *next = curr->next;
    int count = 0;

    while (next != NULL)
    {
        if (abs(next->data - prev->data) == curr->data)
            count++;

        prev = curr;
        curr = next;
        next = next->next;
    }

    return count;
}

void addTail(int k, List &ls)
{
    Node *newNode = new Node;
    newNode->data = k;
    newNode->next = NULL;

    if (ls.head == NULL)
    {
        ls.head = newNode;
        return;
    }

    Node *curr = ls.head;
    while (curr->next != NULL)
        curr = curr->next;

    curr->next = newNode;
}

void print(Node* head)
{
    Node* curr = head;
    while(curr != NULL)
    {
        cout << curr->data << ' ';
        curr = curr->next;
    }
    cout << endl;
}

int main()
{
    int a[6] = {12, 10, 2, 9, 5, 4};
    List ls;
    ls.head = NULL;
    for(int i = 0; i < 6; i++)
        addTail(a[i], ls);

    print(ls.head);
    cout << Count(ls);
}