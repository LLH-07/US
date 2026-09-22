#include "TicketManager.h"

using namespace std;

int main()
{
    KhachHang k1("20", "Hoa", "123");

    Phim p1("H07", "My Spring", "LLH", 2.5, 45);

    TicketManager quanly;
    VeXemPhim* v1 = quanly.banVe(k1, p1, "18h 25/12/2023", new VeBanDem());
    VeXemPhim* v = quanly.banVe(k1, p1, "18h 25/12/2023", new VeHSSV(new VeBanDem(), k1, p1));
    
    //v1->layThongTinVe();
    v->layThongTinVe();

    return 0;
}