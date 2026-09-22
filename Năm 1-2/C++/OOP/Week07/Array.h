#pragma once
#include <exception>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

class Array
{
private:
    int *arr;
    int size;

public:
    Array(int size)
    {
        this->size = size;
        arr = new int[size];
    }
    ~Array()
    {
        delete[] arr;
    }
    
    int getValueAt(int index)
    {
        return arr[index];
    }
    void insertValue()
    {
        for (int i = 0; i < size; i++)
            cin >> arr[i];
    }
};

class IndexOutOfRangeException : public exception
{
private:
    int index, capacity;

public:
    IndexOutOfRangeException(int index, int capacity)
    {
        this->index = index;
        this->capacity = capacity;
    }

    const char *what() const throw()
    {
        stringstream ss;
        ss << "Exception: Index is out of range!" << endl;
        ss << "Detail:" << endl;
        ss << "index = " << index << ", capacity = " << capacity << endl;

        return strdup(ss.str().c_str());
    }
};