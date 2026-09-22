#pragma once
#include <vector>
#include "VeXemPhim.h"

using namespace std;

class TicketManager
{
private:
    vector<VeXemPhim *> danh_sach_ve;
public:
    TicketManager(){}

    VeXemPhim *banVe(KhachHang khach, Phim phim, string ngay_gio, VeXemPhim *loaiVe)
    {
        soluong += 1;
        ma_ve += 1;

        VeXemPhim *ve = loaiVe;
        ve->ganKhachHang(khach);
        ve->ganPhim(phim);
        ve->ganNgayGio(ngay_gio);
        ve->ganMaGhe("H4");
        ve->ganMa();

        danh_sach_ve.push_back(ve);
        return ve;
    }

    VeXemPhim *timVe(int ma)
    {
        for (int i = 0; i < danh_sach_ve.size(); i++)
            if (danh_sach_ve[i]->layMaVe() == ma)
                return danh_sach_ve[i];

        return NULL;
    }

    double tinhTongDoanhThu()
    {
        double tong = 0;
        for (int i = 0; i < danh_sach_ve.size(); i++)
            tong += danh_sach_ve[i]->tinhGiaVe();

        return tong;
    }
};