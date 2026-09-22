#include <iostream>
#include "Bank.h"


using namespace std;

int main()
{
    Bank ABC("ABC", "TP Ho Chi Minh, Viet Nam");
    Account Huy("01234", "Huy", 1000000);
    ABC.addAccount(&Huy);
    Huy.withdraw(2000000);
    Huy.withdraw(500000);
    Account Khanh("56789", "Khanh", 1000000);
    ABC.addAccount(&Khanh);
    Khanh.recharge(5000000);
    //ABC.updateAccount(Khanh);
    //ABC.updateAccount(Huy);
    ABC.getAllAccount();
    ABC.getHighestBalance();
}