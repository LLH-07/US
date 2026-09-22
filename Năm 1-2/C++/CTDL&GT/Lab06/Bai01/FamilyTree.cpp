#include <iostream>
#include <fstream>
#include <string.h>
#include <string>

using namespace std;

struct Node
{
    string data;
    Node **children;
    int numChildren;
};

Node *createChildNode(string data)
{
    Node *newNode = new Node;
    newNode->data = data;
    newNode->children = new Node *;
    newNode->numChildren = 0;
    return newNode;
}

Node *search(Node *parent, string childName)
{
    if (parent == NULL)
        return NULL;

    if (parent->data == childName)
        return parent;

    for (int i = 0; i < parent->numChildren; i++)
    {
        Node* temp = search(parent->children[i], childName);
        if(temp != NULL)
            return temp;
    }

    return NULL;
}

void add(Node *&parent, string data)
{
    Node *newChild = createChildNode(data);
    if (parent == NULL)
    {
        parent = newChild;
        return;
    }

    parent->children[parent->numChildren] = newChild;
    parent->numChildren++;
}

void PreOder(Node *parent)
{
    if (parent == NULL)
        return;

    cout << parent->data << " | ";

    for (int i = 0; i < parent->numChildren; i++)
        PreOder(parent->children[i]);
}

void InOrder(Node *parent)
{
    if (parent == NULL)
        return;

    if (parent->numChildren > 0)
        InOrder(parent->children[0]);

    cout << parent->data << " | ";

    for (int i = 1; i < parent->numChildren; i++)
        InOrder(parent->children[i]);
}

void PostOrder(Node *parent)
{
    if (parent == NULL)
        return;

    for (int i = 0; i < parent->numChildren; i++)
        PostOrder(parent->children[i]);

    cout << parent->data << " | ";
}

void readFile(Node *&parent)
{
    ifstream fin("input.txt");
    if (!fin.is_open())
    {
        cout << "File could not be opened!";
        exit(0);
    }

    int num;
    fin >> num;

    for(int i = 0; i < num; i++)
    {
        string parentName = "";
        string childName = "";
        Node* pos;
        char ign;

        fin >> ign;
        fin >> parentName;

        pos = search(parent, parentName);
        if(pos == NULL)
        {
            add(parent, parentName);
            pos = parent;
        }

        fin >> ign;
        getline(fin, childName, ' ');
        add(pos, childName);

        getline(fin, childName, ')');
        add(pos, childName);
        fin >> ign;

    }

    fin.close();
}

int main()
{
    Node *parent = NULL;
    readFile(parent);

    cout << "PreOder: \n";
    PreOder(parent);

    cout << "\n\nInOrder: \n";
    InOrder(parent);

    cout << "\n\nPostOrder: \n";
    PostOrder(parent);
}