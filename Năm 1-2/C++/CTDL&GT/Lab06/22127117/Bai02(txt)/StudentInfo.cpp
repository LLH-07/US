#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Binary tree
struct info
{
    string ID = "";
    string name = "";
    string birth = "";
    string status = "";
};

struct Node
{
    info data;
    Node *left;
    Node *right;
};

Node *createNode(info data)
{
    Node *newNode = new Node;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void Insert(Node *&root, info data)
{

    if (root == NULL)
    {
        root = createNode(data);
        return;
    }

    if (data.name < root->data.name)
        Insert(root->left, data);

    else if (data.name > root->data.name)
        Insert(root->right, data);

    return;
}

Node *Search(Node *root, string id)
{
    if (root == NULL || root->data.ID == id)
        return root;

    if (Search(root->left, id) != NULL)
        return Search(root->left, id);

    return Search(root->right, id);
}

Node *findMaxOnLeftTree(Node *root)
{
    Node *temp = root;
    while (temp->right != NULL)
        temp = temp->right;

    return temp;
}

Node *deleteMaxNodeOnLeftTree(Node *maxNode)
{
    if (maxNode == NULL)
        return maxNode;

    if (maxNode->right == NULL)
    {
        Node *temp = maxNode->left;
        delete maxNode;
        return temp;
    }

    maxNode->right = deleteMaxNodeOnLeftTree(maxNode->right);
    return maxNode;
}
// root is grandparent of the deleted node
Node *Delete(Node *root, string name)
{
    if (root == NULL)
        return NULL;

    if (name < root->data.name)
        root->left = Delete(root->left, name);

    else if (name > root->data.name)
        root->right = Delete(root->right, name);

    else
    {
        // Have no children
        if (root->left == NULL && root->right == NULL)
        {
            delete root;
            return NULL;
        }

        // Have one child
        else if (root->left == NULL)
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

        // Have 2 children
        Node *temp = findMaxOnLeftTree(root->left);
        root->data = temp->data;
        root->left = deleteMaxNodeOnLeftTree(root->left);
    }

    return root;
}

// File
Node *readFile(int &n)
{
    ifstream fin("student.data.txt");
    if (!fin.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    if (!fin.eof())
        fin >> n;
    fin.ignore();
    Node *root = NULL;

    while (!fin.eof())
    {
        info data;

        fin >> data.ID;
        fin >> data.name;
        fin >> data.birth;
        fin >> data.status;
        fin.ignore();
        // fin.ignore();

        Insert(root, data);
    }

    fin.close();

    return root;
}

void InOrder(Node *root, ofstream &out)
{
    if (root == NULL)
        return;

    InOrder(root->left, out);

    if (root->data.ID != "")
        out << root->data.ID << endl;

    if (root->data.name != "")
        out << root->data.name << endl;

    if (root->data.birth != "")
        out << root->data.birth << endl;

    if (root->data.status != "")
        out << root->data.status << endl;
    InOrder(root->right, out);
}

void writeFile(int n, Node *root)
{
    ofstream out("student.data.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    out << n << endl;
    InOrder(root, out);
    out.close();
}

// Menu
void menu()
{
    cout << "(0) Exit program\n";
    cout << "(1) Insert a new student information\n";
    cout << "(2) Update a student info\n";
    cout << "(3) List all students's name\n";
    cout << "(4) List graduated students\n";
    cout << "(5) Delete a student\n";
    cout << "(6) Delete all graduated students\n";
    cout << endl;
    cout << "Your option: ";
}

void addNewStudent(int &n, Node *&root)
{
    n++;
    info newData;

    cout << "Student code: ";
    cin >> newData.ID;

    cout << "Student name: ";
    cin >> newData.name;

    cout << "Student birthday: ";
    cin >> newData.birth;

    cout << "Student status(0: undergrad | 1: graduated): ";
    cin >> newData.status;

    Insert(root, newData);
}

void update(Node *&root)
{
    cout << "Code of the student: ";
    string id;
    cin >> id;

    Node *posUpdate = Search(root, id);
    if (posUpdate == NULL)
    {
        cout << "Do not exist!\n";
        return;
    }

    cout << "An infomation need updating:\n";
    cout << "(1) Name\n";
    cout << "(2) Birthday\n";
    cout << "(3) Status\n";
    cout << "Your choice: ";
    int temp;
    cin >> temp;

    cout << "Change into: ";
    if (temp == 1)
    {
        string name;
        cin >> name;
        posUpdate->data.name = name;
    }

    else if (temp == 2)
    {
        string birth;
        cin >> birth;
        posUpdate->data.birth = birth;
    }

    else if (temp == 3)
    {
        char status;
        cin >> status;
        posUpdate->data.status = status;
    }
}

void listAll(Node *root)
{
    if (root == NULL)
        return;

    listAll(root->left);

    cout << root->data.ID << endl;
    cout << root->data.name << endl;
    cout << root->data.birth << endl;
    cout << root->data.status << endl;
    cout << endl;

    listAll(root->right);
}

void listGraduated(Node *root)
{
    if (root == NULL)
        return;

    listGraduated(root->left);

    if (root->data.status == "1")
    {
        cout << root->data.ID << endl;
        cout << root->data.name << endl;
        cout << root->data.birth << endl;
        cout << root->data.status << endl;
        cout << endl;
    }

    listGraduated(root->right);
}

void deleteAStudent(Node *&root, int &n)
{
    cout << "Code of the student: ";
    string id;
    cin >> id;

    Node *posDel = Search(root, id);
    if (posDel == NULL)
    {
        cout << "Do not exist!\n";
        return;
    }

    root = Delete(root, posDel->data.name);
    n--;
}

void deleteGraduated(Node *&root, int &n)
{
    if (root == NULL)
        return;

    deleteGraduated(root->left, n);
    deleteGraduated(root->right, n);

    if (root->data.status == "1")
    {
        root = Delete(root, root->data.name);
        n--;
    }
}

void menuSelection(int &n, Node *&root)
{
    int select;
    menu();
    cin >> select;
    cout << endl;

    if (select == 0)
        return;

    else if (select == 1)
        addNewStudent(n, root);

    else if (select == 2)
        update(root);

    else if (select == 3)
    {
        cout << "Students's list:\n";
        listAll(root);
        cout << endl;
    }

    else if (select == 4)
    {
        listGraduated(root);
        cout << endl;
    }

    else if (select == 5)
        deleteAStudent(root, n);

    else if (select == 6)
        deleteGraduated(root, n);

    else
        exit(0);
    cout << "Done!\n\n";

    menuSelection(n, root);
}

int main()
{
    int n;
    Node *root = readFile(n);

    menuSelection(n, root);

    char select;
    cout << "Do you want to save your changes?(y/n): ";
    cin >> select;
    if (tolower(select) == 'y')
        writeFile(n, root);

    cout << "Program exit!\n";
    return 0;
}
