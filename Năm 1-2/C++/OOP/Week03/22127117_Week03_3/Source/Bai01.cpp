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
    integerStack.clear();
    if(integerStack.isEmpty()) cout << "Stack is empty\n";

    //----------------------------------------------------------

    cout << endl;
    Fraction f(3, 4);
    Stack<Fraction> fStack;
    fStack.push(f);
    if (fStack.isEmpty())
        cout << "Stack is empty!\n";
    else
        cout << "Stack is not empty!\n";

    while (!fStack.isFull())
    {
        cout << "Top element: " << fStack.getTop() << endl;
        f += fStack.getTop();
        fStack.push(f);
    }
    if(fStack.isFull()) cout << "Top element: " << fStack.getTop() << " - Stack is full!" << endl;
    fStack.pop();
    cout << "Top element after popping top element: " << fStack.getTop() << endl << endl;
}