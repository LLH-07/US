#include <iostream>

using namespace std;

struct Node
{
    int data;
    Node *left;
    Node *right;
};

Node *createNode(int newData)
{
    Node *newNode = new Node;

    newNode->data = newData;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void insertNode(Node *&root, int newData)
{
    Node *newNode = createNode(newData);
    if (root == NULL)
    {
        root = newNode;
        return;
    }

    if (newData < root->data)
        insertNode(root->left, newData);

    else if (newData > root->data)
        insertNode(root->right, newData);
}

// a
void LNR(Node *root)
{
    if (root == NULL)
        return;

    LNR(root->left);
    cout << root->data << ' ';
    LNR(root->right);
}

void LRN(Node *root)
{
    if (root == NULL)
        return;

    LRN(root->left);
    LRN(root->right);
    cout << root->data << ' ';
}

void NLR(Node *root)
{
    if (root == NULL)
        return;

    cout << root->data << ' ';
    NLR(root->left);
    NLR(root->right);
}

void NRL(Node *root)
{
    if (root == NULL)
        return;

    cout << root->data;
    NRL(root->right);
    NRL(root->left);
}

void RNL(Node *root)
{
    if (root == NULL)
        return;

    RNL(root->right);
    cout << root->data << ' ';
    RNL(root->left);
}

void RLN(Node *root)
{
    if (root == NULL)
        return;

    RLN(root->right);
    RLN(root->left);
    cout << root->data << ' ';
}

// b
bool search(Node *root, int data)
{
    if (root == NULL)
        return false;

    if (data < root->data)
        return search(root->left, data);

    else if (data > root->data)
        return search(root->right, data);

    return true;
}

// c
Node *findMinOnRightTree(Node *root)
{
    if (root == NULL)
        return root;

    if (root->left == NULL)
        root = root->left;

    return root;
}

Node *deleteNode(Node *root, int data)
{
    if (root == NULL)
        return root;

    // No children
    if (data < root->data)
        root->left = deleteNode(root->left, data);

    else if (data > root->data)
        root->right = deleteNode(root->right, data);

    else
    {
        // Have no children
        if (root->left == NULL && root->right == NULL)
        {
            delete root;
            return NULL;
        }

        // Have one child
        if (root->left == NULL)
        {
            Node *temp = root->right;
            delete root;
            return temp;
        }

        else if (root->right == NULL)
        {
            Node *temp = root->left;
            delete root;
            return temp;
        }

        Node *temp = findMinOnRightTree(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data);
    }
}

Node *DELETE(Node *root, int data)
{
    if (root == NULL)
    {
        cout << "Tree is empty!\n";
        return root;
    }

    return deleteNode(root, data);
}

// d
int countNode(Node *root)
{
    if (root == NULL)
        return 0;

    return countNode(root->left) + countNode(root->right) + 1;
}

// e
void countFullChildrenSubTreeNode(Node *root, int& cnt)
{
    if (root == NULL)
        return;


    if (root->left != NULL && root->right != NULL)
        cnt++;

    countFullChildrenSubTreeNode(root->left, cnt);
    countFullChildrenSubTreeNode(root->right, cnt);
}

//  f
int countLeafNode(Node* root)
{
    if(root == NULL)
        return 0;

    if(root->left == NULL && root->right == NULL)
        return 1;
        
    return countLeafNode(root->left) + countLeafNode(root->right);
}


int main()
{
    Node *root = NULL;
    insertNode(root, 8);
    insertNode(root, 7);
    insertNode(root, 3);
    insertNode(root, 1);
    insertNode(root, 9);
    insertNode(root, 6);
    insertNode(root, 5);
    insertNode(root, 2);
    insertNode(root, 4);

    int num = 0;
    countFullChildrenSubTreeNode(root, num);
    cout << countNode(root) << endl
         << num << endl << countLeafNode(root) << endl;
}
