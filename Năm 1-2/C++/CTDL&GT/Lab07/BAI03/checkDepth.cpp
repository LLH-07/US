#include <iostream>
#include <cmath>

using namespace std;

struct Node
{
    int data;
    Node* left;
    Node* right;
    int balance;
};

Node* createNode(int data)
{
    Node* newNode = new Node;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->balance = 0;
    return newNode;
}

int getHeight(Node* root)
{
    if(root == NULL)
        return 0;

    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int getBalance(Node* root)
{
    if(root == NULL)
        return 0;

    return getHeight(root->right) - getHeight(root->left);
}

void singleRightRotate(Node *&root)
{
    Node *temp = root->left; // p1
    root->left = temp->right;
    temp->right = root;

    // Update balance
    root->balance = getBalance(root);
    temp->balance = getBalance(temp);
    root = temp;
}

void singleLeftRotate(Node *&root)
{
    Node *temp = root->right; // p1
    root->right = temp->left;
    temp->left = root;

    // Update balance
    root->balance = getBalance(root);
    temp->balance = getBalance(temp);

    root = temp;
}

void doubleRightRotate(Node *&root)
{
    singleLeftRotate(root->left);
    singleRightRotate(root);
}

void doubleLeftRotate(Node *&root)
{
    singleRightRotate(root->right);
    singleLeftRotate(root);
}

void insert(Node*& root, int data)
{
    if(root == NULL)
    {
        root = createNode(data);
        return;
    }

    if(data < root->data)
        insert(root->left, data);

    else if(data > root->data)
        insert(root->right, data);

    root->balance = getBalance(root);

    if(root->balance < -1)
    {
        if(data < root->left->data)
            singleRightRotate(root);

        else 
            doubleRightRotate(root);
    }

    else if(root->balance > 1)
    {
        if(data > root->right->data)
            singleLeftRotate(root);

        else
            doubleLeftRotate(root);
    }
}



