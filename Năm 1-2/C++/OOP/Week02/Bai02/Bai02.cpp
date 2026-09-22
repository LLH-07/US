#include <iostream>
#include "Bai02.h"

using namespace std;

int main()
{
    Stack s(10);

    if(s.isEmpty())
        cout << "Stack is empty!\n";

    s.push(0);
    if(s.isEmpty())
        cout << "Stack is empty!\n";
    else cout << "Stack is not empty!\n";

    for(int i = 1; i < 10; i++)
        s.push(i);
    if(s.isFull()) cout << "Stack is full!\n";

    s.pop();
    if(s.isFull()) cout << "Stack is full!\n";
    else cout << "Stack is not full!\n";

    cout << "Top value: " << s.getTopValue() << endl;
    return 0;
}