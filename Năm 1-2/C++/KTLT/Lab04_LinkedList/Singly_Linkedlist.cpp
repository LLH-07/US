#include <iostream>

using namespace std;

struct NODE
{
    int key;
    NODE *p_next;
};

struct List
{
    NODE *p_head;
    NODE *p_tail;
};


// ***** LINK LIST WITHOUT P_TAIL *****

// Problem 1
// Initialize a NODE from a given integer
NODE *createNode(int data)
{
    NODE *new_Node = new NODE;
    new_Node->key = data;
    new_Node->p_next = NULL;

    return new_Node;
}

// Problem 2
// Insert an integer to the head of given linked list
void addHead(NODE *&pHead, int data)
{
    NODE *newNode = createNode(data);
    newNode->p_next = pHead;
    pHead = newNode;
}

// Problem 3
// Insert an integer to the tail of a given linked list
void addTail(NODE *&pHead, int data)
{
    NODE *newNode = createNode(data);

    if (pHead == NULL)
        pHead = newNode;

    else
    {
        NODE *curNode = pHead;
        while (curNode->p_next != NULL)
            curNode = curNode->p_next;

        curNode->p_next = newNode;
    }
}

// Problem 4
// Remove the first NODE of a given linked list
void removeHead(NODE *&pHead)
{
    if (pHead != NULL)
    {
        NODE *oldNode = pHead;
        pHead = pHead->p_next;
        delete oldNode;
    }
}

// Problem 5
// Remove the last NODE of a given linked list
void removeTail(NODE *&pHead)
{
    if (pHead != NULL)
    {
        if (pHead->p_next == NULL) // if list only have one element.
        {
            delete pHead;
            pHead = NULL;
        }

        else
        {
            NODE *curNode = pHead;
            NODE *nextNode = curNode->p_next;

            while (nextNode->p_next != NULL)
            {
                curNode = nextNode;
                nextNode = nextNode->p_next;
            }

            curNode->p_next = NULL;
            delete nextNode;
        }
    }
}

// Problem 6
// Remove all NODE from a given linked list
void removeAll(NODE *&pHead)
{
    NODE *curNode = pHead;

    while (curNode != NULL)
    {
        removeHead(pHead);
        curNode = pHead;
    }

    delete curNode;
}

// Problem 7
// Remove an integer before a value of a given linked list
void removeBefore(NODE *&pHead, int val)
{
    if (pHead != NULL)
    {
        if (pHead->key == val)
        {
            pHead = pHead->p_next;
            return;
        }

        // check 2 or more nodes

        NODE *preNode = pHead;
        NODE *curNode = pHead->p_next;

        // if curNode == NULL, it means preNode is the last node of link list.
        while (curNode != NULL && curNode->key != val)
        {
            if (curNode->p_next != NULL && (curNode->p_next)->key == val)
            {
                // It means you have to delete the previous the (curNode->p_next) which is curNode
                preNode->p_next = curNode->p_next;
                delete curNode;
                return;
            }

            preNode = curNode;
            curNode = curNode->p_next;
        }
    }
}

// Problem 8
// Remove an integer after a value of a given linked list
void removeAfter(NODE *&pHead, int val)
{
    if (pHead != NULL)
    {
        NODE *curNode = pHead;

        // if curNode == NULL, it means you've already traversed all nodes in list
        while (curNode != NULL)
        {
            if (curNode->key == val && curNode->p_next != NULL)
            {
                NODE *nextNode = curNode->p_next;
                curNode->p_next = nextNode->p_next;
                delete nextNode;
            }

            curNode = curNode->p_next;
        }
    }
}

// Problem 9
// Insert an integer at a position of a given linked list
// if pos starts from 1
void addPos(NODE *&pHead, int data, int pos)
{
    // bool inserted = false;
    if (pos == 1)
    {
        addHead(pHead, data);
        return;
        // inserted = true;
    }

    if (pHead != NULL)
    {
        NODE *newNode = createNode(data);
        NODE *curNode = pHead;

        int count = 1;

        // if curNode->p_next == NULL, curNode is the last Node
        while (curNode != NULL && count < pos - 1)
        {
            curNode = curNode->p_next;
            count++;
        }

        if (curNode == NULL)
        {
            if (count < pos - 1)
                return;
        }
        newNode->p_next = curNode->p_next;
        curNode->p_next = newNode;
    }
}

