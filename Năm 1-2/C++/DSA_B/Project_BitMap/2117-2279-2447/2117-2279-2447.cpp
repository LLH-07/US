#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

#pragma pack(push, 1)
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
#pragma pack(pop)

class BMP
{
private:
    BMP_Header header;
    BMP_Info header_info;
    int paletteSize;
    char* palette;
    char* data;

public:
    BMP() : palette(nullptr), data(nullptr) {}
    ~BMP()
    {
        delete[] palette;
        delete[] data;
    }

    void readBMPFile(string filename = "default.bmp");
    void writePalette(string filename = "palette.txt");
    void writePixelArray(string filename = "pixel.txt");
    bool isHavingPalette();
    int getWidth();
    int getHeight();
    short getBitType();
};

void BMP::readBMPFile(string filename)
{
    if (filename == "")
        filename = "default.bmp";

    ifstream fin(filename, ios::binary);

    if (!fin.is_open())
    {
        cout << "File " << filename << " could not be opened!\n";
        return;
    }


    fin.read((char*)&header, sizeof(header));

    // Kiem tra ma nhan dang xem co phai la file bitmap khong?
    if (header.type[0] != 'B' || header.type[1] != 'M')
    {
        cout << "This is not a bitmap file!\n";
        return;
    }
    fin.read((char*)(&header_info), sizeof(header_info));
    if (fin.fail()) {
        cerr << "Error reading BMP info.\n";
        return;
    }

    paletteSize = header.offset - sizeof(BMP_Header) - sizeof(BMP_Info);

    if (paletteSize > 0)
    {
        palette = new char[paletteSize];
        fin.read(palette, paletteSize);
    }

    else palette = nullptr;



    // Read pixel data
    fin.seekg(header.offset, ios::beg); // offset is the start position of the image data
    int sizeData = header_info.width * header_info.height * (header_info.bitCount / 8); // 3 bytes per pixel (RGB)
    data = new char[sizeData];
    fin.read(data, sizeData);

    if (header_info.bitCount == 24) {
        for (int i = 0; i < sizeData; i += 3) {
            swap(data[i], data[i + 2]);
        }
    }

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

    if (palette == nullptr) {
        cerr << "Palette data is not available.\n";
        return;
    }
    fout << paletteSize / 4 << endl;
    for (int i = 0; i < paletteSize; i += 4)
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

    for (int y = 0; y < header_info.height; y++)
    {
        for (int x = 0; x < header_info.width; x++)
        {
            int pos = (y * header_info.width + x) * (header_info.bitCount / 8);
            fout << hex << (int)data[pos + 2] << " "
                << hex << (int)data[pos + 1] << " "
                << hex << (int)data[pos] << " ";
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

int main()
{
    string filename = "";

    cout << "Insert filename: ";
    getline(cin, filename);

    BMP bmp;
    bmp.readBMPFile(filename);
    bmp.writePixelArray();
    cout << "Filename: " << filename << endl;
    cout << "Bit depth: " << bmp.getBitType() << endl;
    cout << "Width: " << bmp.getWidth() << endl;
    cout << "Height: " << bmp.getHeight() << endl;
    cout << "Color palette: ";

    if (bmp.isHavingPalette())
    {
        cout << "Yes\n";
        bmp.writePalette();
    }

    else
        cout << "No\n";

    return 0;
}