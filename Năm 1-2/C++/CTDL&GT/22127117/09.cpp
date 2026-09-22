#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

struct student
{
    char Fullname[100];
    char Birthday[100];
    char Address[100];
    char Class[100];
    char ID[100];
};

int main()
{
    ofstream bout("output1.9.bin", ios::binary);

    if(!bout.is_open())
    {
        cout << "File could not be opened!\n";
        return 0;
    }
    
    int n;
    student S[1000];

    cout << "Number of students: ";
    cin >> n;
    cout << endl;
    bout.write((char*)&n, sizeof(n));
    cin.ignore();

    for (int i = 0; i < n; i++)
    {
        cout << "Fullname: ";
        cin.getline(S[i].Fullname, sizeof(S[i].Fullname));
        bout.write((char *)&S[i].Fullname, sizeof(S[i].Fullname));

        cout << "Date of birth: ";
        cin.getline(S[i].Birthday, sizeof(S[i].Birthday));
        bout.write((char *)&S[i].Birthday, sizeof(S[i].Birthday));

        cout << "Address: ";
        cin.getline(S[i].Address, sizeof(S[i].Address));
        bout.write((char *)&S[i].Address, sizeof(S[i].Address));

        cout << "Class: ";
        cin.getline(S[i].Class, sizeof(S[i].Class));
        bout.write((char *)&S[i].Class, sizeof(S[i].Class));

        cout << "ID: ";
        cin.getline(S[i].ID, sizeof(S[i].ID));
        bout.write((char *)&S[i].ID, sizeof(S[i].ID));
        cout << endl;
    }

    bout.close();
}