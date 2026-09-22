#include "TicketManager.h"

using namespace std;

int main()
{
    KhachHang k1("20", "Hoa", "123");
    KhachHang k2("07", "Meow", "1234");
    KhachHang k3("04", "Phung", "12345");
    KhachHang k4("12", "Shiba", "123456");

    Phim p1("H07", "My Spring", "LLH", 2.5, 45);
    Phim p2("M20", "Meo Lap Trinh", "HCMUS", 2, 40);
    Phim p3("P12", "My Date", "KHTN", 3, 50);
    Phim p4("S04", "My Bubby", "LY", 3.5, 60);

    TicketManager quanly;
    VeXemPhim* v1 = quanly.banVe(k1, p1, "18h 25/12/2023", new VeBanDem());
    VeXemPhim* v2 = quanly.banVe(k2, p2, "15h 25/12/2023", new VeBanNgay());
    VeXemPhim* v3 = quanly.banVe(k3, p3, "20h 30/12/2023", new VeBanDem());
    VeXemPhim* v4 = quanly.banVe(k4, p4, "14h 24/12/2023", new VeCuoiTuan());



    v1->layThongTinVe();
    v2->layThongTinVe();
    v3->layThongTinVe();
    v4->layThongTinVe();



    return 0;
}