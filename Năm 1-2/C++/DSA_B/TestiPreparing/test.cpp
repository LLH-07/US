#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Node
{
    int data;
    Node* next = nullptr;
};

struct List
{
    Node* head = nullptr;
    Node* tail = nullptr;
};

Node* createNode(int data)
{
    Node* newNode = new Node;
    newNode->data = data;
    return newNode;
}

void printList(List* list)
{
    if(list == nullptr)
    {
        cout << "List is empty!\n";
        return;
    }

    Node* curr = list->head;
    while(curr != nullptr)
    {
        cout << curr->data << ' ';
        curr = curr->next;
    }
}

void removeHead(List *&L)
{
    if (L == nullptr || L->head == nullptr)
        return;

    Node *tmp = L->head;

    L->head = tmp->next;

    delete tmp;
}

void removeTail(List *&L)
{
    if (L == nullptr || L->head == nullptr)
        return;

    Node *tmp = L->head;

    if (tmp == L->tail)
    {
        delete tmp;
        L->head = L->tail = nullptr;
        return;
    }

    while (tmp->next != L->tail)
        tmp = tmp->next;

    delete L->tail;
    tmp->next = nullptr;
    L->tail = tmp;
}

void removeAll(List *&L)
{
    while (L != nullptr && L->head != nullptr)
        removeHead(L);

    return;
}

void removeBefore(List *&L, int val)
{
    if (L == nullptr || L->head == nullptr || L->head->next == nullptr || L->head->data == val)
        return;

    Node *prev = nullptr;
    Node *curr = L->head;
    Node *next = curr->next;

    if (next != nullptr && next->data == val)
    {
        removeHead(L);
        return;
    }

    while (next != nullptr && next->data != val)
    {
        prev = curr;
        curr = next;
        next = next->next;
    }

    if (next != nullptr && next->data == val)
    {
        prev->next = next;
        delete curr;
    }
}

void removeAfter(List *&L, int val)
{
    if (L == nullptr || L->head == nullptr || L->head->next == nullptr)
        return;

    Node *curr = L->head;
    Node *next = curr->next;

    while (next->next != nullptr && curr->data != val)
    {
        curr = next;
        next = next->next;
    }

    if (curr->data == val)
    {
        curr->next = next->next;
        delete next;
    }

    return;
}

void removePos(List *&L, int data, int pos)
{
    if (pos < 0 || L == nullptr || L->head == nullptr)
        return;

    if (pos == 0)
    {
        removeHead(L);
        return;
    }

    int i = 0;
    Node *tmp = L->head;

    while (tmp->next != nullptr && i < pos - 1)
    {
        tmp = tmp->next;
        i++;
    }

    if (i == pos - 1 && tmp->next != nullptr)
    {
        Node *deleted = tmp->next;
        tmp->next = deleted->next;
        delete deleted;
    }
}

int*** create3DMatrix(int depth, int row, int col)
{
    int ***a = new int**[depth];
    for(int i = 0; i < depth; i++)
    {
        a[i] = new int*[row];
        for(int j = 0; j < row; j++)
            a[i][j] = new int[col];
    }

    return a;
}

void delete3DMatrix(int depth, int row, int col, int***a)
{
    for(int i = 0; i < depth; i++)
    {
        for(int j = 0; j < row; j++)
            delete[] a[i][j];

        delete[] a[i];
    }

    delete[] a;
}

// ---------------------------------------------------------
void maxSubmatrixSum(int** matrix, int r, int c) 
{ 
  
    // Stores maximum submatrix sum 
    int maxSubmatrix = 0; 
  
    // Take each row as starting row 
    for (int i = 0; i < r; i++) { 
  
        // Take each column as the 
        // starting column 
        for (int j = 0; j < c; j++) { 
  
            // Take each row as the 
            // ending row 
            for (int k = i; k < r; k++) { 
  
                // Take each column as 
                // the ending column 
                for (int l = j; l < c; l++) { 
  
                    // Stores the sum of submatrix 
                    // having topleft index(i, j) 
                    // and bottom right index (k, l) 
                    int sumSubmatrix = 0; 
  
                    // Iterate the submatrix 
                    // row-wise and calculate its sum 
                    for (int m = i; m <= k; m++) { 
                        for (int n = j; n <= l; n++) { 
                            sumSubmatrix += matrix[m][n]; 
                        } 
                    } 
  
                    // Update the maximum sum 
                    maxSubmatrix 
                        = max(maxSubmatrix, 
                              sumSubmatrix); 
                } 
            } 
        } 
    } 
  
    // Print the answer 
    cout << maxSubmatrix; 
} 


// ---------------------------------------------------------
int getSum(int r1, int r2, int c1, int c2,
           int dp[][4])
{
    return dp[r2][c2] - dp[r2][c1] - dp[r1][c2]
           + dp[r1][c1];
}
 
// Function that returns true if it is possible
// to find the sub-matrix with required sum
bool sumFound(int K, int S, int grid[][1000])
{
 
    // 2-D array to store the sum of
    // all the sub-matrices
    int dp[4][4];
 
    // Filling of dp[][] array
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dp[i + 1][j + 1] = dp[i + 1][j] + dp[i][j + 1]
                               - dp[i][j] + grid[i][j];
 
    // Checking for each possible sub-matrix of size k X k
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            int sum = getSum(i, i + K, j, j + K, dp);
 
            if (sum == S)
                return true;
        }
 
    // Sub-matrix with the given sum not found
    return false;
}


// --------------------------------------------------------
struct DoB
{
    short day, month, year;
};

struct Player
{
    string name = "";
    DoB dob;
    short height = 0;
    short weight = 0;
    string nation = "";
    string club = "";
};

void readFile(string filename, Player p[], int& n)
{
    ifstream in(filename);
    if(!in.is_open())
    {
        cout << "FILE ERROR!\n";
        return;
    }

    string ignoreLine;
    char ignoreChar;
    getline(in, ignoreLine);
    n = 0;

    while(!in.eof())
    {
        getline(in, p[n].name, ',');

        in >> p[n].dob.day;
        in >> ignoreChar;
        in >> p[n].dob.month;
        in >> ignoreChar;
        in >> p[n].dob.year;
        in >> ignoreChar;

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