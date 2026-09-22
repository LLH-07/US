#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Bank.h"

using namespace std;

class Account 
{
private:
    string ID;
    string fullname;

protected:
    long int balance;

public:
    // Constructor
    Account() : ID(""), fullname(""), balance(50000){};
    Account(string cccd, string HoTen) : ID(cccd), fullname(HoTen), balance(50000){};
    Account(string cccd, string HoTen, long int soDu) : ID(cccd), fullname(HoTen), balance(50000 + soDu){};

    // Method
    void recharge(long int money);
    void withdraw(long int money);
    void checkBalance();
    bool operator<(const Account &src);
    bool operator>(const Account &src);
    bool operator==(const Account &src);
    bool operator<=(const Account &src);
    bool operator>=(const Account &src);
    void getAccount();
    long int getBalance();
    string getID();
    void updateBalance(long int money);
};

void Account::recharge(long int money)
{
    cout << "Recharge: " << money << endl;
    balance += money;
    cout << "Balance: " << balance << endl << endl;

    // updateBalance(balance);
}

void Account::withdraw(long int money)
{
    if (money > balance)
    {
        cout << "Balance account is not enough! Please try again!\n";
        return;
    }

    if (balance - money < 50000)
    {
        cout << "INVALID!\n";
        return;
    }

    cout << "Withdraw: " << money << endl;
    balance -= money;
    cout << "Balance: " << balance << endl << endl;

    // updateBalance(balance);
}

void Account::checkBalance()
{
    cout << "Balance Account: " << balance << endl;
}

bool Account::operator<(const Account &src)
{
    if (this->balance < src.balance)
        return true;

    return false;
}

bool Account::operator>(const Account &src)
{
    if (this->balance > src.balance)
        return true;

    return false;
}

bool Account::operator==(const Account &src)
{
    if (this->balance == src.balance)
        return true;

    return false;
}

bool Account::operator<=(const Account &src)
{
    if (this->balance <= src.balance)
        return true;

    return false;
}

bool Account::operator>=(const Account &src)
{
    if (this->balance >= src.balance)
        return true;

    return false;
}

void Account::getAccount()
{
    cout << "ID: " << ID << endl;
    cout << "Fullname: " << fullname << endl;
    cout << "Balance account: " << balance << endl;
    cout << endl;
}

long int Account::getBalance()
{
    return balance;
}

string Account::getID()
{
    return ID;
}

void Account::updateBalance(long int money)
{
    balance = money;
}