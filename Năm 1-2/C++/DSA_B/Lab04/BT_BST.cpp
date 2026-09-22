#include <iostream>

using namespace std;

struct NODE
{
    int key;
    NODE *left;
    NODE *right;
};

// Bai 01
NODE *createNode(int data)
{
    NODE *newNode = new NODE;
    newNode->key = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Bai 02
void Insert(NODE *&pRoot, int x)
{
    if (pRoot == NULL)
    {
        pRoot = createNode(x);
        return;
    }

    if (x < pRoot->key)
        Insert(pRoot->left, x);

    else if (x > pRoot->key)
        Insert(pRoot->right, x);
}

// Bai 03
void NLR(NODE *pRoot)
{
    if (pRoot == NULL)
        return;

    cout << pRoot->key << " ";
    NLR(pRoot->left);
    NLR(pRoot->right);
}

// Bai 04
void LNR(NODE *pRoot)
{
    if (pRoot == NULL)
        return;

    LNR(pRoot->left);
    cout << pRoot->key << " ";
    LNR(pRoot->right);
}

// Bai 05
void LRN(NODE *pRoot)
{
    if (pRoot == NULL)
        return;

    LRN(pRoot->left);
    LRN(pRoot->right);
    cout << pRoot->key << " ";
}

// Bai 07
int Height(NODE *pRoot)
{
    if (pRoot == NULL)
        return 0;

    int leftHeight = Height(pRoot->left);
    int rightHeight = Height(pRoot->right);

    return max(leftHeight, rightHeight) + 1;
}

// Bai 06
void printCurrentLevel(NODE *pRoot, int level)
{
    if (pRoot == NULL)
        return;

    // Check if at the top of the subtree
    if (level == 1)
        cout << pRoot->key << ' ';

    else
    {
        printCurrentLevel(pRoot->left, level - 1);
        printCurrentLevel(pRoot->right, level - 1);
    }
}

void levelOrder(NODE *pRoot) // BFS
{
    int height = Height(pRoot);
    for (int level = 0; level < height; level++)
        printCurrentLevel(pRoot, level + 1);
}

// Bai 08
int countNode(NODE *pRoot)
{
    if (pRoot == NULL)
        return 0;

    int total = countNode(pRoot->left) + countNode(pRoot->right);
    return total + 1;
}

// Bai 09
int sumNode(NODE *pRoot)
{
    if (pRoot == NULL)
        return 0;

    return pRoot->key + sumNode(pRoot->left) + sumNode(pRoot->right);
}

// Bai 10
NODE *Search(NODE *pRoot, int x)
{
    if (pRoot == NULL || pRoot->key == x)
        return pRoot;

    else if (x < pRoot->key)
        return Search(pRoot->left, x);

    else if (x > pRoot->key)
        return Search(pRoot->right, x);
}

// Bai 11
NODE *findMax(NODE *pRoot)
{
    if (pRoot == NULL || pRoot->right == NULL)
        return pRoot;

    return findMax(pRoot->right);
}

void Remove(NODE *&pRoot, int x)
{
    if (pRoot == NULL)
        return;

    if (x < pRoot->key)
        Remove(pRoot->left, x);

    else if (x > pRoot->key)
        Remove(pRoot->right, x);

    else
    {
        // Have no child
        if (pRoot->left == NULL && pRoot->right == NULL)
            pRoot = NULL;

        // Have one child
        else if (pRoot->left != NULL)
        {
            NODE *tmp = pRoot->left;
            delete pRoot;
            pRoot = tmp;
        }

        else if (pRoot->right != NULL)
        {
            NODE *tmp = pRoot->right;
            delete pRoot;
            pRoot = tmp;
        }

        // Have 2 children
        else
        {
            NODE *tmp = findMax(pRoot->left);
            pRoot->key = tmp->key;
            Remove(pRoot->left, pRoot->key);
        }
    }
}

// Bai 12
NODE *createTree(int a[], int n)
{
    NODE *root = NULL;
    for (int i = 0; i < n; i++)
        Insert(root, a[i]);

    return root;
}

// Bai 13
void removeTree(NODE *&pRoot)
{
    if (pRoot == NULL)
        return;

    removeTree(pRoot->left);
    removeTree(pRoot->right);

    delete pRoot;
    pRoot = NULL;
}

// Bai 14
int heightNode(NODE *pRoot, int value)
{
    NODE *tmp = Search(pRoot, value);

    if (tmp == NULL)
        return -1;

    return Height(tmp);
}

// Bai 15
int Level(NODE *pRoot, NODE *p)
{
    NODE *tmp = Search(pRoot, p->key);
    if (tmp == NULL)
        return -1;

    return Height(pRoot) - Height(tmp) + 1;
}

// Bai 16
int countLeaf(NODE *pRoot)
{
    if(pRoot == NULL)
        return 0;

    if(pRoot->left == NULL && pRoot->right == NULL)
        return 1;
    
    return countLeaf(pRoot->left) + countLeaf(pRoot->right);
}

// Bai 17
int countLess(NODE* pRoot, int x)
{
    NODE* tmp = Search(pRoot, x);
    if(tmp == NULL)
        return 0;

    return countNode(tmp->left);
}

// Bai 18
int countGreater(NODE* pRoot, int x)
{
    NODE* tmp = Search(pRoot, x);
    if(tmp == NULL)
        return 0;

    return countNode(tmp->right);
}

// Bai 19
NODE* findMin(NODE* pRoot)
{
    if (pRoot == NULL || pRoot->left == NULL)
        return pRoot;

    return findMax(pRoot->left);
}
bool isBST(NODE* pRoot)
{
    if(pRoot == NULL)
        return true;

    NODE* minNode = findMin(pRoot->right);
    NODE* maxNode = findMax(pRoot->left);
    if(pRoot->left != NULL && maxNode->key > pRoot->key)
        return false;

    if(pRoot->right != NULL && minNode->key < pRoot->key)
        return false;

    return isBST(pRoot->left) && isBST(pRoot->right);   
}

// Bai 20
bool isFullBST(NODE* pRoot)
{
    if(pRoot == NULL)
        return true;

    if(pRoot->left == NULL && pRoot->right == NULL)
        return true;

    if(!isBST(pRoot)) return false;

    if(pRoot->left != NULL && pRoot->right != NULL)
        return isFullBST(pRoot->left) && isFullBST(pRoot->right);


}

int main()
{
    NODE *root = NULL;
    for(int i = 0; i < 10; i++)
        Insert(root, i);

    cout << countLess(root, 5) << endl << countGreater(root, 5) << endl;
}
