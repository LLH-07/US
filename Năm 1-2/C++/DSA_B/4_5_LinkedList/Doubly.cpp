#include <iostream>

using namespace std;

struct Node
{
    int key;
    Node *pNext = nullptr;
    Node *pPrev = nullptr;
};

struct List
{
    Node *pHead;
    Node *pTail;
};

Node *createNode(int data)
{
    Node *newNode = new Node;
    newNode->key = data;
    return newNode;
}

List *createList(Node *p_node)
{
    List *newList = new List;
    newList->pHead = newList->pTail = p_node;
    return newList;
}

bool addHead(List *&L, int data)
{
    Node *newHead = createNode(data);

    if (L == nullptr)
        L = createList(newHead);

    else if (L->pHead == nullptr)
        L->pHead = L->pTail = newHead;

    else
    {
        L->pHead->pPrev = newHead;
        newHead->pNext = L->pHead;
        L->pHead = newHead;
    }

    return true;
}

bool addTail(List *&L, int data)
{
    Node *newNode = createNode(data);

    if (L == nullptr)
        L = createList(newNode);

    else if (L->pHead == nullptr)
        L->pHead = L->pTail = newNode;

    else
    {
        newNode->pPrev = L->pTail;
        L->pTail->pNext = newNode;
        L->pTail = newNode;
    }

    return true;
}

void removeHead(List *&L)
{
    if (L == nullptr || L->pHead == nullptr)
        return;

    Node *del = L->pHead;
    L->pHead = del->pNext;

    if (L->pHead != nullptr)
        L->pHead->pPrev = nullptr;

    delete del;
}

void removeTail(List *&L)
{
    if (L == nullptr || L->pHead == nullptr)
        return;

    Node *del = L->pTail;
    L->pTail = L->pTail->pPrev;

    if (L->pTail != nullptr)
        L->pTail->pNext = nullptr;

    delete del;
}

void removeAll(List *&L)
{
    while (L != nullptr && L->pHead != nullptr)
        removeHead(L);
}

void removeBefore(List *&L, int val)
{
    if (L == nullptr || L->pHead == nullptr || L->pHead->key == val)
        return;

    Node *curr = L->pHead;

    while (curr != nullptr && curr->key != val)
        curr = curr->pNext;

    if (curr != nullptr && curr->key == val)
    {
        Node *del = curr->pPrev;
        del->pPrev->pNext = curr;
        curr->pPrev = del->pPrev;

        if (del == L->pHead)
            L->pHead = curr;

        delete del;
    }
}

void removeAfter(List *&L, int val)
{
    if (L == nullptr || L->pHead == nullptr || L->pHead->key == val)
        return;

    Node *curr = L->pHead;

    while (curr != nullptr && curr->key != val)
        curr = curr->pNext;

    if (curr != nullptr && curr->pNext != nullptr && curr->key == val)
    {
        Node *del = curr->pNext;
        curr->pNext = del->pNext;

        if (del == L->pTail)
            L->pTail = curr;

        else
            del->pNext->pPrev = curr;

        delete del;
    }
}

bool addPos(List *&L, int data, int pos)
{
    if (L == nullptr || L->pHead == nullptr || pos < 0)
        return false;

    if (pos == 0)
    {
        addHead(L, data);
        return true;
    }

    int i = 0;
    Node *curr = L->pHead;
    while (curr != nullptr && i < pos)
    {
        curr = curr->pNext;
        i++;
    }

    if (curr != nullptr && i == pos)
    {
        Node *prev = curr->pPrev;
        Node *newNode = createNode(data);

        prev->pNext = newNode;
        newNode->pPrev = prev;
        newNode->pNext = curr;
        curr->pPrev = newNode;

        return true;
    }

    return false;
}

void removePos(List *&L, int data, int pos)
{
    if (L == nullptr || L->pHead == nullptr || pos < 0)
        return;

    if (pos == 0)
    {
        removeHead(L);
        return;
    }

    int i = 0;
    Node *curr = L->pHead;

    while (curr != nullptr && i < pos)
        curr = curr->pNext;

    if (curr != nullptr && i == pos)
    {
        Node *prev = curr->pPrev;
        Node *next = curr->pNext;

        if (next != nullptr)
            next->pPrev = prev;

        else
            L->pTail = prev;

        prev->pNext = next;

        delete curr;
        return;
    }
}

bool addBefore(List *&L, int data, int val)
{
    if (L == nullptr || L->pHead == nullptr)
        return false;

    if (L->pHead->key == val)
    {
        addHead(L, data);
        return true;
    }

    Node *curr = L->pHead;
    int pos = 0;

    while (curr != nullptr && curr->key != val)
    {
        curr = curr->pNext;
        pos++;
    }

    return addPos(L, data, pos - 1);
}

bool addAfter(List *&L, int data, int val)
{
    if (L == nullptr || L->pHead == nullptr)
        return false;

    Node *curr = L->pHead;
    int pos = 0;

    while (curr != nullptr && curr->key != val)
    {
        curr = curr->pNext;
        pos++;
    }

    return addPos(L, data, pos + 1);
}

void printList(List *L)
{
    Node *tmp = L->pHead;

    cout << "List includes: ";
    while (tmp != nullptr)
    {
        cout << tmp->key << ' ';
        tmp = tmp->pNext;
    }
    cout << endl;
}

int countElements(List *L)
{
    int cnt = 0;
    Node *tmp = L->pHead;

    while (tmp != nullptr)
    {
        cnt++;
        tmp = tmp->pNext;
    }

    return cnt;
}

List *reverseList(List *L)
{
    if (L == nullptr || L->pHead == nullptr)
        return L;

    List *newList = createList(nullptr);
    Node *curr = L->pTail;

    while (curr != nullptr)
    {
        addTail(newList, curr->key);
        curr = curr->pPrev;
    }

    return newList;
}

void removeDuplicate(List *&L)
{
    if (L == nullptr || L->pHead == nullptr)
        return;

    Node *curr = L->pHead;
    Node *next;

    while (curr != nullptr)
    {
        next = curr->pNext;
        if (next != nullptr && curr->key == next->key)
        {
            curr->pNext = next->pNext;
            if (next->pNext != nullptr)
                next->pNext->pPrev = curr;

            delete next;
        }

        else
            curr = curr->pNext;
    }
}

bool removeElement(List *&L, int key)
{
    if (L == nullptr || L->pHead == nullptr)
        return false;

    Node *curr = L->pHead;
    Node *next;

    while (curr != nullptr)
    {
        next = curr->pNext;
        if (curr->key == key)
        {
            if (curr->pPrev == nullptr)
                removeHead(L);
            else
            {
                curr->pPrev->pNext = next;
                if (next != nullptr)
                    next->pPrev = curr->pPrev;

                delete curr;
            }
        }

        curr = next;
    }
}

int main()
{
    List *l = nullptr;

    addTail(l, 5);
    addTail(l, 5);
    addTail(l, 3);
    addTail(l, 5);
    addTail(l, 4);
    addTail(l, 5);
    addTail(l, 6);
    addTail(l, 6);

    cout << "Before:\n";
    printList(l);

    removeElement(l, 5);
    cout << "After:\n";
    printList(l);

    return 0;
}