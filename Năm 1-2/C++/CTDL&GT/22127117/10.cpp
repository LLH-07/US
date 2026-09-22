#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    ifstream fin("output1.9.bin", ios::binary);

    if(!fin.is_open())
    {
        cout << "File could not be opened!\n";
        return 0;
    }

    int n;

    fin.read((char*)&n, sizeof(n));

    for(int i = 0; i < n; i++)
    {
        cout << i + 1 << ".\n";
        char name[100], birth[100], address[100], classes[100], id[100];

        cout << "Fullname: ";
        fin.read((char *)&name, sizeof(name));
        cout << name << endl;

        cout << "Date of birth: ";
        fin.read((char *)&birth, sizeof(birth));
        cout << birth << endl;

        cout << "Address: ";
        fin.read((char *)&address, sizeof(address));
        cout << address << endl;

        cout << "Class: ";
        fin.read((char *)&classes, sizeof(classes));
        cout << classes << endl;

        cout << "ID: ";
        fin.read((char *)&id, sizeof(id));
        cout << id << endl;

        cout << endl;
    }

    fin.close();
}