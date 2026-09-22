#pragma once
#include <iostream>

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
    if(isFull())
    {
        cout << "Stack is full!\n";
        return;
    }
    
    topIndex++;
    arr[topIndex] = val;
}

void Stack::pop()
{
    if(isEmpty())
    {
        cout << "Stack is empty!\n";
        return;
    }
    
    topIndex--;
}

int Stack::getTopValue()
{
    if(isEmpty)
    {
        cout << "Stack is empty!\n";
        return -1;
    }

    return arr[topIndex];
}