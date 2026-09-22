#include "Fraction.h"
#include "Array.h"
#include "FileNotFoundException.h"
#include "IntegerFormatException.h"
#include "DateFormatException.h"
#include "TimeFormatException.h"
#include "Stack.h"
#include "Queue.h"

using namespace std;

int main()
{
    // Bai 1
    cout << "Bai 1\n";
    Fraction ps;
    int a, b;
    cout << "Numerator: ";
    cin >> a;
    cout << "Denominator: ";
    cin >> b;
    try
    {
        if (b == 0)
            throw DividedByZeroException();

        ps.setValue(a, b);
    }
    catch (DividedByZeroException &ex)
    {
        cout << ex.what() << endl;
    }

    // Bai 2
    cout << "\nBai 2\n";
    int size;
    cout << "Size: ";
    cin >> size;
    Array arr(size);
    cout << "Array: ";
    arr.insertValue();
    int index;
    cout << "Get value at index: ";
    cin >> index;

    try
    {
        if (index < 0 || index >= size)
            throw IndexOutOfRangeException(index, size);

        arr.getValueAt(index);
    }
    catch (IndexOutOfRangeException &e)
    {
        cout << e.what() << '\n';
    }

    // Bai 3
    cout << "\nBai 3\n";
    string filename;
    cin.ignore();
    cout << "Filename: ";
    getline(cin, filename);
    try
    {
        fstream f(filename);
        if (!f.is_open())
            throw FileNotFoundException(filename);

        f.close();
    }
    catch (FileNotFoundException &e)
    {
        cout << e.what() << '\n';
    }

    // Bai 4
    cout << "\nBai 4\n";
    cout << "Input string: ";
    string intStr;
    cin >> intStr;

    try
    {
        for (int i = 0; i < intStr.size(); i++)
            if (intStr[i] < 48 || intStr[i] > 57)
                throw IntegerFormatException(intStr);
    }
    catch (IntegerFormatException &e)
    {
        cout << e.what() << '\n';
    }

    // Bai 5
    cout << "\nBai 5\n";
    string date;
    cout << "Date: ";
    cin >> date;

    try
    {
        if (!checkValidDate(date))
            throw DateFormatException();

        cout << date << endl;
    }
    catch (DateFormatException &e)
    {
        cout << e.what() << '\n';
    }

    // Bai 6
    cout << "\nBai 6\n";
    string time;
    cout << "Time: ";
    cin >> time;

    try
    {
        if (!isValidTime(time))
            throw TimeFormatException();

        cout << time << endl;
    }
    catch (TimeFormatException &e)
    {
        cout << e.what() << endl;
    }

    // Bai 7
    cout << "\nBai 7\n";
    Stack s(5);
    Queue q(5);

    for (int i = 0; i < 7; i++)
    {
        s.push(i);
        q.enqueue(i);
    }

    cout << "-------------------\n";

    for (int i = 0; i < 7; i++)
    {
        s.pop();
        q.dequeue();
    }
    cout << "-------------------\n";

    int s1 = s.getTopValue();
    int q1 = q.getFrontValue();
}