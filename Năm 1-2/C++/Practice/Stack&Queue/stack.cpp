#include <iostream>
#include <cstring>
#include <stack>
#define MaxSize 1000

using namespace std;

// STACK

// Array based implementation

int a[MaxSize];
int top = -1; // Empty stack

void PushA(int x)
{
    if ( top == MaxSize ) 
    {
        cout << "Stack overflow\n";
        return;
    }

    top++;
    a[top] = x;
}

void PopA()
{
    if ( top == -1 )
    {
        cout << "Error: No element to pop\n";
        return;
    }
    top--;
}

bool isEmptyA()
{
    if ( top == -1 )
        return true;

    return false; 
}

int TopA()
{
    return a[top];
}

void PrintArray()
{
    cout << "Stack: ";
    for ( int i = 0; i <=  top; i++ )
        cout << a[i] << ' ';
    
    cout << endl;
}

// Linked List implementation

struct Node
{
    int data;
    Node* link;
};

Node* Top = NULL; // Top = head

void PushLL(int x)
{
    Node* newNode = new Node();
    newNode->data = x;
    newNode->link = Top;
    Top = newNode;
}

void PopLL()
{
    Node* temp;

    if (Top == NULL)
    {
        cout << "Error: No element to pop\n";
        return;
    }

    temp = Top;
    Top = temp->link;
    delete temp;
}

void PrintLL()
{
    Node* temp = Top;
    cout << "Stack: ";
    while ( temp != NULL )
    {
        cout << temp->data << " ";
        temp = temp->link;
    }
    cout << endl;
}

// STRING REVERSAL USING STACK

/*
class Stack
{
    private:
        char A[101];
        int top;

    public:
        void Push(int x);
        void Pop();
        int Top();
        bool isEmpty();
};
*/


// Other knowledge: C[] is alternative way for *C in C++
void ReverseA(char *C, int len)
{
    // Stack S;
    // Code below will use stack from standard template library which is declared above
    
    // By using stack library, we'll have an available Stack's class.
    // To create an object, declare "stack<Data_type> name"
    // Only that statement, we create a stack of characters

    stack<char> S;
    
    // loop for push;
    for ( int i = 0; i < len; i++ )
        S.push(C[i]);

    // loop for pop
    for ( int i = 0; i < len; i++ )
    {
        C[i] = S.top();
        S.pop();
    }
}

/*
int main()
{
    char C[51];
    cout << "Enter a string: ";
    gets(C);
    ReverseA(C, strlen(C));
    cout << "Output:  " << C;
}
*/

// REVERSE A LINKED LIST

Node* head = NULL;

// ...insert node

void ReverseLL()
{
    if ( head == NULL )
        return;

    stack<Node*> S;
    Node* temp = head;

    while ( temp != NULL )
    {
        S.push(temp);
        temp = temp->link;
    }

    // After while loop, value of temp is NULL
    temp = S.top();
    head = temp;
    S.pop();

    while ( !S.empty() )
    {
        temp->link = S.top();
        S.pop();
        temp = temp->link;
    }

    temp->link = NULL;

}