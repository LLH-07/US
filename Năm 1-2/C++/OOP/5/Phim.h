#pragma once
#include <string>

using namespace std;

class Phim
{
private:
    string maPhim;
    string tenPhim;
    string daoDien;
    double thoiLuong;
    double giaVe;

public:
    Phim(){}
    Phim(string ma, string ten, string dd, double tluong, double gia) : maPhim(ma), tenPhim(ten), daoDien(dd), thoiLuong(tluong), giaVe(gia){}

    double layGiaVe()
    {
        return giaVe;
    }

    string layTenPhim()
    {
        return tenPhim;
    }
};
