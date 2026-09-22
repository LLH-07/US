#pragma once
#include <string>

using namespace std;

class KhachHang
{
private:
    string maKH;
    string tenKH;
    string sdt;

public:
    KhachHang(){}
    KhachHang(string ma, string ten, string dt) : maKH(ma), tenKH(ten), sdt(dt){} 

    string layTenKhachHang()
    {
        return tenKH;
    }
};