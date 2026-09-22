#pragma once
#include <iostream>

using namespace std;

template <class T>
class Stack
{
private:
    T *data;
    int topIndex;
    int capacity;

public:
    Stack()
    {
        data = new T[10];
        topIndex = -1;
        this->capacity = 10;
    }
    Stack(int n)
    {
        data = new T[n];
        topIndex = -1;
        this->capacity = n;
    }
    ~Stack()
    {
        delete[] data;
    };

    void clear();
    bool isFull();
    bool isEmpty();
    void push(const T &item);
    T pop();
    T getTop();
};

template <class T>
bool Stack<T>::isEmpty()
{
    return topIndex < 0;
}

template <class T>
bool Stack<T>::isFull()
{
    return topIndex == (capacity - 1);
}

template <class T>
void Stack<T>::push(const T &val)
{
    if (isFull())
    {
        cout << "Stack is full!\n";
        return;
    }

    topIndex++;
    data[topIndex] = val;
}

template <class T>
T Stack<T>::pop()
{
    if (isEmpty())
    {
        cout << "Stack is empty!\n";
        return T();
    }

    topIndex--;
    return data[topIndex + 1];
}

template <class T>
T Stack<T>::getTop()
{
    if (isEmpty())
    {
        cout << "Stack is empty!\n";
        return T();
    }

    return data[topIndex];
}

template <class T>
void Stack<T>::clear()
{
    topIndex = -1;
}
