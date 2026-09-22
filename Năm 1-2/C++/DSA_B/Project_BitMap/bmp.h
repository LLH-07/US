#pragma once
#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

struct BMP_Header
{
    char type[2];      // Ma nhan dang file bitmap
    unsigned int size; // Kich thuoc cua file (byte)
    int reserved;      // Du tru (Khong su dung)
    int offset;        // Vi tri bat dau hinh anh
};

struct BMP_Info
{
    unsigned long infoSize;
    unsigned long width;       // Chieu rong anh (pixel)
    unsigned long height;      // Chieu dai anh (pixel)
    unsigned short planes = 1; // Số plane màu (Thuong là 1)
    unsigned short bitCount;   // So bit/ pixel 1 - 4 - 8
    unsigned long compression;
    unsigned long sizeImage;
    long x_pixel_per_meter;
    long y_pixel_per_meter;
    unsigned long colorUsed; // So mau su dung
    unsigned long importantColor;
};

class BMP
{
private:
    BMP_Header header;
    BMP_Info header_info;
    vector<char> data;   // Gia tri mau 1 pixel / 1 phan hinh anh
    vector<int> palette; // Tap hop cac mau sac khac nhau hinh anh dung

public:
    void readBMPFile(string filename);
    void writePalette(string filename = "palette.txt");
    void writePixelArray(string filename = "pixel.txt");
    bool isHavingPalette();
    int getWidth();
    int getHeight();
    short getBitType();
};

void BMP::readBMPFile(string filename)
{
    ifstream fin(filename, ios::binary);

    if (!fin.is_open())
    {
        cout << "File " << filename << " could not be opened!\n";
        return;
    }

    // Kiem tra ma nhan dang xem co phai la file bitmap khong?
    fin.read((char *)&header, sizeof(header));

    if (header.type[0] != 'B' || header.type[1] != 'M')
    {
        cout << "This is not a bitmap file!\n";
        return;
    }
    fin.read((char *)(&header_info), sizeof(header_info));
    int paletteSize;

    // Check if there's a palette
    if (isHavingPalette())
    {
        paletteSize = (1 << header_info.bitCount) * 4; // Each entry is 4 bytes (RGBA)
        palette.resize(paletteSize);
        fin.read((char *)(palette.data()), paletteSize);
    }

    // Read pixel data
    fin.seekg(header.offset, fin.beg); // offset la vi tri bat dau cua hinh anh
    // if(isHavingPalette()) data.resize(header_info.width * header_info.height * 3);
    data.resize(header_info.width * header_info.height * (header_info.bitCount / 8)); // 24-bit BMP file duoc bieu dien bang 3 bytes
    fin.read((char *)(data.data()), data.size());

    fin.close();
}

void BMP::writePalette(string filename)
{
    ofstream fout(filename);

    if (!fout.is_open())
    {
        cout << "File " << filename << " could not be opened!\n";
        return;
    }

    fout << palette.size() / 4 << endl;

    for (int i = 0; i < palette.size(); i += 4)
    {
        unsigned char red = palette[i];
        unsigned char green = palette[i + 1];
        unsigned char blue = palette[i + 2];
        unsigned char alpha = palette[i + 3]; // unused byte

        fout << hex << (int)(red) << " "
             << hex << (int)(green) << " "
             << hex << (int)(blue) << " "
             << hex << (int)(alpha) << endl;
    }

    fout.close();
}

void BMP::writePixelArray(string filename)
{
    ofstream fout(filename);

    if (!fout.is_open())
    {
        cout << "File " << filename << " could not be opened!\n";
        return;
    }

    fout << header_info.height << " " << header_info.width << endl;
    int bytesPerPixel = header_info.bitCount / 8;
    for (int y = 0; y < header_info.height; y++)
    {
        for (int x = 0; x < header_info.width; x++)
        {
            int pos = (y * header_info.width + x) * bytesPerPixel;

            // In BMP file, RGB values are stored backward: BGR.
            // +2, +1, +0 offset -> access blue, green, red component of the pixel.
            // fout << hex << (int)(data[pos + 2]) << " "
            //      << hex << (int)(data[pos + 1]) << " "
            //      << hex << (int)(data[pos]) << " ";

            if (bytesPerPixel == 1)
            {
                if (!palette.empty())
                {
                    // Sử dụng bảng màu
                    int colorIndex = data[pos];
                    int palettePos = colorIndex * 4;
                    fout << hex << (int)palette[palettePos + 2] << " "
                         << hex << (int)palette[palettePos + 1] << " "
                         << hex << (int)palette[palettePos] << " ";
                }
                else
                {
                    // Không có bảng màu, giả định đen hoặc trắng
                    fout << hex << (int)data[pos] << " "
                         << hex << (int)data[pos] << " "
                         << hex << (int)data[pos] << " ";
                }
            }
            else if (bytesPerPixel == 3)
            {
                // Định dạng 24-bit (BGR)
                fout << hex << (int)data[pos + 2] << " "
                     << hex << (int)data[pos + 1] << " "
                     << hex << (int)data[pos] << " ";
            }
        }
        fout << endl;
    }

    fout.close();
}

bool BMP::isHavingPalette()
{
    return header_info.bitCount <= 8;
}

int BMP::getWidth()
{
    return header_info.width;
}

int BMP::getHeight()
{
    return header_info.height;
}

short BMP::getBitType()
{
    return header_info.bitCount;
}