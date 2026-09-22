#include <iostream>
#include <cmath>
#include <fstream>
#include <time.h>
#include <iomanip>

using namespace std;

int numNode = 0;

// Create data structure for AVL tree
struct AVLnode
{
    string data;
    AVLnode *left;
    AVLnode *right;
    int Bal; // (-1, 0, 1) // -1: if(the left sub-tree is higher than than right sub-tree one and vice versa for -1, 0: if tree is balance)
};

// Initialize
AVLnode *createNode(string data)
{
    AVLnode *newTree = new AVLnode;
    newTree->left = NULL;
    newTree->right = NULL;
    newTree->data = data;
    newTree->Bal = 0;

    return newTree;
}

// Calculate tree height
int getHeight(AVLnode *root)
{
    if (root == NULL)
        return 0;

    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int getBalance(AVLnode *root)
{
    if (root == NULL)
        return 0;

    return getHeight(root->right) - getHeight(root->left);
}

void singleRightRotate(AVLnode *&root)
{
    AVLnode *temp = root->left; // p1
    root->left = temp->right;
    temp->right = root;

    // Update balance
    root->Bal = getBalance(root);
    temp->Bal = getBalance(temp);
    root = temp;
}

void singleLeftRotate(AVLnode *&root)
{
    AVLnode *temp = root->right; // p1
    root->right = temp->left;
    temp->left = root;

    // Update balance
    root->Bal = getBalance(root);
    temp->Bal = getBalance(temp);

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
void insert(AVLnode *&root, string data)
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

int search(AVLnode* root, string data)
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

int totalDepth(AVLnode *root, int curDepth)
{
    if(root == NULL)
        return 0;

    int leftDepth = totalDepth(root->left, curDepth + 1);
    int rightDepth = totalDepth(root->right, curDepth + 1);
    int sum = leftDepth + rightDepth + curDepth;

    return sum;
}

double averageDepth(AVLnode* root)
{
    int curDepth = 0;
    return totalDepth(root, curDepth) / numNode;
}

int longestDepth(AVLnode* root)
{
    return getHeight(root) - 1;
}


void readFile(AVLnode*& root)
{
    ifstream in("50k.txt");
    
    if(!in.is_open())
    {
        cout << "File could not be opened!\n";
        return;
    }

    string data;
    for(int i = 0; i < 50000; i++)
    {
        in >> data;
        insert(root, data);
        numNode++;
    }

    in.close();
}

int main()
{
    AVLnode* root = NULL;
    readFile(root);

    string foundName = "Yuji";
    string unfound = "Phoon";
    int b;

    clock_t start, end;
    double usedTime1, usedTime2;

    start = clock();
    b = search(root, foundName);
    end = clock();
    usedTime1 = double(end - start) / CLOCKS_PER_SEC;

    start = clock();
    b = search(root, unfound);
    end = clock();
    usedTime2 = double(end - start) / CLOCKS_PER_SEC;

    cout << fixed << setprecision(3);
    cout << "Running time(ms) when search existed name: " << usedTime1 * 1000.0 << endl;
    cout << "Running time(ms) when search name that don't exist: " << usedTime2 * 1000.0 << endl;

    cout << endl;
    cout << fixed << setprecision(0) << "Average depth nodes: " << averageDepth(root) << endl;
    cout << "The longest depth nodes: " << longestDepth(root) << endl;

    return 0;
}