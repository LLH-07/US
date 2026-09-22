#include <iostream>
#include <exception>
#include <string>
#include <sstream>

using namespace std;

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
        stringstream stream;
        stream << "Exception: Index out of range" << endl;
        stream << "Detail: index = " << index << ", capacity = "
               << capacity << endl;
        return _strdup(stream.str().c_str());
    }
};
int main()
{
    int arr[] = {2, -7, 8, 9};
    int size = 4;
    int index = 7;
    try
    {
        if (index < 0 || index >= size)
            throw IndexOutOfRangeException(index, size);
        cout << arr[index] << endl;
    }
    catch (IndexOutOfRangeException &ex)
    {
        cout << ex.what() << endl;
    }
    return 0;
}