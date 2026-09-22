#include <iostream>
#include <cstring>
#include <stdlib.h>
#define MAX_SIZE 1000

using namespace std;

struct Stack
{
    char data[MAX_SIZE];
    int top = -1;
};

bool isEmpty(Stack s)
{
    return s.top < 0;
}

bool isFull(Stack s)
{
    return s.top >= MAX_SIZE - 1;
}

void push(Stack &s, char data)
{
    if (isFull(s))
    {
        cout << "Stack is overflow!";
        return;
    }

    s.top++;
    s.data[s.top] = data;
    cout << "Completed!\n\n";
}

void pop(Stack &s)
{
    if (isEmpty(s))
    {
        cout << "Stack is underflow!";
        return;
    }

    s.top--;
    cout << "Completed!\n\n";
}

void showingData(Stack s)
{
    cout << "Completed!\n";

    cout << "Data: ";
    for (int i = 0; i <= s.top; i++)
        cout << s.data[i] << ' ';

    cout << endl
         << endl;
}

void doFunction()
{
    char n;
    Stack s;
    cout << "Test Stack Function\n";
    cout << "(0) Stop the program.\n";
    cout << "(1) Add a character..\n";
    cout << "(2) Remove a character.\n";
    cout << "(3) Show data.\n\n";
    cout << "Please enter a number: ";
    cin >> n;

    while (n != '0')
    {
        if (n == '1')
        {
            char c;
            cout << "Enter a character: ";
            cin >> c;
            push(s, c);
        }

        else if (n == '2')
            pop(s);

        else if (n == '3')
            showingData(s);

        else
            cout << "\nInvalid!\n";

        cout << "(0) Stop the program.\n";
        cout << "(1) Add a character..\n";
        cout << "(2) Remove a character.\n";
        cout << "(3) Show data.\n\n";
        cout << "Please enter a number: ";
        cin >> n;
    }

    return;
}

int main()
{
    doFunction();
    return 0;
}