// Problem 10
// Remove an integer at a position of a given linked list
void RemovePos(NODE *&pHead, int pos)
{
    if (pHead != NULL)
    {
        if (pos == 1)
            removeHead(pHead);

        NODE *curNode = pHead;
        int i = 1;

        while (curNode != NULL && i < pos - 1)
        {
            curNode = curNode->p_next;
            i++;
        }

        if (curNode == NULL)
            return;

        NODE *p1 = curNode->p_next; // Node need to be deleted
        NODE *p2 = p1->p_next;      // the node after deleted node

        curNode->p_next = p2;
        delete p1;
    }
}

// Problem 11
// Insert an integer before a value of a given linked list
void addBefore(NODE *&pHead, int data, int val)
{
    if (pHead != NULL)
    {
        if (pHead->key == val)
        {
            addHead(pHead, data);
            return;
        }

        NODE *newNode = createNode(data);
        NODE *curNode = pHead;

        while (curNode->p_next != NULL && (curNode->p_next)->key != val)
            curNode = curNode->p_next;

        newNode->p_next = curNode->p_next;
        curNode->p_next = newNode;
    }
}

// Problem 12
// Insert an integer after a value given linked list
void addAfter(NODE *&pHead, int data, int val)
{
    if (pHead != NULL)
    {
        NODE *newNode = createNode(data);
        NODE *curNode = pHead;

        while (curNode != NULL)
        {
            if (curNode->key == val)
            {
                newNode->p_next = curNode->p_next;
                curNode->p_next = newNode;
                return;
            }

            curNode = curNode->p_next;
        }
    }
}

// Problem 13
// Print all elements of a given linked list
void printList(NODE *&pHead)
{
    NODE *curNode = pHead;

    cout << "List includes: ";
    while (curNode != NULL)
    {
        cout << curNode->key << ' ';
        curNode = curNode->p_next;
    }
    cout << endl;
}

// Problem 14
// Count the number of elements linked list
int countElements(NODE *&pHead)
{
    int count = 0;
    NODE *curNode = pHead;
    while (curNode != NULL)
    {
        curNode = curNode->p_next;
        count++;
    }

    return count;
}

// Problem 15
// Count the number of appearances of a value in a given linked list
int countAppearance(NODE *&pHead, int val)
{
    int count = 0;
    NODE *curNode = pHead;

    while (curNode != NULL)
    {
        if (curNode->key == val)
            count++;

        curNode = curNode->p_next;
    }

    return count;
}

// Problem 16
// Create a new List by reverse a given linked list
NODE *reverseList(NODE *&pHead)
{
    NODE *preNode = NULL;
    NODE *curNode = pHead;
    NODE *nextNode;

    while (curNode != NULL)
    {
        nextNode = curNode->p_next;
        curNode->p_next = preNode;
        preNode = curNode;
        curNode = nextNode;
    }
    pHead = preNode;
}

// Problem 17
// Remove all duplicates from a given linked list
void removeDuplicate(NODE* &pHead)
{
    if(pHead == NULL || pHead->p_next == NULL)
        return;

    NODE* curr = pHead;
    while(curr != NULL)
    {
        NODE* traversal = curr;

        while(traversal->p_next != NULL)
        {
            if(traversal->key == curr->key && traversal != curr)
            {
                NODE* deleted = traversal;
                traversal = deleted->p_next;

                delete deleted;
            }

            else 
                traversal = traversal->p_next;
        }

        curr = curr->p_next;
    }
}

// Problem 18
// Remove all key value from a given linked list
bool removeElement(NODE* &pHead, int key)
{
    NODE* curr = pHead;
    int pos = 1;
    bool removed = false;
    while(curr != NULL)
    {
        if(curr->key == key)
        {
            curr = curr->p_next;
            RemovePos(pHead, pos);
            removed = true;
        }

        else curr = curr->p_next;
        pos++;
    }

    return removed;
}


// ***** LINK LIST WITH P_TAIL *****

