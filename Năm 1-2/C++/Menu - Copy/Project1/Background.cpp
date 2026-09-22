#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include <MMsystem.h>
#include "console.h"
#include "move.h"
#include "navigation.h"
#include "menuBoard.h"
#include "game.h"
#include "Background.h"

using namespace std;

void menuBackground()
{
    static wstring m1  = L"                                /000000\\                          \n";
    static wstring m2  = L"                   //////ooooo/0        0\\oooo\\\\\\              \n";
    static wstring m3  = L"               ////000000o....           ....oo000000\\\\          \n";
    static wstring m25 = L"            ///000~                                   o00oo\\      \n";
    static wstring m4  = L"         ////000    00     ~~~~          ~~~~        00   :^oo\\  \n";
    static wstring m5  = L"        ///000:  00000  .000000.    :0:  .000000.  ?0000  .00|| \n";
    static wstring m6  = L"      //oo000    0000    000000    .^o^.   000000         000oo \n";
    static wstring m7  = L"       0000      0000  ....      :/00000\\:     ....     00000///\n";
    static wstring m8  = L"|0000000000:     00  .000000.    |0000000|   .000000:   0000        \n";
    static wstring m9 =  L"00---------00000000  .000000.    |       |   .000000:  000          \n";
    static wstring m10 = L"00             0000    ~00~      \\       /      00     00          \n";
    static wstring m11 = L"00               000               ^~~~^             00000         \n";
    static wstring m12 = L"000000:           0000                  00!. ..   0000000          \n";
    static wstring m13 = L"-0000000000:      0000  ||      ^0000000^.00     000000            \n";
    static wstring m14 = L"      00000000:  0000  .00    .000000~. :00.  ^.    000            \n";
    static wstring m15 = L"     000000|  000000 ^ .00                 !Y.      0000           \n";
    static wstring m16 = L"   00000    0000000  .00    ~?5:                      00           \n";
    static wstring m17 = L"   .0000!!!  :.~G&G  .00    00^                       00           \n";
    static wstring m18 = L"        ?00000:  PB    00.   00^                      00           \n";
    static wstring m19 = L"           ^~~000000~  :~0000?^.                    0000           \n";
    static wstring m20 = L"             000000 ? !....                        0000            \n";
    static wstring m21 = L"                   00000..      ..0000000..    000000              \n";
    static wstring m22 = L"                      !000~ ^~~0000000000000: 00000                \n";
    static wstring m23 = L"                     :00000000000        00000000000               \n";
    static wstring m24 = L"                        0000000            0000000.                \n";

    PrintTextColor (m1, 06);
    PrintTextColor (m2, 06);
    PrintTextColor (m3, 06);
    PrintTextColor (m25, 06);
    PrintTextColor (m4, 06);
    PrintTextColor (m5, 06);
    PrintTextColor (m6, 06);
    PrintTextColor (m7, 06);
    PrintTextColor (m8, 06);
    PrintTextColor (m9, 06);
    PrintTextColor(m10, 06);
    PrintTextColor(m11, 06);
    PrintTextColor(m12, 06);
    PrintTextColor(m13, 06);
    PrintTextColor(m14, 06);
    PrintTextColor(m15, 06);
    PrintTextColor(m16, 06);
    PrintTextColor(m17, 06);
    PrintTextColor(m18, 06);
    PrintTextColor(m19, 06);
    PrintTextColor(m20, 06);
    PrintTextColor(m21, 06);
    PrintTextColor(m22, 06);
    PrintTextColor(m23, 06);
    PrintTextColor(m24, 06);

}

