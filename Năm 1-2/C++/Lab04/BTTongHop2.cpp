#include <iostream>

using namespace std;

void getData(float nhietdo[2][12])
{
    for (int i = 0; i < 2; i++)
    {
        if(i == 0) cout << "Nhiet do cao nhat: \n";
        else cout << "Nhiet do thap nhat: \n";
        for(int j = 0; j < 12; j++)
        {
            cout << "Thang " << j + 1 << ": ";
            cin >> nhietdo[i][j];
        }
    }
}

float averageHigh(float nhietdo[2][12])
{
    float avg = 0;
    for(int j = 0; j < 12; j++) avg += nhietdo[0][j];
    return avg/12;
}

float averageLow(float nhietdo[2][12])
{
    float avg = 0;
    for(int j = 0; j < 12; j++) avg += nhietdo[1][j];
    return avg/12;
}

int indexHighTemp(float nhietdo[2][12])
{
    int mx = -1, pos;
    for(int j = 0; j < 12; j++)
    {
        if(mx < nhietdo[0][j])
        {
            mx = nhietdo[0][j];
            pos = j;
        }
    }
    return pos;
    //cout << "Vi tri co nhiet do cao nhat o dong 1 cot " << pos + 1 << endl;
}

int indexLowTemp(float nhietdo[2][12])
{
    int mn = 1000, pos;
    for(int j = 0; j < 12; j++)
    {
        if(mn > nhietdo[1][j])
        {
            mn = nhietdo[1][j];
            pos = j;
        }
    }
    //cout << "Vi tri co nhiet do cao nhat o dong 2 cot " << pos + 1;
    return pos;
}

int main()
{
    float nhietdo[2][12];
    float nhietdoTBcao, nhiedoTBthap;
    getData(nhietdo);
    nhietdoTBcao = averageHigh(nhietdo);
    nhiedoTBthap = averageLow(nhietdo);

    cout << "Trung binh nhiet do cao: " << nhietdoTBcao << endl;
    cout << "Trung binh nhiet do thap: " << nhiedoTBthap << endl;
    cout << "Nhiet do cao nhat: " << nhietdo[0][indexHighTemp(nhietdo)] << endl;
    cout << "Nhiet do thap nhat: " << nhietdo[0][indexLowTemp(nhietdo)];
}