#pragma once
#include <iostream>
#include <string>
#include "Rong.h"

using namespace std;

class TrangThai
{
public:
    virtual double tinhLucChien(double satThuong, double phongThu, double hp, short level) = 0;
    virtual void tanCong(string ten) = 0;
    virtual void diChuyen(string ten) = 0;
};

class BinhThuong : public TrangThai
{
    double tinhLucChien(double satThuong, double phongThu, double hp, short level)
    {
        return (satThuong + phongThu + hp) * level * 1.0;
    }

    void tanCong(string ten)
    {
        cout << "Rong " << ten << " phun lua mau vang" << endl;
    }

    void diChuyen(string ten)
    {
        cout << "Rong " << ten << " di chuyen toc do binh thuong" << endl;
    }
};

class CuongNo : public TrangThai
{
    double tinhLucChien(double satThuong, double phongThu, double hp, short level)
    {
        return (2.0 * satThuong + phongThu + hp) * level * 1.0;
    }

    void tanCong(string ten)
    {
        cout << "Rong " << ten << " phun lua mau do"  << endl;
    }

    void diChuyen(string ten)
    {
        cout << "Rong " << ten << " di chuyen nhanh" << endl;
    }
};

class PhongThu : public TrangThai
{
    double tinhLucChien(double satThuong, double phongThu, double hp, short level)
    {
        return (satThuong + phongThu * 1.5 + hp * 1.5) * level * 1.0;
    }

    void tanCong(string ten)
    {
        cout << "Rong " << ten << " phun lua mau xanh la" << endl;
    }

    void diChuyen(string ten)
    {
        cout << "Rong " << ten << " di chuyen cham";
    }
};