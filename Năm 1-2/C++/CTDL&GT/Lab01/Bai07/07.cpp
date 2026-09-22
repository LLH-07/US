#include <iostream>
#define MAX_SIZE 1000

using namespace std;

struct queue
{
    int data[MAX_SIZE];
    int front = -1;
    int rear = -1;
};

bool isEmpty(queue q)
{
    return (q.front == -1 && q.rear == -1);
}

bool isFull(queue q)
{
    return q.rear >= MAX_SIZE - 1;
}

void enqueue(queue &q, int data)
{
    if (isFull(q))
    {
        cout << "\nQueue is overflow!\n\n";
        return;
    }
    q.rear++;
    q.data[q.rear] = data;

    cout << "Completed!";
}

void dequeue(queue &q)
{
    if (isEmpty(q))
    {
        cout << "\nQueue in underflow!\n\n";
        return;
    }

    else if (q.front == q.rear)
    {
        q.front = -1;
        q.rear = -1;
    }

    else
        q.front++;

    cout << "Completed!";
}

void showingData(queue q)
{
    cout << "Completed!\n";
    cout << "Data: ";
    for (int i = q.front; i <= q.rear; i++)
        cout << q.data[i] << ' ';

    cout << endl;
}

void doFunction()
{
    int n;
    queue q;
    cout << "Test Queue Function\n";
    cout << "(0) Stop the program.\n";
    cout << "(1) Add a number..\n";
    cout << "(2) Remove a number.\n";
    cout << "(3) Show data.\n\n";
    cout << "Please enter a number: ";
    cin >> n;

    while (n != 0)
    {
        if (n == 1)
        {
            int a;
            cout << "Enter a number: ";
            cin >> a;
            enqueue(q, a);
        }

        else if (n == 2)
            dequeue(q);

        else if (n == 3)
            showingData(q);

        else
            cout << "\nInvalid!\n\n";

        cout << "(0) Stop the program.\n";
        cout << "(1) Add a number..\n";
        cout << "(2) Remove a number.\n";
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
