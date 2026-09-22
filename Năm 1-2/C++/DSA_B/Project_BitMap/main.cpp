#include "bmp.h"
#include <string>

using namespace std;

int main()
{
    string filename = "simple_image.bmp";

    //cout << "Insert filename: ";
    //getline(cin, filename);
    if (filename == "")
        filename = "default.bmp";
    
    BMP bmp;
    bmp.readBMPFile(filename);
    bmp.writePixelArray();
    cout << "Filename: " << filename << endl;
    cout << "Bit depth: " << bmp.getBitType() << endl;
    cout << "Width: " << bmp.getWidth() << endl;
    cout << "Height: " << bmp.getHeight() << endl;
    cout << "Color palette: ";

    if(!bmp.isHavingPalette())
        cout << "No\n";

    else
    {
        cout << "Yes\n";
        bmp.writePalette();
    }

    return 0;
}