#include <iostream>
#include <sstream>
#include <cstring>
#include <string.h>

using namespace std;

struct Stack
{
    double data;
    Stack* link;
};

bool isEmpty(Stack *&top)
{
    return (top == NULL);
}

void Push(double data, Stack *&top)
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

double Top(Stack *&top)
{
    if (isEmpty(top))
        return 0;

    double temp = top->data;
    return temp;
}

bool isOperator(char op)
{
    return op == '+' || op == '-' || op == '*' || op == '/';
}

double calculate(double a, double b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    
    case '-':
        return a - b;

    case '*':
        return a * b;

    case '/':
        return a / b;
    }

    return 0;
}

double evaluate(string postfix)
{
    Stack* s = NULL;
    stringstream ss(postfix);

    while(!ss.eof())
    {
        string temp;
        ss >> temp;

        if(isdigit(temp[0]))
        {
            double operand = stod(temp);
            Push(operand, s);
        }

        else if(isOperator(temp[0]))
        {
            double operand1, operand2;
            operand2 = Top(s);
            Pop(s);
            operand1 = Top(s);
            Pop(s);

            if(operand1 == operand2 && operand1 == 0 && temp[0] == '/')
            {
                cout << "Error: Division by zero!" << endl;
                return 0;
            }

            Push(calculate(operand1, operand2, temp[0]), s);
        }
    }

    double res = Top(s);
    delete s;
    
    return res;
}

int main()
{
    string s = "12 5 +";
    cout << evaluate(s);
}