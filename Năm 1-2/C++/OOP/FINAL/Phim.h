#pragma once
#include <string>

using namespace std;

class Phim
{
private:
    string maPhim;
    string tenPhim;
    string dao_dien;
    double thoi_luong;
    double giaVe;

public:
    Phim(){}
    Phim(string ma, string ten, string dd, double tl, double gia) : maPhim(ma), tenPhim(ten), dao_dien(dd), thoi_luong(tl), giaVe(gia){}

    double layGiaVeCoBan()
    {
        return giaVe;
    }

    string layTenPhim()
    {
        return tenPhim;
    }
};