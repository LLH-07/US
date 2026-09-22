#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>

using namespace std;

// NODE (BST)
struct Node
{
    string data;
    Node *left;
    Node *right;
};

Node *createNode(string data)
{
    Node *newNode = new Node;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void insertLeft(Node *&root, string data)
{
    if (root == NULL)
    {
        root = createNode(data);
        return;
    }

    root->left = createNode(data);
}

void insertRight(Node *&root, string data)
{
    if (root == NULL)
    {
        root = createNode(data);
        return;
    }

    root->right = createNode(data);
}

void update(Node *&replaced, string newData)
{
    replaced->data = newData;
}

// READ & WRITE FILE
void readFile(Node *&root)
{
    ifstream in("Q_A.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    Node *temp = NULL;
    Node *parent = NULL;
    string s;

    while (getline(in, s))
    {
        stringstream ss(s);
        string type, question;

        ss >> type;
        ss.ignore();
        getline(ss, question);

        if (type == "#Q")
        {
            if (temp == NULL)
                temp = createNode(question);

            else if (temp->left == NULL)
            {
                insertLeft(temp, question);
                if (parent != NULL)
                    temp = temp->left;
            }

            else if (temp->right == NULL)
            {
                insertRight(temp, question);
                parent = temp->right;
                if (parent != NULL)
                    temp = temp->right;
            }
        }

        else if (type == "#A")
        {
            if (temp->left == NULL)
                insertLeft(temp, question);

            else if (temp->right == NULL)
            {
                insertRight(temp, question);
                temp = parent;
            }
        }

        if (parent == NULL)
        {
            parent = temp;
            root = parent;
        }
    }

    in.close();
}

string type(Node *root)
{
    if (root->left == NULL && root->right == NULL)
        return "#A";

    else
        return "#Q";
}

void writePre(Node *root, ofstream &out)
{
    if (root == NULL)
        return;

    out << type(root) << " " << root->data << endl;
    writePre(root->left, out);
    writePre(root->right, out);
}

void writeFile(Node *root)
{
    ofstream out("Q_A.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    writePre(root, out);

    out.close();
}

//
bool checkVowels(char c)
{
    return c == 'u' || c == 'e' || c == 'o' || c == 'a' || c == 'i';
}

// GAME
void welcomeConsole(short &selection)
{
    cout << " ------------------------------------ \n";
    cout << "|       WELCOME TO ANIMAL GAME       |\n";
    cout << " ------------------------------------ \n";
    cout << "|           (1) Play Game            |\n";
    cout << "|             (2) Exit               |\n";
    cout << " ------------------------------------ \n";
    cout << "Your selection: ";
    cin >> selection;
}

void win()
{
    system("cls");
    cout << "BINGO! I WON!\n";
}

void lose(Node *root)
{
    system("cls");
    cin.ignore();
    cout << "HAIZZ! I don't know that animal!\n";
    cout << "Could you tell me what animal is it? ";
    string animal;
    getline(cin, animal);

    char c;
    cout << "Would you want to give me some details to know what is it (y/n)?: ";
    cin >> c;

    if(tolower(c) == 'n')
    {
        system("cls");
        cout << "OH... (/ ^ \\)\n";
        return;
    }

    system("cls");
    cout << "YAYYY!\n";
    cin.ignore();

    animal[0] = tolower(animal[0]);

    string replacedAnswer = "Is it a";
    if (checkVowels(animal[0]))
        replacedAnswer += "n";
    replacedAnswer += " " + animal + "?";

    string addedQuestion;
    cout << "Please give me a question distinguishing between two of them: ";
    getline(cin, addedQuestion);
    addedQuestion[0] = toupper(addedQuestion[0]);

    char yn;
    cout << "For a";
    if (checkVowels(animal[0]))
        cout << "n";

    cout << " " << animal << " the answer for your question is (y/n): ";
    cin >> yn;

    // Update
    string oldQuestion = root->data;
    update(root, addedQuestion);

    if (tolower(yn) == 'y')
    {
        insertLeft(root, replacedAnswer);
        insertRight(root, oldQuestion);
    }

    else if (tolower(yn) == 'n')
    {
        insertLeft(root, oldQuestion);
        insertRight(root, replacedAnswer);
    }
}

void startGame(Node *root)
{
    system("cls");
    char ans;

    cout << root->data << " (y/n): ";
    cin >> ans;

    ans = tolower(ans);
    if (ans == 'y')
    {
        if (root->left == NULL)
            win();

        else
            startGame(root->left);
    }

    else if (ans == 'n')
    {
        if (root->right == NULL)
            lose(root);
        else
            startGame(root->right);
    }
}

void Game(Node *root)
{
    system("cls");
    short selection;
    welcomeConsole(selection);

    if (selection == 1)
        startGame(root);

    else
    {
        system("cls");
        cout << "GAME STOP!";
        exit(0);
    }
}

void restart(Node* root)
{
    char cont;
    cout << "\nDo you want to play again?(y/n): ";
    cin >> cont;

    startGame(root);

    if (tolower(cont) == 'y') restart(root);
}

void endGame(Node* root)
{
    system("cls");
    cout << "THANK YOU FOR PLAYING!\n";
    writeFile(root);
}

int main()
{
    Node *root = NULL;
    readFile(root);
    Game(root);
    restart(root);
    endGame(root);

    return 0;
}