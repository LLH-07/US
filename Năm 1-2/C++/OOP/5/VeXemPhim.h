#pragma once
#include <iostream>
#include "Phim.h"
#include "KhachHang.h"

using namespace std;

class VeXemPhim
{
protected:
    string maVe;
    Phim phim;
    KhachHang kH;
    string maGhe;
    string ngay_gio;

public:
    VeXemPhim() {}
    VeXemPhim(string ma_ve, Phim p, KhachHang kh, string ghe, string ngayGio) : maVe(ma_ve), phim(p), kH(kh), maGhe(ghe), ngay_gio(ngay_gio) {}

    string layNgayGio();
    double layGiaVeBanDem();
    double layGiaVeBanNgay();
    double layGiaVeCuoiTuan();
    double layGiaVeBan();
    void layThongTin();
};

string VeXemPhim::layNgayGio()
{
    return ngay_gio;
}

bool xacDinhNgayDem(VeXemPhim ve)
{
    string NgayGio = ve.layNgayGio();
    string gio = "";
    gio += NgayGio[0] + NgayGio[1];

    if("06" <= gio && gio < "18") return 0;
    else return 1;

}

double VeXemPhim::layGiaVeBanDem()
{
    double giaVeCB = phim.layGiaVe();
    return giaVeCB + 0.1 * giaVeCB;
}

double VeXemPhim::layGiaVeBanNgay()
{
    double giaBanDem = layGiaVeBanDem();
    return giaBanDem * 0.7;
}

double VeXemPhim::layGiaVeCuoiTuan()
{
    double giaVeCB = phim.layGiaVe();
    return giaVeCB + 0.3 * giaVeCB;
}

void VeXemPhim::layThongTin()
{
    cout << "Ma ve: " << maVe << endl
         << "Ten khach hang: " << kH.layTenKhachHang() << endl
         << "Ten phim: " << phim.layTenPhim() << endl
         << "Ma so ghe ngoi: " << maGhe << endl
         << "Ngay va gio xem: " << ngay_gio << endl
         << "Gia ban ra: ";
}
