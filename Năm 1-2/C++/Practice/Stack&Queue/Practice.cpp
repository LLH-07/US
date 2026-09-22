#include <iostream>
#include <string.h>

using namespace std;

struct Stack
{
    char data;
    Stack *next;
};

bool isEmpty(Stack *&top)
{
    return top == NULL;
}

void Push(char data, Stack *&top)
{
    Stack *newNode = new Stack;
    newNode->data = data;
    newNode->next = top;
    top = newNode;
}

void Pop(Stack *&top)
{
    if (isEmpty(top))
    {
        cout << "Stack underflow!";
        return;
    }

    Stack *temp = top;
    top = top->next;
    delete temp;
}

char Top(Stack *&top)
{
    if (isEmpty(top))
    {
        cout << "Stack underflow!";
        return '\0';
    }

    return top->data;
}

// Problem B1
string reverse(string str)
{
    Stack *s = NULL;
    for (int i = 0; i < str.length(); i++)
        Push(str[i], s);

    string res = "";
    while (!isEmpty(s))
    {
        res += Top(s);
        Pop(s);
    }

    return res;
}

// Problem B2
bool isOpeningParentheses(char c)
{
    return c == '(' || c == '[' || c == '{';
}

bool isBalancedParentheses(string str)
{
    Stack *s = NULL;
    for (int i = 0; i < str.size(); i++)
    {
        if (isOpeningParentheses(str[i]))
            Push(str[i], s);

        else
        {
            if (str[i] == ')')
            {
                if (Top(s) != '(')
                    return false;

                else
                    Pop(s);
            }

            else if (str[i] == ']')
            {
                if (Top(s) != '[')
                    return false;

                else
                    Pop(s);
            }

            else if (str[i] == '}')
            {
                if (Top(s) != '{')
                    return false;

                else
                    Pop(s);
            }
        }
    }

    return isEmpty(s);
}

// Problem B3
string convertToBinary(int decimal)
{
    Stack *s = NULL;
    string res = "";

    while (decimal != 0)
    {
        Push(char((decimal % 2) + 48), s);
        decimal /= 2;
    }

    while (!isEmpty(s))
    {
        res += Top(s);
        Pop(s);
    }
    return res;
}

// Problem B4
struct customer
{
    int arrivalTime;
    int processingTime;
};

int countCustomerLeft(customer c[], int numCustomer)
{
    int passedTime = 0, leftCustomer = 0;

    for (int i = 0; i < numCustomer; i++)
    {
        if (passedTime - c[i].arrivalTime <= 10)
        {
            passedTime = max(passedTime, c[i].arrivalTime);
            passedTime += c[i].processingTime;
        }

        else
            leftCustomer++;
    }

    return leftCustomer;
}

int main()
{
    customer c[6] = {{5, 9}, {7, 5}, {14, 5}, {30, 15}, {32, 5}, {34, 5}};
    cout << countCustomerLeft(c, 6);
}
