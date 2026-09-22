#pragma once
#include "Account.h"
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class Bank
{
private:
    string name;
    string address;
    vector<Account*> listAccount;
    int numAccount;

public:
    // Constructor
    Bank(string NAME, string ADDRESS) : name(NAME), address(ADDRESS), numAccount(0){};

    void addAccount(Account* account);
    void sortAccount();
    void getAllAccount();
    long getTotalBalance();
    void getHighestBalance();
    void updateAccount(Account src);
};

void Bank::addAccount(Account* account)
{
    listAccount.push_back(account);
    numAccount++;
}

void Bank::sortAccount()
{
    sort(listAccount.begin(), listAccount.end());
}

void Bank::getAllAccount()
{
    for(int i = numAccount - 1; i >= 0; i--)
        listAccount[i]->getAccount();
}

long Bank::getTotalBalance()
{
    long total = 0;
    for(int i = 0; i < numAccount; i++)
        total += listAccount[i]->getBalance();

    return total;
}

void Bank::getHighestBalance()
{
    cout << "The highest balance: " << listAccount[numAccount - 1]->getBalance() << endl;
}

void Bank::updateAccount(Account src)
{
    for(int i = 0; i < numAccount; i++)
    {
        if(listAccount[i]->getID() == src.getID())
        {
            long int m = src.getBalance();
            listAccount[i]->updateBalance(m);
        }
    }
}