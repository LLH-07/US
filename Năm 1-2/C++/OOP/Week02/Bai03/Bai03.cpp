#include <iostream>
#include "Bai03.h"

using namespace std;

int main()
{
    Queue q(10);

    if(q.isEmpty()) cout << "Queue is empty!\n";
    else cout << "Queue is not empty!\n";

    for(int i = 0; i < 10; i++)
        q.enqueue(i);

    if(q.isFull()) cout << "Queue is full!\n";
    else cout << "Queue is not full!";

    q.dequeue();
    if(q.isFull()) cout << "Queue is full!\n";
    else cout << "Queue is not full!\n";

    cout << "Front value: " << q.getFrontValue() << endl;

}