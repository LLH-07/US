#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <time.h>
#include <iomanip>

using namespace std;

int numNode = 0;

struct avlNode
{
    string data;
    avlNode* left;
    avlNode* right;
    int bal;
};

avlNode* createNode(string newData)
{
    avlNode* newNode = new avlNode;
    newNode->data = newData;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->bal = 0;
    return newNode;
}

int getHeight(avlNode* root)
{
    if(root == NULL)
        return 0;

    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int getBalance(avlNode* root)
{
    if(root == NULL)
        return 0;

    return getHeight(root->right) - getHeight(root->left);
}

void singleRightRotate(avlNode*& root)
{
    avlNode* temp = root->left;
    root->left = temp->right;
    temp->right = root;

    temp->bal = getBalance(temp);
    root->bal = getBalance(root);

    root = temp;
}

void singleLeftRotate(avlNode*& root)
{
    avlNode* temp = root->right;
    root->right = temp->left;
    temp->left = root;

    temp->bal = getBalance(temp);
    root->bal = getBalance(root);
}

void doubleRightRotate(avlNode*& root)
{
    singleLeftRotate(root->left);
    singleRightRotate(root);
}

void doubleLeftRotate(avlNode*& root)
{
    singleRightRotate(root->right);
    singleLeftRotate(root);
}

void insert(avlNode*& root, string data)
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

    root->bal = getBalance(root);

    if(root->bal < -1)
    {
        if(data < root->left->data)
            singleRightRotate(root);

        else if(data > root->left->data)
            doubleRightRotate(root);
    }

    else if(root->bal > 1)
    {
        if(data < root->right->data)
            doubleLeftRotate(root);

        else if(data > root->right->data)
            singleLeftRotate(root);
    }
}

int search(avlNode* root, string data)
{
    if(root == NULL)
        return 0;

    if(root->data == data)
        return 1;

    if(data < root->data)
        return search(root->left, data);

    else if(data > root->data)
        return search(root->right, data);
}

int totalDepth(avlNode *root, int curDepth)
{
    if(root == NULL)
        return 0;

    int leftDepth = totalDepth(root->left, curDepth + 1);
    int rightDepth = totalDepth(root->right, curDepth + 1);
    int sum = leftDepth + rightDepth + curDepth;

    return sum;
}

double averageDepth(avlNode* root)
{
    int curDepth = 0;
    return totalDepth(root, curDepth) / numNode;
}

int longestDepth(avlNode* root)
{
    return getHeight(root) - 1;
}

void readFile(avlNode*& root)
{
    ifstream in("50k.txt");
    
    if(!in.is_open())
    {
        cout << "File could not be opened!\n";
        return;
    }

    string data;
    while(!in.eof())
    {
        in >> data;
        insert(root, data);
        numNode++;
    }

    in.close();
}

void preOrder(avlNode *root)
{
    if (root == NULL)
        return;

    cout << root->data << endl;

    preOrder(root->left);
    preOrder(root->right);
}

int main()
{
    avlNode* root = NULL;

    readFile(root);

    preOrder(root);

    // string foundName = "Yuji";
    // string unfound = "Phoon";
    // int b;

    // clock_t start, end;
    // double usedTime1, usedTime2;

    // start = clock();
    // b = search(root, foundName);
    // end = clock();
    // usedTime1 = double(end - start) / CLOCKS_PER_SEC;

    // start = clock();
    // b = search(root, unfound);
    // end = clock();
    // usedTime2 = double(end - start) / CLOCKS_PER_SEC;

    // cout << fixed << setprecision(3);
    // cout << "Running time(ms) when search existed name: " << usedTime1 * 1000.0 << endl;
    // cout << "Running time(ms) when search name that don't exist: " << usedTime2 * 1000.0 << endl;

    // cout << endl;
    // cout << fixed << setprecision(0) << "Average depth nodes: " << averageDepth(root) << endl;
    // cout << "The longest depth nodes: " << longestDepth(root) << endl;

    return 0;
}