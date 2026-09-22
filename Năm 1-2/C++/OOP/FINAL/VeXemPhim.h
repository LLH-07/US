#pragma once
#include <iostream>
#include "Phim.h"
#include "KhachHang.h"

using namespace std;

static int soluong = 0;
static int ma_ve = 0;

class VeXemPhim
{
protected:
    int maVe;
    Phim phim;
    KhachHang KH;
    string maGhe;
    string ngay_gio;
    double gia;

public:
    VeXemPhim() {}
    VeXemPhim(int ma, Phim p, KhachHang c, string ghe, string ngayGio) : maVe(ma), phim(p), KH(c), maGhe(ghe), ngay_gio(ngayGio) {}

    virtual double tinhGiaVe() = 0;
    virtual void layThongTinVe() = 0;
    void ganKhachHang(KhachHang kh)
    {
        KH = kh;
    }
    void ganPhim(Phim p)
    {
        phim = p;
    }
    void ganNgayGio(string ngayGio)
    {
        ngay_gio = ngayGio;
    }
    void ganMaGhe(string ma)
    {
        maGhe = ma;
    }
    void ganMa()
    {
        maVe = ma_ve;
    }
    int layMaVe()
    {
        return maVe;
    }
};

class VeBanDem : virtual public VeXemPhim
{
public:
    virtual double tinhGiaVe()
    {
        gia = phim.layGiaVeCoBan() * 1.1;
        return gia;
    }

    virtual void layThongTinVe()
    {
        cout << "Ma ve: " << maVe << endl
             << "Ten khach hang: " << KH.layTenKhachHang() << endl
             << "Ten phim: " << phim.layTenPhim() << endl
             << "Ma so ghe: " << maGhe << endl
             << "Ngay va gio: " << ngay_gio << endl
             << "Gia ban ra: " << tinhGiaVe() << endl
             << endl;
    }
};

class VeBanNgay : virtual public VeXemPhim
{
public:
    virtual double tinhGiaVe()
    {
        gia = (phim.layGiaVeCoBan() * 1.1) * 0.7;
        return gia;
    }

    virtual void layThongTinVe()
    {
        cout << "Ma ve: " << maVe << endl
             << "Ten khach hang: " << KH.layTenKhachHang() << endl
             << "Ten phim: " << phim.layTenPhim() << endl
             << "Ma so ghe: " << maGhe << endl
             << "Ngay va gio: " << ngay_gio << endl
             << "Gia ban ra: " << tinhGiaVe() << endl;
    }
};

class VeCuoiTuan : virtual public VeXemPhim
{
public:
    virtual double tinhGiaVe()
    {
        gia = phim.layGiaVeCoBan() * 1.3;
        return gia;
    }

    virtual void layThongTinVe()
    {
        cout << "Ma ve: " << maVe << endl
             << "Ten khach hang: " << KH.layTenKhachHang() << endl
             << "Ten phim: " << phim.layTenPhim() << endl
             << "Ma so ghe: " << maGhe << endl
             << "Ngay va gio: " << ngay_gio << endl
             << "Gia ban ra: " << tinhGiaVe() << endl;
    }
};

class VeHSSV : public VeBanDem, public VeBanNgay, public VeCuoiTuan
{
public:
    VeHSSV(VeXemPhim* loaiVe, KhachHang khach, Phim p)
    {
        VeXemPhim* ve = loaiVe;
        ve->ganKhachHang(khach);
        ve->ganPhim(p);
        gia = ve->tinhGiaVe();
    }

    double tinhGiaVe()
    {
        return gia * 0.7;
    }

    void layThongTinVe()
    {
        cout << "Ma ve: " << maVe << endl
             << "Ten khach hang: " << KH.layTenKhachHang() << endl
             << "Ten phim: " << phim.layTenPhim() << endl
             << "Ma so ghe: " << maGhe << endl
             << "Ngay va gio: " << ngay_gio << endl
             << "Gia ban ra: " << tinhGiaVe() << endl;
    }
};

class VeVIP : public VeBanDem, public VeBanNgay, public VeCuoiTuan
{
public:
    VeVIP(VeXemPhim* loaiVe, KhachHang khach, Phim p)
    {
        VeXemPhim* ve = loaiVe;
        ve->ganKhachHang(khach);
        ve->ganPhim(p);
        gia = ve->tinhGiaVe();
    }

    double tinhGiaVe()
    {
        return gia * 0.85;
    }

    void layThongTinVe()
    {
        cout << "Ma ve: " << maVe << endl
             << "Ten khach hang: " << KH.layTenKhachHang() << endl
             << "Ten phim: " << phim.layTenPhim() << endl
             << "Ma so ghe: " << maGhe << endl
             << "Ngay va gio: " << ngay_gio << endl
             << "Gia ban ra: " << tinhGiaVe() << endl;
    }
};