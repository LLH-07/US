#include <iostream>

using namespace std;

struct Node
{
    int data;
    Node *next;
};

Node *createNode(int val)
{
    Node *newNode = new Node;
    newNode->data = val;
    newNode->next = NULL;
    return newNode;
}

void addTail(Node *&head, int val)
{
    if (head == NULL)
    {
        head = createNode(val);
        return;
    }

    Node *newNode = createNode(val);
    Node *temp = head;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = newNode;
}

bool findVal(Node *head[], int n, int val)
{
    int pos = val % n;
    Node *temp = head[pos];
    while (temp != NULL)
    {
        if (temp->data == val)
            return true;

        temp = temp->next;
    }

    return false;
}

int main()
{
    int n;
    cout << "n =  ";
    cin >> n;

    Node *(*head) = new Node *[n];
    for(int i = 0; i < n; i++)
        head[i] = NULL;

    int x;
    for (int i = 0; i < n; i++)
    {
        cout << i + 1 << ". ";
        cin >> x;
        addTail(head[x % n], x);
    }

    cout << "Value need to be found: ";
    cin >> x;

    if(findVal(head,n ,x))
        cout << "FOUND!";

    else cout << "NOT EXIST!";

    delete[] head;

    return 0;
}