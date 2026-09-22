#include <iostream>
#include <stack>
#include <queue>
#include <string.h>

using namespace std;

string Stack(string str)
{
    stack<char> s;
    string res = "";
    for(int i = 0; i < str.length(); i++)
    {
        if(isalpha(str[i]))
            s.push(str[i]);
        
        else if(str[i] == '*')
        {
            res += s.top();
            s.pop();
        }
    }

    return res;
}

string Queue(string str)
{
    queue<char> s;
    string res = "";
    for(int i = 0; i < str.length(); i++)
    {
        if(isalpha(str[i]))
            s.push(str[i]);
        
        else if(str[i] == '*')
        {
            res += s.front();
            s.pop();
        }
    }

    return res;
}

int main()
{
    string s1 = "EAS*Y*QUE***ST***IO*N***";
    string s2 = "LA*STI*N*FIR*ST**OU*T******";

    cout << "Stack: " << endl;
    cout << Stack(s1) << endl << Stack(s2) << endl << endl;
    cout << "Queue: " << endl;
    cout << Queue(s1) << endl << Queue(s2) << endl;
}