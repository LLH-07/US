#include "Bai01.h"
#include "Fraction.h"
#include <iostream>

using namespace std;

int main()
{
    Stack<int> integerStack;
    if (integerStack.isEmpty())
        cout << "Stack is empty!\n";
    int i = 0;
    while (!integerStack.isFull())
    {
        integerStack.push(i);
        i++;
    }
    cout << "Stack is full!\n";
    cout << "Top element: " << integerStack.getTop() << endl;
    while (!integerStack.isEmpty())
        integerStack.pop();
    cout << "Stack is empty\n";

    //----------------------------------------------------------

    Fraction f(3, 4);
    Stack<Fraction> fStack(f);
    if (integerStack.isEmpty())
        cout << "Stack is empty!\n";
    else
        cout << "Stack is not empty!\n";

    while (!integerStack.isFull())
    {
        f += fStack.getTop();
        fStack.push(f);
    }
    cout << "Stack is full!\n";
    fStack.pop();
    cout << "Top element after popping top element: " << fStack.getTop() << endl;
}