#pragma once
#include <iostream>
#include <exception>

using namespace std;

class Stack
{
private:
    int *arr;
    int topIndex;
    int size;

public:
    Stack(int size)
    {
        arr = new int[size];
        topIndex = -1;
        this->size = size;
    }
    ~Stack()
    {
        delete[] arr;
    }

    bool isEmpty();
    bool isFull();
    void push(int val);
    void pop();
    int getTopValue();
};

bool Stack::isEmpty()
{
    return topIndex == -1;
}

bool Stack::isFull()
{
    return topIndex == (size - 1);
}

void Stack::push(int val)
{
    try
    {
        if (isFull())
            throw overflow_error("Exception: Stack is full!\n");
        topIndex++;
        arr[topIndex] = val;
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

void Stack::pop()
{
    try
    {
        if (isEmpty())
            throw underflow_error("Exception: Stack is empty!\n");

        topIndex--;
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

int Stack::getTopValue()
{
    try
    {
        if (isEmpty())
            throw underflow_error("Exception: Stack is empty!\n");

        return arr[topIndex];
    }
    catch(const exception& e)
    {
        cout << e.what() << endl;
    }
}