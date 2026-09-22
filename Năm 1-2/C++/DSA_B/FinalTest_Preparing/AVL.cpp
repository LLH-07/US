#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Player
{
    string name = "";
    string dob = "";
    short height = 0;
    short weight = 0;
    string nation = "";
    string club = "";
};

void readFile(string filename, Player p[], int &n)
{
    ifstream in(filename);
    if (!in.is_open())
    {
        cout << "FILE ERROR!\n";
        return;
    }

    string ignoreLine;
    char ignoreChar;
    in >> ignoreLine;
    n = 0;

    while (!in.eof())
    {
        getline(in, p[n].name, ',');

        getline(in, p[n].dob, ',');

        in >> p[n].height;
        in >> ignoreChar;

        in >> p[n].weight;
        in >> ignoreChar;

        getline(in, p[n].nation, ',');
        getline(in, p[n].club);
        n++;
    }
    in.close();
}

struct Node
{
    string data;
    Node *left;
    Node *right;
    int height;
};

Node *createNode(string data)
{
    Node *newNode = new Node;
    newNode->left = newNode->right = NULL;
    newNode->data = data;
    newNode->height = 0;
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

void singleRightRotate(Node *&root)
{
    Node *temp = root->left;
    root->left = temp->right;
    temp->right = root;

    root->height = getHeight(root);
    temp->height = getHeight(temp);

    root = temp;
}

void singleLeftRotate(Node *&root)
{
    Node *temp = root->right;
    root->right = temp->left;
    temp->left = root;

    root->height = getHeight(root);
    temp->height = getHeight(temp);

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

void insert(Node *&root, string data)
{
    Node *newNode = createNode(data);
    if (root == NULL)
    {
        root = newNode;
        return;
    }

    if (data < root->data)
        insert(root->left, data);

    else if (data > root->data)
        insert(root->right, data);

    int bal = getBalance(root);
    if (bal < -1) // Left heavy
    {
        if (data < root->left->data)
            singleRightRotate(root);

        else if (data > root->left->data)
            doubleRightRotate(root);
    }

    else if (bal > 1)
    {
        if (data > root->right->data)
            singleLeftRotate(root);

        else if (data < root->right->data)
            doubleLeftRotate(root);
    }

    root->height = getHeight(root);
}

Node *findMax(Node *root)
{
    if (root == NULL || root->right == NULL)
        return root;

    return root->right;
}

void Remove(Node *&root, string data)
{
    if (root == NULL)
        return;

    if (data < root->data)
        Remove(root->left, data);

    else if (data > root->data)
        Remove(root->right, data);

    else
    {
        // Have no children
        if (root->left == NULL && root->right == NULL)
            root = NULL;

        else if (root->left == NULL)
            root = root->right;

        else if (root->right == NULL)
            root = root->left;

        else
        {
            Node *temp = findMax(root->left);
            root->data = temp->data;
            Remove(root->left, root->data);
        }
    }

    // After delete
    if (root == NULL)
        return;

    int bal = getBalance(root);

    if (bal < -1)
    {
        if (getBalance(root->left) <= 0)
            singleRightRotate(root);

        else
            doubleRightRotate(root);
    }

    else if (bal > 1)
    {
        if (getBalance(root->right) >= 0)
            singleLeftRotate(root);

        else
        {
            doubleLeftRotate(root);
        }
    }

    root->height = getHeight(root);
}

bool isAVL(Node *root)
{
    if (root == NULL)
        return true;

    int bal = getBalance(root);
    if (bal < -1 || bal > 1)
        return false;

    return isAVL(root->left) && isAVL(root->right);
}

void removeTwoNodeWithHeight(Node *root, int height)
{
    if (root == NULL)
        return;

    removeTwoNodeWithHeight(root->left, height);
    removeTwoNodeWithHeight(root->right, height);

    if (root->height == height)
        Remove(root, root->data);

    if (root->height == height)
        Remove(root, root->data);
}

void preOrder(Node *root)
{
    if (root == NULL)
        return;

    cout << root->data << ' ';
    preOrder(root->left);
    preOrder(root->right);
}

void removeAtHeight(Node *&root, int height, int currHeight = 0)
{
    if (root == NULL)
        return;

    if (currHeight < height)
    {
        removeAtHeight(root->left, height, currHeight + 1);
        removeAtHeight(root->right, height, currHeight + 1);
    }

    else if (currHeight == height)
        Remove(root, root->data);
}
/*
// Search
AVLnode *search(AVLnode *root, int data)
{
    if (root == NULL)
        return NULL;

    if (data == root->data)
        return root;

    else if (data < root->data)
        return search(root->left, data);

    else if (data > root->data)
        return search(root->right, data);

    return NULL;
}

// 7.3
bool areSameLeafDepth(AVLnode *root, int curDepth, int &Depth)
{
    if (root == NULL)
        return true;

    if (root->left == NULL && root->right == NULL)
    {
        if (Depth == 0)
            Depth = curDepth;

        return Depth == curDepth;
    }

    return areSameLeafDepth(root->left, curDepth + 1, Depth) && areSameLeafDepth(root->right, curDepth + 1, Depth);
}

// 7.4
AVLnode *findNearestCommonAncestor(AVLnode *child1, AVLnode *child2, AVLnode *root)
{
    if (root == NULL || child1 == NULL || child2 == NULL)
        return NULL;

    if (child1->data < root->data && child2->data < root->data)
        return findNearestCommonAncestor(child1, child2, root->left);

    if (child1->data > root->data && child2->data > root->data)
        return findNearestCommonAncestor(child1, child2, root->right);

    return root;
}


*/

int main()
{
    Node *root = NULL;
    // insert(root, "1");
    // insert(root, "2");
    // insert(root, "3");
    // insert(root, "4");
    // insert(root, "5");
    // insert(root, "6");
    // insert(root, "7");
    // insert(root, "8");
    // insert(root, "9");
    // insert(root, "10");
    // insert(root, "11");
    // insert(root, "12");
    // insert(root, "13");
    // insert(root, 1);
    // insert(root, 2);
    // insert(root, 3);
    // insert(root, 4);
    // insert(root, 5);
    // insert(root, 6);
    // insert(root, 7);
    // insert(root, 8);
    // insert(root, 9);
    // insert(root, 10);
    // insert(root, 11);
    // insert(root, 12);
    // insert(root, 13);
    preOrder(root);
    cout << endl;
    removeTwoNodeWithHeight(root, 2);
    //removeAtHeight(root, 2);
    // Remove(root, 6);
    preOrder(root);
    cout << endl
         << getBalance(root) << endl;
}