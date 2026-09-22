#include <iostream>

using namespace std;

struct Node
{
    int key;
    Node* pNext;
};

// Stack
Node* createStack(int data)
{
    Node* newNode = new Node;
    newNode->key = data;
    newNode->pNext = nullptr;
    return newNode;
}

void pushStack(Node*& stack, int data)
{
    if(stack == nullptr)
        stack = createStack(data);

    else
    {
        Node* curr = stack;

        while(curr->pNext != nullptr)
            curr = curr->pNext;

        curr->pNext = createStack(data);
    }
}

void popStack(Node*& stack)
{
    if(stack == nullptr)
        return;

    Node* curr = stack;
    if(curr->pNext == nullptr)
    {
        delete curr;
        stack = nullptr;
        return;
    }

    while(curr->pNext->pNext != nullptr)
        curr = curr->pNext;

    Node* tmp = curr->pNext;
    curr->pNext = nullptr;
    delete tmp;
}

int countStackElement(Node* stack)
{
    int cnt = 0;
    Node* curr = stack;
    while(curr != nullptr)
    {
        cnt++;
        curr = curr->pNext;
    }

    return cnt;
}

bool isEmptyStack(Node* stack)
{
    return stack == nullptr;
}

// Queue
Node* createQueue(int data)
{
    Node* newNode = new Node;
    newNode->key = data;
    newNode->pNext = nullptr;
    return newNode;
}

void enqueue(Node*& queue, int data)
{
    if(queue == nullptr)
        queue = createQueue(data);

    else
    {
        Node* curr = queue;

        while(curr->pNext != nullptr)
            curr = curr->pNext;

        curr->pNext = createQueue(data);
    }
}

void dequeue(Node*& queue)
{
    if(queue == nullptr)
        return;

    Node* deleted = queue;
    queue = queue->pNext;
    delete deleted;
}

int countQueueElement(Node* queue)
{
    int cnt = 0;
    Node* curr = queue;
    while(curr != nullptr)
    {
        cnt++;
        curr = curr->pNext;
    }

    return cnt;
}

bool isEmptyStack(Node* queue)
{
    return queue == nullptr;
}

