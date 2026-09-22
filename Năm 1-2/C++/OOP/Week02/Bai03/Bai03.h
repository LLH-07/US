#pragma once
#include <iostream>

using namespace std;

class Queue
{
private:
    int *arr;
    int frontIndex, rearIndex;
    int size;

public:
    Queue(int size)
    {
        arr = new int[size];
        frontIndex = rearIndex = -1;
        this->size = size;
    }
    ~Queue()
    {
        delete[] arr;
    }

    bool isEmpty();
    bool isFull();
    void enqueue(int val);
    void dequeue();
    int getFrontValue();
};

bool Queue::isEmpty()
{
    return frontIndex == -1;
}

bool Queue::isFull()
{
    return rearIndex == (size - 1);
}

void Queue::enqueue(int val)
{
    if(isFull())
    {
        cout << "Queue is full!\n";
        return;
    }

    if(isEmpty())
        frontIndex = rearIndex = 0;

    else rearIndex++;

    arr[rearIndex] = val;
}

void Queue::dequeue()
{
    if(isEmpty())
    {
        cout << "Stack is empty!\n";
        return;
    }

    if(rearIndex == 0)
        frontIndex = rearIndex = -1;

    else
        rearIndex--;
}

int Queue::getFrontValue()
{
    if(isEmpty())
    {
        cout << "Queue is empty!\n";
        return -1;
    }

    return arr[frontIndex];
}
