#include <iostream>
#include <cstring>
#include <string.h>
#include <string>

using namespace std;

// Problem 1
struct movie
{
    char *ID;
    char *name;
    char *producer;
    int year;
    int rank;
};

void getMovie(movie &m)
{
    cin.ignore();
    m.ID = new char[30];
    m.name = new char[100];
    m.producer = new char[30];

    cout << "ID: ";
    cin.getline(m.ID, 30);

    cout << "Name: ";
    cin.getline(m.name, 100);

    cout << "Producer: ";
    cin.getline(m.producer, 100);

    cout << "Produced-year: ";
    cin >> m.year;

    cout << "Rank: ";
    cin >> m.rank;
}

void showMovie(movie m)
{
    cout << "ID: " << m.ID << endl;
    cout << "Name: " << m.name << endl;
    cout << "Producer: " << m.producer << endl;
    cout << "Produced-year: " << m.year << endl;
    cout << "Rank: " << m.rank << endl;
    cout << endl;
}

struct Node
{
    movie data;
    Node *next;
};

Node *createNode(movie m)
{
    Node *newNode = new Node;
    newNode->data = m;
    newNode->next = NULL;
    return newNode;
}

void addHead(Node *&head, movie m)
{
    Node *node = createNode(m);
    node->next = head;
    head = node;
}

void delHead(Node *&head)
{
    if(head == NULL)
        return;

    Node* del = head;
    head = head->next;
    delete del;
}

void getMovieList(Node *&head)
{
    head = NULL;
    int n;
    cout << "Number of movies in list: ";
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        movie Movie;
        getMovie(Movie);
        addHead(head, Movie);
    }
}

void showMovieList(Node *&head)
{
    if (head == NULL)
    {
        cout << "List is empty!";
        return;
    }

    Node *curr = head;
    while (curr != NULL)
    {
        showMovie(curr->data);
        curr = curr->next;
    }
}

void delete2010(Node *&head)
{
    if (head == NULL)
    {
        cout << "List is empty";
        return;
    }

    if(head->data.year == 2010)
    {
        delHead(head);
        return;
    }

    Node* prev = head;
    Node* curr = head->next;

    while(curr != NULL)
    {
        if(curr->data.year == 2010)
        {
            prev->next = curr->next;
            delete curr;
            return;
        }

        prev = curr;
        curr = curr->next;
    }
}
