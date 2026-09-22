#pragma once
#include <iostream>
#include <string>

using namespace std;

class PhuongThucThanhToan
{
public:
    virtual void pay(long gia) = 0;
};

class TienMat : public PhuongThucThanhToan
{
    void pay(long gia)
    {
        cout << "Da thanh toan " << gia << " d bang tien mat.\n";
    }
};

class ATM : public PhuongThucThanhToan
{
    void pay(long gia)
    {
        cout << "Da thanh toan " << gia << "d bang the ATM\n";
    }
};

class Momo : public PhuongThucThanhToan
{
    void pay(long gia)
    {
        cout << "Da thanh toan " << gia << "d bang vi Momo\n";
    }
};

class ZaloPay : public PhuongThucThanhToan
{
    void pay(long gia)
    {
        cout << "Da thanh toan " << gia << "d bang Zalo Pay\n";
    }
};
