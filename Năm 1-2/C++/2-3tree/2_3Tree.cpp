#include <iostream>

using namespace std;

struct Node
{
    int data1 = 0;
    int data2 = 0;
    Node *left = NULL;
    Node *mid = NULL;
    Node *right = NULL;
};

Node *createNode(int data)
{
    Node *newNode = new Node;
    newNode->data1 = data;
    newNode->data2 = 0;
    newNode->left = newNode->right = newNode->mid = NULL;
    return newNode;
}

Node *createRoot(Node *&root, int data)
{
    if (root == NULL)
    {
        root = createNode(data);
        return;
    }
}

bool isLeaf(Node *node)
{
    return node->left == NULL && node->mid == NULL && node->right == NULL;
}

bool isFull(Node *node)
{
    return node->data1 != 0 && node->data2 != 0;
}

    void findMMM(int &Min, int &mid, int &Max)
{
    int tempMax = max(Min, max(mid, Max));
    int tempMin = min(Min, min(mid, Max));
    int tempMid;

    if (tempMin < Min && Min < tempMax)
        tempMid = Min;

    else if (tempMin < mid && mid < tempMax)
        tempMid = mid;

    else if (tempMin < Max && Max < tempMax)
        tempMid = Max;

    Min = tempMin;
    mid = tempMid;
    Max = tempMax;
}
void Split(Node *&root, int data)
{
    Node *pos;
    int min, mid, max;

    if (isFull(root))
    {
        if (data < root->data1)
        {
            pos = root->left;
            int min = pos->data1;
            int max = pos->data2;
            int mid = data;
            findMMM(min, mid, max);

            Node *temp = createNode(mid);
            Node *l = createNode(min);
            Node *r = createNode(max);

            temp->left = l;
            temp->mid = r;

            root->left = temp;
            
        }

        else if (data > root->data2)
            pos = root->right;

        else if (root->data1 < data && data < root->data2)
            pos = root->mid;
    }

    else if (root->data1 != 0)
    {
        if (data < root->data1)
            pos = root->left;

        else if (data > root->data1)
            pos = root->mid;
    }
}

void insert(Node *&root, int data, bool &split)
{
    if (root == NULL)
    {
        root = createNode(data);
        return;
    }

    if (isLeaf(root))
    {
        if (root->data1 == 0 && root->data2 == 0)
            root->data1 = data;

        else if (root->data1 != 0 && root->data2 == 0)
        {
            root->data2 = data;
            if (data < root->data1)
                swap(root->data1, root->data2);
        }

        else
        {
            split = true;
        }
        return;
    }

    if (root->data1 != 0 && root->data2 != 0)
    {
        if (data < root->data1)
            insert(root->left, data, split);

        else if (data > root->data2)
            insert(root->right, data, split);

        else if (root->data1 < data && data < root->data2)
            insert(root->mid, data, split);
    }

    else if (root->data1 != 0)
    {
        if (data < root->data1)
            insert(root->left, data, split);

        else if (data > root->data1)
            insert(root->mid, data, split);
    }

    else if (root->data2 != 0)
    {
        if (data < root->data2)
            insert(root->mid, data, split);

        else if (data > root->data2)
            insert(root->right, data, split);
    }

    if (split)
    {
        Split(root, data);
        split = false;
        if(isFull(root)) split = true;
    }
}
