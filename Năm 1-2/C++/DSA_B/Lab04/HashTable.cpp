#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Company
{
    string name;
    string profit_tax;
    string address;
};

vector<Company> ReadCompanyList(string file_name)
{
    ifstream in(file_name);
    if (!in.is_open())
    {
        cout << "ERROR!\n";
        exit(0);
    }

    string ignoreLine;
    vector<Company> list;

    getline(in, ignoreLine);

    while (!in.eof())
    {
        Company tmp;
        getline(in, tmp.name, '|');
        getline(in, tmp.profit_tax, '|');
        getline(in, tmp.address);
        list.push_back(tmp);
    }

    in.close();

    return list;
}

void printList(vector<Company> list)
{
    list = ReadCompanyList("MST.txt");
    for (int i = 0; i < list.size(); i++)
    {
        cout << list[i].name << endl
             << list[i].profit_tax << endl
             << list[i].address << endl
             << endl;

        cout << "----------------------------------------\n\n";
    }
}

long long HashString(string company_name)
{
    long long res = 0;
    short j = 0;
    for (int i = company_name.size() - 20; i < company_name.size(); i++)
    {
        res += company_name[i] * (long long)(pow(31, j));
        j++;
    }

    res = res % (1000000000 + 9);
    return res;
}

long long hashFunction(string company_name)
{
    return HashString(company_name) % 2000;
}

void Insert(Company *hashTable, Company company)
{
    long long idx = hashFunction(company.name);

    while (hashTable[idx].name != "")
    {
        if (hashTable[idx].name == company.name)
            return;

        idx++;
        if (idx == 2000)
            idx = 0;
    }

    hashTable[idx].name = company.name;
}

Company *CreateHashTable(vector<Company> list_company)
{
    Company *hashTable = new Company[2000];
    for (int i = 0; i < 2000; i++)
        hashTable[i] = {"", "", ""};

    for (int i = 0; i < list_company.size(); i++)
        Insert(hashTable, list_company[i]);

    return hashTable;
}

Company *Search(Company *hash_table, string company_name)
{
    long long idx = hashFunction(company_name);
    long long tmp = idx;
    while (hash_table[idx].name != "")
    {
        idx++;
        if (idx == 2000)
            idx = 0;
        if (idx == tmp)
            break;
        if (hash_table[idx].name == company_name)
            return hash_table;
    }

    return NULL;
}

int main()
{
    vector<Company> list;
    list = ReadCompanyList("MST.txt");
    cout << HashString(list[0].name) << endl;
    // printList(list);
}
