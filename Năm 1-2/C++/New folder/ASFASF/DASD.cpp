#include <iostream>

using namespace std;

int main()
{
    int day, month, year, namnhuan = 0;
    cout << "Nhap vao ngay, thang, nam: ";
    cin >> day >> month >> year;

    if((year % 400 == 0)) namnhuan = 1;
    else if (year % 4 == 0 && year % 100 !=0) namnhuan == 1;

    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
        if (day == 31)
            {
                cout << "Ngay tiep theo la ngay 1 thang " ;

                if (month == 12) cout << 1 << " nam " << year + 1;
                else cout << month + 1 << " nam " << year;
            }
        else cout << "Ngay tiep theo la ngay " << day + 1 << " thang "  << month + 1 << " nam " << year;
    }

    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        if (day == 31)
            {
                cout << "Ngay tiep theo la ngay 1 thang " ;
                cout << month + 1 << " nam " << year;
            }
        else cout << "Ngay tiep theo la ngay " << day + 1 << " thang "  << month + 1 << " nam " << year;
    }

    else if (month == 2)
    {
        if (day == 28) {
            if (namnhuan == 1) cout << "Ngay tiep thap la ngay 29 thang 2 nam " << year;
            else cout << "Ngay tiep theo la ngay 1 thang 3 nam " << year;
        }
    }
    else cout << "Khong hop le!";
    return 0;
}
