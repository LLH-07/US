#pragma once
#include "TrangThai.h"

using namespace std;

class Rong
{
private:
    double satThuong;
    double phongThu;
    double hp;
    string ten;
    short level;
    TrangThai* tt;

public:
    Rong(){}
    Rong(double st, double pt, double mau, string t, short lv) : satThuong(st), phongThu(pt), hp(mau), ten(t), level(lv){}

    void thietLapTrangThai(TrangThai* tt)
    {
        this->tt = tt;
    }

    double tinhLucChien()
    {
        return tt->tinhLucChien(satThuong, phongThu, hp, level);
    }

    void tanCong()
    {
        return tt->tanCong(ten);
    }

    void diChuyen()
    {
        return tt->diChuyen(ten);
    }
};