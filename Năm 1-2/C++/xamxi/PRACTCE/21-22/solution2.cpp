#include <iostream>

using namespace std;

typedef struct Node *Ref;

struct Node
{
    int data;
    Ref next;
};

int question2(Ref head, Ref tail, Ref h, Ref t)
{
    if (head == NULL || h == NULL)
        return 0;

    int count = 0;
    Ref curr = head;

    while (curr != NULL)
    {
        if (curr == h)
        {
            Ref temp = h;
            while (curr != NULL && temp != NULL && curr == temp)
            {
                curr = curr->next;
                temp = temp->next;
            }

            if(temp == NULL)
                count++;
        }

        else curr = curr->next;
    }

    return count;
}

int main()
{
    cout << 1;
    return 0;
}