// Problem 1
// Initialize a List from a give NODE
List createList(NODE* p_node)
{
    List list;

    if(p_node == NULL)
    {
        list.p_head = NULL;
        list.p_tail = NULL;
        return list;
    }

    list.p_head = p_node;
    list.p_tail = p_node;

    while (list.p_tail -> p_next != NULL)
        list.p_tail = list.p_tail -> p_next;
    
    return list;
}

// Problem 2
// Insert an integer to the head of a given List
bool addHead(List* &L, int data)
{
    NODE* newNode = new NODE;
    newNode->key = data;
    newNode->p_next = L->p_head;
    L->p_head = newNode;

    // If list is empty
    if(L->p_tail == NULL)
        L->p_tail = newNode;

        return true;
}

// Problem 3
// Insert an integer to the tail of a given List
bool addTail(List* &L, int data)
{
   NODE* newNode = new NODE;
   newNode->key = data;

    if(L->p_head == NULL)
    {
        L->p_head = newNode;
        L->p_tail = newNode; 
    }

    else
    {
        (L->p_tail)->p_next = newNode;
        L->p_tail = newNode;
    }

    return true;
}

// Problem 4
// Remove the first NODE of a given List
void removeHead(List* &L)
{
    if(L->p_head != NULL)
    {
        NODE* curr = L->p_head;
        L->p_head = curr->p_next;

        // If list only have one element
        if(L->p_head == NULL)
            L->p_tail = NULL;

        delete curr;
    }
}

// Problem 5
// Remove the last NODE of a given List
void removeTail(List* &L)
{
    if(L->p_tail != NULL)
    {
        // If list only have one node
        if(L->p_head == L->p_tail)
        {
            delete L->p_head;
            L->p_head = NULL;
            L->p_tail = NULL;
            return;
        }

        NODE* curr = L->p_head;
        while(curr->p_next != L->p_tail)
            curr = curr->p_next;

        L->p_tail = curr;
        curr = curr->p_next;
        delete curr;
    }
}

// Problem 6
// Remove all NODE from a given List
void removeAll(List* &L)
{
    NODE* curr = L->p_head;
    while(curr != NULL)
    {
        removeHead(L);
        curr = L->p_head;
    }
    L->p_tail = NULL;
}

// Problem 7
// Remove an integer before a value of a given List
void removeBefore(List* &L, int val)
{
    // One or no element in list OR the first node of the list's key = val  -> can't remove before
    if(L->p_head == NULL || L->p_head->p_next == NULL|| L->p_head->key == val)
        return;

    NODE* prev = L->p_head;
    NODE* curr = L->p_head->p_next;

    if(curr->key == val)
        removeHead(L);

    while(curr->p_next != NULL)
    {
        if(curr->p_next->key == val)
        {
            prev->p_next = curr->p_next;
            delete curr;
            return;
        }

        prev = curr;
        curr = curr->p_next;
    }
}

// Problem 8
// Remove an integer after a  value of a given list
void removeAfter(List* &L, int val)
{
    if(L->p_head == NULL)
        return;

    NODE* curr = L->p_head;
    NODE* next = curr->p_next;

    // Check until curr is the last node of the list
    while(next != NULL)
    {
        if(curr->key == val)
        {
            if(next->p_next == NULL)
                L->p_tail = curr;

            curr->p_next = next->p_next;
            delete next;
            return;
        }
        
        curr = next;
        next = curr->p_next;
    }
}

// Problem 9
// Insert an integer at a position of a given List
bool addPos(List* &L, int data, int pos)
{
    if(pos == 1)
    {
        addHead(L, data);
        return true;
    }

    if(L->p_head == NULL)
        return false;

    int i = 1;
    NODE* newNode = createNode(data);
    NODE* curr = L->p_head;

    while(curr != NULL && i < pos - 1)
    {
        curr = curr->p_next;
        i++;
    }

    if(curr != NULL)
    {
        newNode->p_next = curr->p_next;
        curr->p_next = newNode;
        return true;
    }

    return false;
}

