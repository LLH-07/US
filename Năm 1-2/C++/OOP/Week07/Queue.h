#pragma once
#include <iostream>
#include <exception>

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
    try
    {
        if (isFull())
            throw overflow_error("Exception: Queue is full!\n");

        if (isEmpty())
            frontIndex = rearIndex = 0;

        else
            rearIndex++;

        arr[rearIndex] = val;
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

void Queue::dequeue()
{
    try
    {
        if (isEmpty())
            throw underflow_error("Exception: Queue is empty!\n");

        if (rearIndex == 0)
            frontIndex = rearIndex = -1;

        else
            rearIndex--;
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

int Queue::getFrontValue()
{
    try
    {
        if (isEmpty())
            throw underflow_error("Exception: Queue is empty!\n");

        return arr[frontIndex];
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}
