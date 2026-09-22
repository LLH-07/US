#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

struct Node
{
    char Eng[50];
    char Vn[50];
    Node *left;
    Node *right;
};

Node *createNode(char eng[], char vn[])
{
    Node *newNode = new Node;

    strcpy(newNode->Eng, eng);
    strcpy(newNode->Vn, vn);
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void insertNode(Node *&root, char eng[], char vn[])
{
    Node *newNode = createNode(eng, vn);
    if (root == NULL)
    {
        root = newNode;
        return;
    }

    int compareResult = strcmp(eng, root->Eng);

    if (compareResult < 0)
        insertNode(root->left, eng, vn);
    else if (compareResult > 0)
        insertNode(root->right, eng, vn);
}

void readFile(Node *&root)
{
    ifstream in("dictionary.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    char eng[50], vn[50];
    char ignore;
    while (!in.eof())
    {
        // example
        // abate : làm dịu đi
        in >> eng;
        in >> ignore;
        in.getline(vn, sizeof(vn));

        insertNode(root, eng, vn);
    }

    in.close();
}

void lowerWord(char word[])
{
    for (int i = 0; word[i] != '\0'; i++)
        word[i] = tolower(word[i]);
}

Node *search(Node *root, char eng[])
{
    if (root == NULL)
        return NULL;

    int compareResult = strcmp(eng, root->Eng);

    if (compareResult < 0)
        return search(root->left, eng);

    else if (compareResult > 0)
        return search(root->right, eng);

    return root;
}

void function(Node *root)
{
    system("cls");
    char eng[50];
    cout << "English word: ";
    cin >> eng;
    lowerWord(eng);

    Node* temp = search(root, eng);
    if (temp == NULL)
    {
        cout << "Word don't exist in dictionary!";
        return;
    }
    else
        cout << "==> Vietnamese: " << temp->Vn << endl << endl;

    char cont;
    cout << "Continue?(y/n): ";
    cin >> cont;
    if(tolower(cont) == 'y')
        function(root);

    else
    {
        cout << "The program stopped!";
        exit(0);
    }
}

int main()
{
    Node *root = NULL;
    readFile(root);
    function(root);
}