#include "Rong.h"

using namespace std;

int main()
{
    // Bai 02
    Rong dragon(5.5, 10, 15, "Hac long", 2);
    dragon.thietLapTrangThai(new CuongNo());
    cout << "Luc chien: " << dragon.tinhLucChien() << endl;
    dragon.tanCong();
    dragon.diChuyen();
}