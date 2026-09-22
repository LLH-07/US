#include <iostream>
#include <cmath>

using namespace std;

struct Node
{
    int data;
    Node* next;
};

struct List
{
    Node* head;
};

bool checkIncrease(Node* h)
{
    if(h == NULL)
        return false;
    
    if(h->next == NULL)
        return true;

    Node* curr = h;
    Node* next = curr->next;

    if(curr->data > next->data)
        return false;

    return checkIncrease(next);
}