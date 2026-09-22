#include <iostream>

using namespace std;

struct Node
{
    int data;
    Node* link;
};

Node* head;

void insertAtBeginning(int x)
{
    Node* temp = new Node();
    temp->data = x;
    temp->link = head;
    head = temp;
}

void insertAtEnding(int x)
{
    Node* temp = new Node();
    temp->data = x;
    temp->link = NULL;

    Node* temp1 = head;
    if( head == NULL ) head = temp;
    else
    {
        while( temp1->link != NULL )
        {
            temp1 = temp1->link;
        }
        temp1->link = temp;
    }
}

void insertAt(int pos, int x, int num)
{
    Node* temp = new Node();
    temp->data = x;
    temp->link = NULL;

    Node* temp1 = head;

    if(pos > num)
        cout << "Invalid" << endl;
    else
    {
        if(pos == 1) insertAtBeginning(x);
        else
        {
            for(int i = 1; i <= pos - 2; i++)
                temp1 = temp1->link;

            temp->link = temp1->link;
            temp1->link = temp;
        }
    }
}



void deleteAtBeginning()
{
    Node* temp1;
    temp1 = head;
    head = temp1->link;
    delete temp1;
}

void deleteAtEnding()
{
    Node* temp1 = head;
    Node* temp2 = temp1->link;
    while(temp2->link != NULL)
    {
        if(temp2->link != NULL) temp1 = temp2;
        temp2 = temp1->link;
    }
    delete temp2;
    temp1->link = NULL;
}

void deleteAt(int pos, int num)
{
    if(pos > num)
        cout << "Invalid";
    
    else
    {
        if(pos == 1) deleteAtBeginning();
        else
        {
            Node* temp1 = head;
            for(int i = 1; i <= pos - 2; i++)
                temp1 = temp1->link;

            Node* temp2;
            temp2 = temp1->link;
            temp1->link = temp2->link;
            delete temp2;
        }
    }
}

void Reverse()
{
    Node* pre;
    Node* current;
    Node* next;

    pre = NULL;
    current = head;

    while(current != NULL) // current = NULL -> pre = Last Node
    {
        next = current->link;
        current->link = pre;
        pre = current;
        current = next;
    }
    head = pre;
}

void Sort()
{
    Node* current;
    Node*  next;
    current = head;
    
    while(current != NULL)
    {
        next = current->link;
        while(next != NULL)
        {
            if(current->data > next->data)
                swap(current->data, next->data);

            next = next->link;
        }
        current = current->link;
    }
}

void Print()
{
    Node* temp = head;
    cout << "List includes: ";
    
    if(head == NULL)
    {
        cout << "Empty";
        return;
    }
    
    while(temp != NULL)
    {
        cout << temp->data << " ";
        temp = temp->link;
    }
    cout << endl << endl;
}

int main()
{
    head = NULL;
    int n, x, pos;
    /*cout << "How many numbers? ";
    cin >> n;
    for( int i = 0; i < n; i++ )
    {
        cout << "Enter the number: ";
        cin >> x;
        cout << "Enter position: ";
        cin >> pos;
        insertAt(pos, x, i + 1);
        Print();
    }*/
    insertAtEnding(5);
    insertAtEnding(4);
    insertAtEnding(3);
    insertAtEnding(2);
    insertAtEnding(1);
    Print();
    deleteAt(3,5);
    Print();

    return 0;
}
