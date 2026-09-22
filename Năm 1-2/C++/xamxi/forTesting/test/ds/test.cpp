#include <iostream>
#include <string>
#include <cstring>

using namespace std;

struct Stack
{
    char data;
    Stack *link;
};

bool isEmpty(Stack *&top)
{
    if (top == NULL)
        return true;

    return false;
}

void Push(char data, Stack *&top)
{
    Stack *newNode = new Stack;
    newNode->data = data;
    newNode->link = top;
    top = newNode;
}

void Pop(Stack *&top)
{
    if (isEmpty(top))
    {
        cout << "Stack is underflow!\n";
        return;
    }

    Stack *temp = top;
    top = top->link;
    delete temp;
}

char Top(Stack *&top)
{
    if (isEmpty(top))
        return '\0';

    char c = top->data;
    return c;
}

bool isHigherOperator(char op, char newOperator)
{
    if (op == '+' || op == '-')
        if (newOperator == '*' || newOperator == '/')
            return true;

    return false;
}

bool isOperator(char op)
{
    return op == '+' || op == '-' || op == '*' || op == '/';
}

string covertInfixToPostfix(string equation)
{
    int len = equation.size();
    string res = "";
    Stack *s = NULL;

    for (int i = 0; i < len; i++)
    {
        if (isalnum(equation[i]))
        {
            res += equation[i];
            res += " ";
        }
        else if (equation[i] == '(')
            Push(equation[i], s);

        else if (isOperator(equation[i]))
        {
            if (isEmpty(s))
                Push(equation[i], s);

            else
            {
                if (isHigherOperator(Top(s), equation[i]))
                    Push(equation[i], s);

                else
                {
                    res += Top(s);
                    res += " ";
                    Pop(s);
                    Push(equation[i], s);
                }
            }
        }
        else if (equation[i] == ')')
        {
            while (Top(s) != '(')
            {
                res += Top(s);
                res += " ";
                Pop(s);
                if (isEmpty(s))
                {
                    res = "Error!";
                    return res;
                }
            }
            Pop(s);
        }
    }

    while (!isEmpty(s))
    {
        res += Top(s);
        res += " ";
        Pop(s);
    }
    return res;
}

int main()
{
    string equation = "A + B - C * D";
    cout << covertInfixToPostfix(equation) << endl;
}