// Problem 10
// Remove an integer at a position of a given List
void RemovePos(List* &L, int pos)
{
    if(L->p_head == NULL)
        return;

    if(pos == 1)
    {
        removeHead(L);
        return;
    }

    NODE* curr = L->p_head;
    int i = 1;
    
    while(curr != NULL && i < pos - 1)
    {
        curr = curr->p_next;
        i++;
    }

    if(curr != NULL)
    {
        NODE* deletedNode = curr->p_next;
        curr->p_next = deletedNode->p_next;
        delete deletedNode;
    }
}

// Problem 11
// Insert an integer before a value of a given List
bool addBefore(List* &L, int data, int val)
{
    if(L->p_head == NULL || L->p_head->p_next == NULL || L->p_head->key == val)
        return false;

    NODE* curr = L->p_head->p_next;
    int i = 2;

    while(curr != NULL)
    {
        if(curr->key == val)
        {
            addPos(L, data, i - 1);
            return true;
        }
        curr = curr->p_next;
        i++;
    }

    return false;
}

// Problem 12
// Insert an integer after a value of a given List
bool addAfter(List* &L, int data, int val)
{
    if(L->p_head == NULL)
        return false;

    NODE* curr = L->p_head;
    int i = 1;
    
    while(curr != NULL)
    {
        if(curr->key == val)
        {
            addPos(L, data, i + 1);
            return true;
        }

        curr = curr->p_next;
        i++;
    }

    return false;
}

// Problem 13
// Print all elements of a given List
void printList(List* &L)
{
    NODE* curr = L->p_head;
    cout << "List includes: ";
    while(curr != NULL)
    {
        cout << curr->key << " ";
        curr = curr->p_next;
    }
    cout << endl;
}

// Problem 14
// Count the number of elements List
int countElements(List* L)
{
    int count = 0;
    NODE* curr = L->p_head;
    while(curr != NULL)
    {
        count++;
        curr = curr->p_next;
    }
    return count;
}

// Problem 15
// Count the number of appearance of a value in a given linked list
int countAppearance(List* L, int value)
{
    NODE* curr = L->p_head;
    int count = 1;

    while(curr != NULL)
    {
        if(curr->key == value)
            count++;

        curr = curr->p_next;
    }

    return count;
}


// Problem 16
// Create a new List by reverse a given List
List* reverseList(List* &L)
{
    List* newList = &createList(NULL);

    if(L->p_head == NULL)
        return newList;

    // There's only one element in List
    if(L->p_head->p_next == NULL)
        return L;
    
    NODE* prev = NULL;
    NODE* curr = L->p_head;
    NODE* next;

    while(curr != NULL)
    {
        next = curr->p_next;
        curr->p_next = prev;
        prev = curr;
        curr = next;
    }

    newList->p_head = prev;
    newList->p_tail = L->p_head;

    return newList;
}

// Problem 17
// Remove all duplicates from a given List
void removeDuplicate(List* &L)
{
    if(L->p_head == NULL || L->p_head->p_next == NULL)
        return;

    NODE* prev = NULL;
    NODE* curr = L->p_head;
     while(curr != NULL)
    {
        NODE* prev_traversal = curr;
        NODE* traversal = curr->p_next;

        while(traversal != NULL)
        {
            if(traversal->key == curr->key)
            {
                NODE* deleted = traversal;
                prev_traversal->p_next = traversal->p_next;
                traversal = deleted->p_next;
                
                delete deleted;
            }

            else 
            {
                prev_traversal = traversal;
                traversal = traversal->p_next;
            }
        }

        prev = curr;
        curr = curr->p_next;
    }

    L->p_tail = prev;
} 

// Problem 18
// Remove all key value from a given List
bool removeElement(List* &L, int key)
{
    if(L->p_head == NULL)
        return false;

    NODE* curr = L->p_head;
    bool found = false;

    while(curr != NULL)
    {
        if(curr->key == key)
        {
            NODE* deleted = curr;
            curr = curr->p_next;
            found = true;
            delete deleted;
        }

        else curr = curr->p_next;
    }

}


int main()
{
    NODE *head = NULL;
    for (int i = 0; i < 8; i++)
        addHead(head, i);

    printList(head);

    RemovePos(head, 4);

    printList(head);
}