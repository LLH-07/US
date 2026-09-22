#include <iostream>
#include "Don_Hang.h"
#include "Rong.h"

using namespace std;

int main()
{
    // Bai 01
    vector<San_Pham> sp;
    San_Pham sp1("1111", "Ao", 65000, 2);
    San_Pham sp2("2222", "Quan", 170000, 1);
    sp.push_back(sp1);
    sp.push_back(sp2);
    Don_Hang dh("3333", "Ly Lien Hoa", "1234560", "HCM", sp);
    dh.chonPhuongThucThanhToan(new ATM());
    dh.pay();

    // Bai 02
    Rong dragon(5.5, 10, 15, "Hac long", 2);
    dragon.thietLapTrangThai(new CuongNo());
    cout << "Luc chien: " << dragon.tinhLucChien() << endl;
    dragon.tanCong();
    dragon.diChuyen();
};