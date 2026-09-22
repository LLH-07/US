#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;


int a[1000], n;
int front = -1, rear = -1;

bool isEmpty()
{
    if ( front == -1 && rear == -1 )
        return true;

    return false;
}

void Enqueue(int x)
{
    if ( ( rear + 1 ) % n == front ) // is full
        return;

    else if ( isEmpty() )
    {
        front = 0;
        rear = 0;
    }

    else
        rear = ( rear + 1 ) %  n;
        
    a[rear] = x;
}

void Dequeue()
{
    if ( isEmpty() )
        return;

    else if ( ( front == 0 ) && ( rear == 0 ) )
    {
        front = -1;
        rear = -1;
    }

    else
        front = ( front + 1 ) % n;
}

int Front()
{
    if ( !isEmpty() )
        return -1;

    return a[front];
}

void Print()
{
    cout << "Queue includes: ";
    for ( int i = front; i < rear; i++ )
        cout << a[i] << " ";

    cout << endl;
}

int main()
{
    cout << "Input n: ";
    cin >> n;

    srand(time(NULL));

    int temp;

    for ( int i = 0; i < n; i++ )
    {
        temp = rand() % 20;
        Enqueue(temp);
        Print();
    }
    Dequeue();
    Print();


    return 0;
}