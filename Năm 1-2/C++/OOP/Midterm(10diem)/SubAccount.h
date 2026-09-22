#pragma once
#include <string>
#include "Account.h"

using namespace std;

class SubAccount:public Account
{
private:
    string ID;
    string fullname;

public:
    SubAccount(string id, string name) : ID(id), fullname(name){};

    void recharge(long int money)
    {
        balance += money;
    }

    void withdraw(long int money)
    {
       if(money > 3000000)
            cout << "You are not allowed!\n";

        else if(money > balance)
            cout << "Your balance account is not enough!\n";

        else if(money - balance < 50000)
            cout << "INVALID!\n";

        else
            balance -= money;
    }
};