void modeBackground()
{
    static wstring po1  = L"                    ████████████                   \n";
    static wstring po5  = L"              ████████████████████████             \n";
    static wstring po6  = L"           ██████████████████████████████         \n";
    static wstring po7  = L"         ██████████████████████████████████        \n";
    static wstring po8  = L"       ██████████████████████████████████████      \n";
    static wstring po9  = L"     ██████████████████████████████████████████    \n";
    static wstring po10 = L"    ████████████████████████████████████████████   \n";
    static wstring po11 = L"   ██████████████████████████████████████████████  \n";
    static wstring po12 = L"  ██████████████████            ██████████████████ \n";
    static wstring po13 = L" █████████████████                █████████████████\n";
    static wstring po14 = L" █████████████████   █████████    █████████████████\n";
    static wstring po15 = L" █████████████████    ███████     █████████████████\n";
    static wstring po16 = L" ████          █████            █████          ████\n";
    static wstring po17 = L"  ███           ████████    ████████           ███ \n";
    static wstring po18 = L"   ███            ██████████████              ███  \n";
    static wstring po19 = L"    ███                                     ███    \n";
    static wstring po20 = L"     ███                                   ███     \n";
    static wstring po21 = L"       ███                               ███       \n";
    static wstring po22 = L"         ████                         ████         \n";
    static wstring po23 = L"            ████                    ████           \n";
    static wstring po24 = L"               ██████          ██████              \n";
    static wstring po25 = L"                    ████████████                  \n";


    int i = 5;
    GotoXY(110, i);
    PrintTextColor(po1, 0);
    GotoXY(110, i + 1);
    PrintTextColor(po5, 0);
    GotoXY(110, i + 2);
    PrintTextColor(po6, 0);
    GotoXY(110, i + 3);
    PrintTextColor(po7, 0);
    GotoXY(110, i + 4);
    PrintTextColor(po8, 0);
    GotoXY(110, i + 5);
    PrintTextColor(po9, 0);
    GotoXY(110, i + 6);
    PrintTextColor(po10, 0);
    GotoXY(110, i + 7);
    PrintTextColor(po11, 0);
    GotoXY(110, i + 8);
    PrintTextColor(po12, 0);
    GotoXY(110, i + 9);
    PrintTextColor(po13, 0);
    GotoXY(110, i + 10);
    PrintTextColor(po14, 0);
    GotoXY(110, i + 11);
    PrintTextColor(po15, 0);
    GotoXY(110, i + 12);
    PrintTextColor(po16, 0);
    GotoXY(110, i + 13);
    PrintTextColor(po17, 0);
    GotoXY(110, i + 14);
    PrintTextColor(po18, 0);
    GotoXY(110, i + 15);
    PrintTextColor(po19, 0);
    GotoXY(110, i + 16);
    PrintTextColor(po20, 0);
    GotoXY(110, i + 17);
    PrintTextColor(po21, 0);
    GotoXY(110, i + 18);
    PrintTextColor(po22, 0);
    GotoXY(110, i + 19);
    PrintTextColor(po23, 0);
    GotoXY(110, i + 20);
    PrintTextColor(po24, 0);
    GotoXY(110, i + 21);
    PrintTextColor(po25, 0);
    GotoXY(110, i + 22);
    //PrintTextColor(po26, 0);
    //GotoXY(110, i + 26);
}

void nameBackground()
{
    static wstring n1 = L"                  _					                                        \n";
    static wstring n2 = L"                _(_)_                            wWWWw     _	                \n";
    static wstring n3 = L"    @@@@       (_)@(_)   vVVVv       _     @@@@  (___)   _(_)_               \n";
    static wstring n4 = L"   @@()@@ wWWWw  (_)\\    (___)     _(_)_  @@()@@   Y    (_)@(_)             \n";
    static wstring n5 = L"    @@@@  (___)     |/     Y      (_)@(_)  @@@@   \\|/     (_)\\             \n";
    static wstring n6 = L"     /      Y      \\|     \\|/     / (_)   \\|       |/         |           \n";
    static wstring n7 = L"  \\ |       \\|/      | /  \\ | /  \\|/       |/      \\|         \\|/       \n";
    static wstring n8 = L"  \\|//     \\|///  \\\\|//  \\\\|/// \\|///      \\\\|//    \\\\|// \\\\|// \n";
    static wstring n9 = L"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^           \n";
    
    int j = 30;
    GotoXY(60, j);
    PrintTextColor(n1, 0);
    GotoXY(60, j + 1);
    PrintTextColor(n2, 0);
    GotoXY(60, j + 2);
    PrintTextColor(n3, 0);
    GotoXY(60, j + 3);
    PrintTextColor(n4, 0);
    GotoXY(60, j + 4);
    PrintTextColor(n5, 0);
    GotoXY(60, j + 5);
    PrintTextColor(n6, 0);
    GotoXY(60, j + 6);
    PrintTextColor(n7, 0);
    GotoXY(60, j + 7);
    PrintTextColor(n8, 0);
    GotoXY(60, j + 8);
    PrintTextColor(n9, 0);
    GotoXY(60, j + 9);
}
