#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <sstream>

#define table_size 20000

using namespace std;

struct Monster
{
    string name = "";
    string phoneNumber = "";
};


Monster MonsterList[table_size];

// Hashing part
unsigned int hashCode(string name)
{
    unsigned int result = 0;
    const int prime_const = 31;
    int P = 1;
    for (int i = 0; i < name.length(); i++)
    {
        result += name[i] * P;
        P *= 31;
    }

    return result;
}

void h(Monster m)
{
    unsigned int hash = hashCode(m.name);
    int pos = hash % table_size;

    if (MonsterList[pos].name != "")
    {
        int temp = 0;
        int tempPos = pos;
        do
        {
            temp++;
            pos = (hash + temp) % table_size;
        } while (MonsterList[pos].name != "" && tempPos != pos);

        if (pos == tempPos)
        {
            cout << "Can't add!\n";
            return;
        }
    }

    MonsterList[pos].name = m.name;
    MonsterList[pos].phoneNumber = m.phoneNumber;
}

// File part
void readFile()
{
    ifstream in("phonebook.txt");
    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    while (!in.eof())
    {
        //cout << 2 << endl;
        Monster m;
        string temp;
        getline(in, temp);
        stringstream ss(temp);

        temp = "";

        ss >> m.name;
        while(ss >> temp && !isdigit(temp[0]))
        {
                m.name += " " + temp;
        }

        m.phoneNumber = temp;

        h(m);
    }

    in.close();
}

void writeFile()
{
    ofstream out("phonebook.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!\n";
        exit(0);
    }

    for (int i = 0; i < table_size; i++)
        if (MonsterList[i].name != "")
            out << MonsterList[i].name << "  " << MonsterList[i].phoneNumber << endl;

    out.close();
}

// Function part
void menu()
{
    cout << "Press 0 to exit\n";
    cout << "Press 1 to add a monster with phone number to the phone book\n";
    cout << "Press 2 to delete a monster with phone number from the phone book\n";
    cout << "Press 3 to update a phone number\n";
    cout << "Press 4 to find a monster\n";
}

int searchName(Monster m)
{
    unsigned int hash = hashCode(m.name);
    int pos = hash % table_size;

    if (MonsterList[pos].name == m.name)
        return pos;

    int tempPos = pos;
    int temp = 1;

    do
    {
        pos = (temp + hash) % table_size;
        temp++;
    } while (MonsterList[pos].name != m.name && pos != tempPos);

    if (MonsterList[pos].name == m.name)
        return pos;

    return -1;
}

void updates(Monster m, int p)
{
    cout << "Update phone number of : ";
    cin.ignore();
    getline(cin, m.name);

    cout << "New phone number: ";
    cin >> p;

    int pos = searchName(m);
    if (pos == -1)
    {
        cout << "ERROR!\n\n";
        return;
    }

    string temp = to_string(p);
    MonsterList[pos].phoneNumber = temp;
    cout << "SUCCESSFUL!\n\n";
}

void add(Monster m, int p)
{
    cout << "Name: ";
    cin.ignore();
    getline(cin, m.name);

    cout << "Phone number: ";
    cin >> p;

    m.phoneNumber = to_string(p);

    int pos = searchName(m);
    if (pos != -1)
        MonsterList[pos].phoneNumber = m.phoneNumber;

    else
        h(m);

    cout << "SUCCESSFUL!\n\n";
}

void Delete(Monster m)
{
    cout << "Delete name: ";
    cin.ignore();
    getline(cin, m.name);

    int pos = searchName(m);
    if (pos == -1)
    {
        cout << "ERROR!\n\n";
        return;
    }

    MonsterList[pos].name = "";
    MonsterList[pos].phoneNumber = "";
    cout << "SUCCESSFUL!\n\n";
}

string find(Monster m)
{
    cout << "Search name: ";
    cin.ignore();
    getline(cin, m.name);

    int pos = searchName(m);
    if (pos == -1)
    {
        return "ERROR!";
    }

    return MonsterList[pos].phoneNumber;
}

void menuSelection()
{
    menu();
    unsigned short opt;
    Monster m;

    cout << "\nYour option: ";
    cin >> opt;

    if (opt == 0)
    {
        writeFile();
        cout << "SUCCESSFUL!\n";
        exit(0);
    }

    else if (opt == 1)
    {
        int p;
        add(m, p);
    }

    else if (opt == 2)
    {
        Delete(m);
    }

    else if (opt == 3)
    {
        int p;
        updates(m, p);
    }

    else if (opt == 4)
    {
        string temp = find(m);
        if(temp != "ERROR!")
            cout << "Phone number: " << temp << endl << "SUCCESSFUL!\n\n";

        else cout << temp << endl << endl;
    }

    menuSelection();
}

int main()
{
    readFile();
    menuSelection();
    return 0;
}