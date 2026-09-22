#include <iostream>
#include <cmath>

using namespace std;

// Create data structure for AVL tree
struct AVLnode
{
    int data;
    AVLnode *left;
    AVLnode *right;
    int Bal; // (-1, 0, 1)
};

// Initialize
AVLnode *createNode(int data)
{
    AVLnode *newTree = new AVLnode;
    newTree->left = NULL;
    newTree->right = NULL;
    newTree->data = data;
    newTree->Bal = 0;

    return newTree;
}

int getHeight(AVLnode *root)
{
    if (root == NULL)
        return 0;

    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int getBalance(AVLnode* root)
{
    if(root == NULL)
        return 0;

    return getHeight(root->right) - getHeight(root->left);
}

void singleRightRotate(AVLnode *&root)
{
    AVLnode *temp = root->left; // p1
    root->left = temp->right;
    temp->right = root;

    // Update balance
    root->Bal = getHeight(root->right) - getHeight(root->left);
    temp->Bal = getHeight(root->right) - getHeight(root->left);

    root = temp;
}

void singleLeftRotate(AVLnode *&root)
{
    AVLnode *temp = root->right; // p1
    root->right = temp->left;
    temp->left = root;

    // Update balance
    root->Bal = getHeight(root->right) - getHeight(root->left);
    temp->Bal = getHeight(root->right) - getHeight(root->left);

    root = temp;
}

void doubleRightRotate(AVLnode *&root)
{
    singleLeftRotate(root->left);
    singleRightRotate(root);
}

void doubleLeftRotate(AVLnode *&root)
{
    singleRightRotate(root->right);
    singleLeftRotate(root);
}

// Insert
void insert(AVLnode *&root, int data)
{
    AVLnode *newNode = createNode(data);
    if (root == NULL)
    {
        root = newNode;
        return;
    }

    if (data < root->data)
    {
        insert(root->left, data);
        root->Bal = getBalance(root);

        // After inserting, if the height between left and right, then rotate
        if (root->Bal < -1)
        {
            if (data < root->left->data)
                singleRightRotate(root);

            else if (data > root->left->data)
                doubleRightRotate(root);
        }
    }

    else if (data > root->data)
    {
        insert(root->right, data);
        root->Bal = getBalance(root);

        if (root->Bal > 1)
        {
            if (data > root->right->data)
                singleLeftRotate(root);

            else if (data < root->right->data)
                doubleLeftRotate(root);
        }
    }
}

// Print tree
void preOrder(AVLnode *root)
{
    if (root == NULL)
        return;

    cout << root->data << ' ';

    preOrder(root->left);
    preOrder(root->right);
}

void inOrder(AVLnode *root)
{
    if (root == NULL)
        return;

    inOrder(root->left);

    cout << root->data;

    inOrder(root->right);
}

void postOrder(AVLnode *root)
{
    if (root == NULL)
        return;

    postOrder(root->left);
    postOrder(root->right);

    cout << root->data;
}

// Delete
AVLnode *findMin(AVLnode *root)
{
    if (root == NULL)
        return NULL;

    AVLnode *temp = root;
    while (temp->left != NULL)
        temp = temp->left;

    return temp;
}

AVLnode *deleteNode(AVLnode *root, int data)
{
    if (root == NULL)
        return NULL;

    if (data < root->data)
        root->left = deleteNode(root->left, data);

    else if (data > root->data)
        root->right = deleteNode(root->right, data);

    else
    {
        // No children
        if (root->left == NULL && root->right == NULL)
        {
            delete root;
            return NULL;
        }

        // One child
        else if (root->left == NULL)
        {
            AVLnode *temp = root->right;
            delete root;
            return temp;
        }

        else if (root->right == NULL)
        {
            AVLnode *temp = root->left;
            delete root;
            return temp;
        }

        AVLnode* temp = findMin(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, root->data);
    }

    root->Bal = getBalance(root);

    if(root->Bal < -1)
    {
        if(getBalance(root->left) < 0)
            singleRightRotate(root);

        else
            doubleRightRotate(root);
    }

    else if(root->Bal > 1)
    {
        if(getBalance(root->right) < 0)
            doubleLeftRotate(root);

        else 
            singleLeftRotate(root);
    }

    return root;
}

AVLnode *findUnbalanceTree(AVLnode *&root);