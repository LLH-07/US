#include <iostream>

using namespace std;

struct Node
{
    Node* prev;
    int data;
    Node* next;
};

Node* head = NULL;
Node* tail = NULL;

Node* getNewNode(int x)
{
    Node* newNode = new Node();
    newNode->prev = NULL;
    newNode->data = x;
    newNode->next = NULL;
    return newNode;
}

void InsertAtHead(int x)
{
    Node* newNode = getNewNode(x);
    if( head == NULL )
    {
        head = newNode;
        return;
    }

    head->prev = newNode;
    newNode->next = head;
    head = newNode;

}

void InsertAtTail(int x)
{
    Node* newNode = getNewNode(x);
    Node* temp = head;

    if( head == NULL )
    {
        head = newNode;
        return;
    }

    while ( temp->next != NULL )
        temp = temp->next;

    newNode->prev = temp;
    temp->next = newNode;

}

void InsertAt(int x, int pos)
{
    Node* newNode = getNewNode(x);
    Node* temp = head;

    pos--;

    if(pos == 0)
    {
        InsertAtHead(x);
        return;
    }

    for( int i = 0; i < pos - 2; i++ )
        temp = temp->next;

    // When finishing loop, temp is at (pos - 1)
    newNode->next = temp->next;
    newNode->prev = temp;
    temp->next = newNode;

    temp = newNode->next;
    temp->prev = newNode; 

}

void deleteAtHead()
{
    Node* temp = head;

    temp = temp->next;
    temp->prev = NULL;
    head = temp;

    delete temp->prev;
}

void deleteAtTail()
{
    Node* temp = head;

    while( temp->next != NULL )
        temp = temp->next;

    temp = temp->prev;
    delete temp->next;
    temp->next = NULL;
}

void deleteAt(int pos)
{
    Node* temp;
    temp = head;


    if(pos == 0)
    {
        deleteAtHead();
        return;
    }


    for( int i = 1; i <= pos - 2; i++ )
        temp = temp->next;

    //temp at position (pos - 1)

    Node* temp1;
    temp1 = temp->next; //temp1 at (pos)
    temp->next = temp1->next;

    temp = temp->next;
    temp->prev = temp1->prev;

    delete temp1;

}

void Reverse()
{
    Node* prev;
    Node* current;
    Node* next;

    prev = NULL;
    current = head;
    
    while( current != NULL )
    {
        next = current->next;
        current->prev = next;
        current->next = prev;
        prev = current;
        current = next;
    }

    head = prev;
}

void print()
{
    Node* temp = head;

    cout << "List includes: ";

    while( temp->next != NULL )
    {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << temp->data;
    
    
    cout << endl;

    cout << "Reverse list: ";
    while( temp != NULL )
    {
        cout << temp->data << " ";
        temp = temp->prev;
    }
    
    

    cout << endl << endl;
}

int main()
{
   
    InsertAtTail(1);
    InsertAtTail(2);
    InsertAtTail(3);
    InsertAtTail(4);
    InsertAtTail(5);
    InsertAtTail(6);
    
    print();
    /*
    InsertAtHead(1);
    InsertAtHead(2);
    InsertAtHead(3);
    InsertAtHead(4);
    InsertAtHead(5);
    print();
    */

    Reverse();
    print();
}