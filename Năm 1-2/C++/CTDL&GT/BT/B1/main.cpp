#include <iostream>
#include <cmath>

using namespace std;


#define MAX_NODE 100
int BST[MAX_NODE];

void initialBST()
{
    for(int i = 0; i < MAX_NODE; i++)
        BST[i] = -1;
}

bool isEmpty()
{
    return BST[0] == -1;
}

int getHeight(int root = 0)
{
    if(BST[root] == -1)
        return 0;

    return max(getHeight(root * 2 + 1), getHeight(root * 2 + 1)) + 1;
}

int getNumNode(int root = 0)
{
    if(BST[root] == -1)
        return 0;

    return 1 + getNumNode(root * 2 + 1) + getNumNode(root * 2 + 2);
}

int getDataRootNode()
{
    return BST[0];
}

void insertData(int data, int root  = 0)
{
    if(BST[root] == -1)
    {
        BST[root] = data;
        return;
    }

    if(data < BST[root])
        insertData(data, root * 2 + 1);

    else if(data > BST[root])
        insertData(data, root * 2 + 2);
}

int findMax(int root)
{
    while(BST[root * 2 + 2] != -1)
        root = root * 2 + 1;

    return root;
}

void deleteData(int data, int root = 0)
{
    if(BST[root] == -1)
        return;

    if(data < BST[root])
        deleteData(data, root * 2 + 1);

    else if(data > BST[root])
        deleteData(data, root * 2 + 2);

    else
    {
        // Have no child
        if(BST[root * 2 + 1] == -1 && BST[root * 2 + 2] == -1)
            BST[root] = -1;

        // Have one child
        else if(BST[root * 2 + 1] == -1)
        {
            BST[root] = BST[root * 2 + 2];
            deleteData(BST[root * 2 + 2], root * 2 + 2);
        }

        else if(BST[root * 2 + 2] == -1)
        {
            BST[root] = BST[root * 2 + 1];
            deleteData(BST[root * 2 + 1], root * 2 + 1);
        }

        // Have 2 children
        else
        {
            int posMax = findMax(root * 2 + 1);
            BST[root] = BST[posMax];
            deleteData(BST[root], posMax);
        }

    }


}

void deleteTree()
{
    initialBST();
}

int retrieve(int data, int root = 0)
{
    if(BST[root] == -1)
        return -1;

    if(BST[root] == data)
        return root;

    else if(data < BST[root])
        return retrieve(data, root * 2 + 1);

    else
        return retrieve(data, root * 2 + 2);
}

bool contain(int data)
{
    return retrieve(data) != -1;
}

void preOrder(int root = 0)
{
    if(BST[root] == -1)
        return;

    cout << BST[root] << ' ';
    preOrder(root * 2 + 1);
    preOrder(root * 2 + 2);
}

void inOrder(int root = 0)
{
    if(BST[root] == -1)
        return;

    inOrder(root * 2 + 1);
    cout << BST[root] << ' ';
    inOrder(root * 2 + 2);
}

void postOrder(int root = 0)
{
    if(BST[root] == -1)
        return;

    postOrder(root * 2 + 1);
    postOrder(root * 2 + 2);
    cout << BST[root] << ' ';
}

int main()
{
    return 0;
}