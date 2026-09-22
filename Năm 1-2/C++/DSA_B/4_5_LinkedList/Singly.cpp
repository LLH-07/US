#include <iostream>

using namespace std;

struct Node
{
    int key;
    Node *p_next;
};

struct List
{
    Node *p_head;
    Node *p_tail;
};

Node *createNode(int data)
{
    Node *newNode = new Node;
    newNode->key = data;
    newNode->p_next = NULL;
    return newNode;
}

List *createList(Node *p_node)
{
    List *newList = new List;
    newList->p_head = p_node;
    newList->p_tail = p_node;
    return newList;
}

bool addHead(List *&L, int data)
{
    Node *newNode = createNode(data);

    if (L == nullptr)
        L = createList(newNode);

    else
    {
        newNode->p_next = L->p_head;
        L->p_head = newNode;
    }

    return true;
}

bool addTail(List *&L, int data)
{
    Node *newNode = createNode(data);

    if (L == nullptr)
        L = createList(newNode);

    else
    {
        L->p_tail->p_next = newNode;
        L->p_tail = newNode;
    }

    return true;
}

void removeHead(List *&L)
{
    if (L == nullptr || L->p_head == nullptr)
        return;

    Node *tmp = L->p_head;

    L->p_head = tmp->p_next;

    delete tmp;
}

void removeTail(List *&L)
{
    if (L == nullptr || L->p_head == nullptr)
        return;

    Node *tmp = L->p_head;

    if (tmp == L->p_tail)
    {
        delete tmp;
        L->p_head = L->p_tail = nullptr;
        return;
    }

    while (tmp->p_next != L->p_tail)
        tmp = tmp->p_next;

    delete L->p_tail;
    tmp->p_next = nullptr;
    L->p_tail = tmp;
}

void removeAll(List *&L)
{
    while (L != nullptr && L->p_head != nullptr)
        removeHead(L);

    return;
}

void removeBefore(List *&L, int val)
{
    if (L == nullptr || L->p_head == nullptr || L->p_head->p_next == nullptr || L->p_head->key == val)
        return;

    Node *prev = nullptr;
    Node *curr = L->p_head;
    Node *next = curr->p_next;

    if (next != nullptr && next->key == val)
    {
        removeHead(L);
        return;
    }

    while (next != nullptr && next->key != val)
    {
        prev = curr;
        curr = next;
        next = next->p_next;
    }

    if (next != nullptr && next->key == val)
    {
        prev->p_next = next;
        delete curr;
    }
}

void removeAfter(List *&L, int val)
{
    if (L == nullptr || L->p_head == nullptr || L->p_head->p_next == nullptr)
        return;

    Node *curr = L->p_head;
    Node *next = curr->p_next;

    while (next->p_next != nullptr && curr->key != val)
    {
        curr = next;
        next = next->p_next;
    }

    if (curr->key == val)
    {
        curr->p_next = next->p_next;
        delete next;
    }

    return;
}

bool addPos(List *&L, int data, int pos)
{
    if (pos < 0 || L == nullptr)
        return false;
    if (pos == 0)
    {
        addHead(L, data);
        return true;
    }

    int i = 0;
    Node *curr = L->p_head;
    Node *next = curr->p_next;
    Node *newNode = createNode(data);

    while (i < pos && curr != nullptr)
    {
        curr = next;
        next = next->p_next;
        i++;
    }

    if (i == pos)
    {
        curr->p_next = newNode;
        newNode->p_next = next;

        if (curr == nullptr)
            L->p_tail = newNode;

        return true;
    }

    delete newNode;
    return false;
}

void removePos(List *&L, int data, int pos)
{
    if (pos < 0 || L == nullptr || L->p_head == nullptr)
        return;

    if (pos == 0)
    {
        removeHead(L);
        return;
    }

    int i = 0;
    Node *tmp = L->p_head;

    while (tmp->p_next != nullptr && i < pos - 1)
    {
        tmp = tmp->p_next;
        i++;
    }

    if (i == pos - 1 && tmp->p_next != nullptr)
    {
        Node *deleted = tmp->p_next;
        tmp->p_next = deleted->p_next;
        delete deleted;
    }
}

bool addBefore(List *&L, int data, int val)
{
    if (L == nullptr || L->p_head == nullptr)
        return false;

    Node *newNode = createNode(data);
    Node *prev = nullptr;
    Node *curr = L->p_head;

    if (L->p_head->key == val)
    {
        newNode->p_next = curr;
        L->p_head = newNode;
        return true;
    }

    while (curr != nullptr && curr->key != val)
    {
        prev = curr;
        curr = curr->p_next;
    }

    if (curr != nullptr && curr->key == val)
    {
        prev->p_next = newNode;
        newNode->p_next = curr;
        return true;
    }

    delete newNode;
    return false;
}

bool addAfter(List *&L, int data, int val)
{
    if (L == nullptr || L->p_head == nullptr)
        return false;

    Node *newNode = createNode(data);
    Node *curr = L->p_head;

    while (curr != nullptr && curr->key != val)
        curr = curr->p_next;

    if (curr != nullptr && curr->key == val)
    {
        Node *next = curr->p_next;

        curr->p_next = newNode;
        newNode->p_next = next;

        if (next == nullptr)
            L->p_tail = newNode;

        return true;
    }

    delete newNode;
    return false;
}

void printList(List *L)
{
    Node *tmp = L->p_head;

    cout << "List includes: ";
    while (tmp != nullptr)
    {
        cout << tmp->key << ' ';
        tmp = tmp->p_next;
    }
    cout << endl;
}

int countElements(List *L)
{
    int cnt = 0;
    Node *tmp = L->p_head;

    while (tmp != nullptr)
    {
        cnt++;
        tmp = tmp->p_next;
    }

    return cnt;
}

List *reverseList(List *L)
{
    if (L == nullptr || L->p_head == nullptr)
        return nullptr;

    List *newList = createList(L->p_tail);
    newList->p_tail = L->p_head;

    Node *prev = nullptr;
    Node *curr = L->p_head;
    Node *next;

    while (curr != nullptr)
    {
        next = curr->p_next;
        curr->p_next = prev;
        prev = curr;
        curr = next;
    }

    return newList;
}

void removeDuplicate(List *&L)
{
    if (L == nullptr || L->p_head == nullptr)
        return;


    Node *curr = L->p_head;
    Node *next;
    int pos = 0;

    while (curr != nullptr)
    {
        next = curr->p_next;
        if (next != nullptr && curr->key == next->key)
            removePos(L, curr->key, pos + 1);

        else
        {
            curr = curr->p_next;
            pos++;
        }
    }
}

void removeElement(List *&L, int key)
{
    if(L == nullptr || L->p_head == nullptr)
        return;
    
    Node* curr = L->p_head;
    Node* next;
    int pos = 0;

    while(curr != nullptr)
    {
        next = curr->p_next;
        if(curr->key == key)
            removePos(L, key, pos);

        else
            pos++;

        curr = next;
    }
}

int main()
{
    List* l = nullptr;

    addTail(l, 5);
    addTail(l, 5);
    addTail(l, 5);
    addTail(l, 5);
    addTail(l, 4);
    addTail(l, 5);
    addTail(l, 6);

    cout << "Before:\n";
    printList(l);


    removeElement(l,5);
    cout << "After:\n";
    printList(l);


}
