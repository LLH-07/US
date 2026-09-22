#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct Node
{
    int data;
    Node *left;
    Node *right;
    int balance;
};

Node *createNode(int data)
{
    Node *newNode = new Node;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->balance = 0;
    return newNode;
}

int getHeight(Node *root)
{
    if (root == NULL)
        return 0;

    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int getBalance(Node *root)
{
    if (root == NULL)
        return 0;

    return getHeight(root->right) - getHeight(root->left);
}

bool checkBalanceTree(Node *root)
{
    if (root == NULL)
        return true;

    int bal = getBalance(root);

    if (bal < -1 || bal > 1)
        return false;

    // true if right & left sub-tree are balance
    return checkBalanceTree(root->left) && checkBalanceTree(root->right);
}

void insert(Node *&root, int data)
{
    if (root == NULL)
    {
        root = createNode(data);;
        return;
    }

    if (data < root->data)
        insert(root->left, data);

    else if (data > root->data)
        insert(root->right, data);

    root->balance = getBalance(root);
}

void read_write_File()
{
    ifstream in("input.txt");
    ofstream out("Output.txt");
    if (!in.is_open() || !out.is_open())
    {
        cout << "File could not be opened!";
        exit(0);
    }

    int n;

    in >> n;
    in.ignore();

    for(int i = 0; i < n; i++)
    {
        Node *root = NULL;
        string s;
        
        getline(in, s);
        
        stringstream ss(s);
        int a;
        
        while (ss >> a)
            insert(root, a);

        if(checkBalanceTree(root))
            out << "YES\n";

        else out << "NO\n";
    }



    in.close();
    out.close();
}

int main()
{
    read_write_File();
}