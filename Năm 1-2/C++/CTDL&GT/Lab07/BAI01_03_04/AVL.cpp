#include <iostream>
#include <cmath>

using namespace std;

// Create data structure for AVL tree
struct AVLnode
{
    int data;
    AVLnode *left;
    AVLnode *right;
    int Bal; // (-1, 0, 1) // -1: if(the left sub-tree is higher than than right sub-tree one and vice versa for -1, 0: if tree is balance)
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

        // After inserting, if the height between left and right is unbalance, then rotate
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

    cout << root->data << " ";

    inOrder(root->right);
}

void postOrder(AVLnode *root)
{
    if (root == NULL)
        return;

    postOrder(root->left);
    postOrder(root->right);

    cout << root->data << " ";
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

AVLnode *findMax(AVLnode *root)
{
    if (root == NULL)
        return NULL;

    AVLnode *temp = root;
    while (temp->right != NULL)
        temp = temp->right;

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

        AVLnode *temp = findMax(root->left);
        root->data = temp->data;
        root->left = deleteNode(root->left, root->data);
    }

    if (root == NULL)
        return NULL;

    root->Bal = getBalance(root);

    if (root->Bal < -1)
    {
        if (getBalance(root->left) <= 0)
            singleRightRotate(root);

        else
            doubleRightRotate(root);
    }

    else if (root->Bal > 1)
    {
        if (getBalance(root->right) < 0)
            doubleLeftRotate(root);

        else
            singleLeftRotate(root);
    }

    return root;
}

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

int main()
{
    AVLnode *tree = NULL;

    insert(tree, 5);
    insert(tree, 2);
    insert(tree, 1);
    insert(tree, 4);
    insert(tree, 3);
    insert(tree, 9);
    insert(tree, 7);
    insert(tree, 10);
    insert(tree, 8);
    insert(tree, 6);

    cout << "Before deleting: ";
    cout << "\nPre-order: ";
    preOrder(tree);
    cout << "\nIn-order: ";
    inOrder(tree);
    cout << "\nPost-order: ";
    postOrder(tree);

    tree = deleteNode(tree, 4);
    tree = deleteNode(tree, 3);
    tree = deleteNode(tree, 1);
    tree = deleteNode(tree, 5);
    tree = deleteNode(tree, 7);

    cout << endl
         << endl;

    cout << "\n\nAfter deleting: ";
    cout << "\nPre-order: ";
    preOrder(tree);
    cout << "\nIn-order: ";
    inOrder(tree);
    cout << "\nPost-order: ";
    postOrder(tree);

    cout << endl
         << endl;
    cout << "The height of the tree: " << getHeight(tree) << endl
         << endl;

    if (search(tree, 8) != NULL)
        cout << "8 is found!\n";

    else
        cout << "8 is not found!\n";

    if (search(tree, 4) != NULL)
        cout << "4 is found!\n";

    else
        cout << "4 is not found!\n\n";

    int Depth = 0;

    // 7.3
    cout << "All leaves node are same depth: ";
    if (areSameLeafDepth(tree, 0, Depth))
        cout << "YES\n";

    else
        cout << "NO\n";

    //
    insert(tree, 4);
    insert(tree, 3);
    insert(tree, 5);
    insert(tree, 1);
    insert(tree, 7);

    AVLnode* child1 = search(tree, 1);
    AVLnode* child2 = search(tree, 5);

    cout << "\nThe least common ancestor of " << child1->data << " and " << child2->data << ": " << findNearestCommonAncestor(child1, child2, tree)->data << endl;
}