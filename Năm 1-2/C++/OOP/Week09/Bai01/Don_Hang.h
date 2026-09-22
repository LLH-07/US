#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "San_Pham.h"
#include "PhuongThucThanhToan.h"

using namespace std;

class Don_Hang
{
private:
    string maDon;
    string khachHang;
    string sdt;
    string diaChi;
    vector<San_Pham> danhSachSP;
    long tongTien = 0;
    PhuongThucThanhToan* phuongThuc;

public:
    Don_Hang() {}
    Don_Hang(string maDon, string khachHang, string sdt, string diaChi, vector<San_Pham> danhSach)
    {
        this->maDon = maDon;
        this->khachHang = khachHang;
        this->sdt = sdt;
        this->diaChi = diaChi;
        for(int i = 0; i < danhSach.size(); i++)
            this->danhSachSP.push_back(danhSach[i]);
    }

    void pay()
    {
        tinhTongTien();
        phuongThuc->pay(tongTien);
    }
    void tinhTongTien()
    {
        for(int i = 0; i < danhSachSP.size(); i++)
            tongTien += danhSachSP[i].layGiaSP() * danhSachSP[i].laySoLuong();
    }
    void chonPhuongThucThanhToan(PhuongThucThanhToan* pt)
    {
        phuongThuc = pt;
    }
};