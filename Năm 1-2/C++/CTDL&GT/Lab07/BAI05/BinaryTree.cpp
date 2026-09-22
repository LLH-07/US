#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <cmath>

using namespace std;

int numNode = 0;
struct Node
{
    string data;
    Node *left;
    Node *right;
};

Node *createNode(string newData)
{
    Node *newNode = new Node;
    newNode->data = newData;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void insertNode(Node *&root, string newData)
{
    if (root == NULL)
    {
        root = createNode(newData);
        return;
    }

    if (newData < root->data)
        insertNode(root->left, newData);

    else if (newData > root->data)
        insertNode(root->right, newData);
}

int search(Node *root, string data)
{
    if (root == NULL)
        return 0;

    if (data == root->data)
        return 1;

    if (data < root->data)
        return search(root->left, data);

    else if (data > root->data)
        return search(root->right, data);
}

int totalDepth(Node *root, int curDepth)
{
    if(root == NULL)
        return 0;

    int leftDepth = totalDepth(root->left, curDepth + 1);
    int rightDepth = totalDepth(root->right, curDepth + 1);
    int sum = leftDepth + rightDepth + curDepth;

    return sum;
}

double averageDepth(Node* root)
{
    int curDepth = 0;
    return totalDepth(root, curDepth) / numNode;
}

int longestDepthNode(Node* root)
{
    if(root == NULL)
        return -1;

    return max(longestDepthNode(root->left), longestDepthNode(root->right)) + 1;
}

void readFile(Node *&root)
{
    ifstream in("50k.txt");

    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    string data;
    while (!in.eof())
    {
        in >> data;
        insertNode(root, data);
        numNode++;
    }

    in.close();
}


int main()
{
    Node *root = NULL;
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
    cout << "The longest depth nodes: " << longestDepthNode(root) << endl;

    return 0;
}