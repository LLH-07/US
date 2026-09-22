#pragma once
#include <iostream>
#include <string.h>

using namespace std;

class San_Pham
{
private:
    string maSP;
    string tenSP;
    long giaSP;
    int soLuong;

public:
    San_Pham(){}
    San_Pham(string ma, string ten, long gia, int luong) : maSP(ma), tenSP(ten), giaSP(gia), soLuong(luong){}

    long layGiaSP()
    {
        return giaSP;
    }

    int laySoLuong()
    {
        return soLuong;
    }
};
