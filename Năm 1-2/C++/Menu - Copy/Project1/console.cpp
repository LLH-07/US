#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include "console.h"

#define VERTICAL_LINE char(186)    // ║
#define HORIZONTAL_LINE char(205)  // ═
#define RIGHT_TOP char(187)        // ╗
#define RIGHT_BOTTOM char(188)     // ╝
#define LEFT_TOP char(201)         // ╔
#define LEFT_BOTTOM char(200)      // ╚
#define T_SHAPE_UP char(202)       // ╩
#define T_SHAPE_DOWN char(203)     // ╦
#define T_SHAPE_RIGHT char(204)    // ╠
#define T_SHAPE_LEFT char(185)     // ╣
#define CROSS char(206)            // ╬

using namespace std;

// Create the console window 70
void CreateConsoleWindow(int width, int height) {
	//system("color 70");
	SetConsoleTitleA("PIKACHU GAME");
	SetWindowSize(width, height);
	ShowScrollbar(0);
	FixConsoleWindow();
}

void SetColor(int backgound_color, int text_color)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    int color_code = backgound_color * 16 + text_color;
    SetConsoleTextAttribute(hStdout, color_code);
}

void GotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//Fix the size of the console window
static void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

//Set the size of the console window
static void SetWindowSize(int width, int height) {
	HWND consoleWindow = GetConsoleWindow();
	RECT r;
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetWindowRect(consoleWindow, &r);
	MoveWindow(consoleWindow, 0, 0, width, height, TRUE);
}

//Hide scrollbar of the console window
static void ShowScrollbar(bool Show) {
	HWND hWnd = GetConsoleWindow();
	ShowScrollBar(hWnd, SB_BOTH, Show);
}

//Show/Hide Cursor on the console window
void ShowCur(bool CursorVisibility) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConCurInf;
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = CursorVisibility;
	SetConsoleCursorInfo(handle, &ConCurInf);
}

//Change color of Text since this function is called
void TextColor(int color)
{
	static int __BACKGROUND = 7;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(h, &csbiInfo);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		color + (__BACKGROUND << 4));
}

//Print color text USAGE: PrintTextColor(L"example", <color>);
void PrintTextColor(const wstring& s, const int& color) {
	TextColor(color);
	int OldMode = _setmode(_fileno(stdout), _O_TEXT);
	wcout << s;
	int CurrentMode = _setmode(_fileno(stdout), OldMode);
	TextColor(0);
}

//Print color text USAGE: PrintTextColor("example", <color>);
void PrintTextColor(const string& s, const int& color) {
	TextColor(color);
	int OldMode = _setmode(_fileno(stdout), _O_TEXT);
	cout << s;
	int CurrentMode = _setmode(_fileno(stdout), OldMode);
	TextColor(0);
}

void PrintTextColor_Char(const char& s, const int& color) {
	TextColor(color);
	cout << s;
	TextColor(0);
}

// Clear Console
void clearConsole()
{
	HANDLE hOut;
	COORD Position;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}

//Close Console
void Exit() {
	HWND hwnd = GetConsoleWindow();
	SendMessage(hwnd, WM_CLOSE, 0, 0);
}

void drawMenuCell(string s, int pos_y)
{
	// Top part
	GotoXY(70, pos_y);
	cout << LEFT_TOP;
	for (int i = 0; i < 29; i++)
		cout << HORIZONTAL_LINE;
	cout << RIGHT_TOP;

	// Middle part
	// 1st
	GotoXY(70, pos_y + 1);
	cout << VERTICAL_LINE;
	for (int i = 0; i < 29; i++)
		cout << ' ';
	cout << VERTICAL_LINE;
	// 2nd
	GotoXY(70, pos_y + 2);
	cout << VERTICAL_LINE;
	for (int i = 0; i < 14 - s.size() / 2; i++)
		cout << ' ';

	cout << s;
	
	for (int i = 0; i < 14 - s.size() / 2; i++)
		cout << ' ';

	if (s.size() % 2 == 0)
		cout << ' ';

	cout << VERTICAL_LINE;
	// 3rd
	GotoXY(70, pos_y + 3);
	cout << VERTICAL_LINE;
	for (int i = 0; i < 29; i++)
		cout << ' ';
	cout << VERTICAL_LINE;

	// Bottom part
	GotoXY(70, pos_y + 4);
	cout << LEFT_BOTTOM;
	for (int i = 0; i < 29; i++)
		cout << HORIZONTAL_LINE;
	cout << RIGHT_BOTTOM;
}

void exitGameConsole()	
{
	system("cls");

	wstring ex1 = L"███████╗███████╗███████╗    ██╗   ██╗ ██████╗ ██╗   ██╗     █████╗  ██████╗  █████╗ ██╗███╗   ██╗██╗\n";
	wstring ex2 = L"██╔════╝██╔════╝██╔════╝    ╚██╗ ██╔╝██╔═══██╗██║   ██║    ██╔══██╗██╔════╝ ██╔══██╗██║████╗  ██║██║\n";
	wstring ex3 = L"███████╗█████╗  █████╗       ╚████╔╝ ██║   ██║██║   ██║    ███████║██║  ███╗███████║██║██╔██╗ ██║██║\n";
	wstring ex4 = L"╚════██║██╔══╝  ██╔══╝        ╚██╔╝  ██║   ██║██║   ██║    ██╔══██║██║   ██║██╔══██║██║██║╚██╗██║╚═╝\n";
	wstring ex5 = L"███████║███████╗███████╗       ██║   ╚██████╔╝╚██████╔╝    ██║  ██║╚██████╔╝██║  ██║██║██║ ╚████║██╗\n";
	wstring ex6 = L"╚══════╝╚══════╝╚══════╝       ╚═╝    ╚═════╝  ╚═════╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝╚═╝\n";

	GotoXY((170 / 2) - (ex1.size() / 2), (40 / 2 - 3));
	PrintTextColor(ex1, 4);

	GotoXY((170 / 2) - (ex2.size() / 2), (40 / 2 - 2));
	PrintTextColor(ex2, 6);

	GotoXY((170 / 2) - (ex3.size() / 2), (40 / 2 - 1));
	PrintTextColor(ex3, 2);

	GotoXY((170 / 2) - (ex4.size() / 2), (40 / 2 - 0));
	PrintTextColor(ex4, 1);

	GotoXY((170 / 2) - (ex5.size() / 2), (40 / 2 + 1));
	PrintTextColor(ex5, 5);

	GotoXY((170 / 2) - (ex6.size() / 2), (40 / 2 + 2));
	PrintTextColor(ex6, 3);

	Sleep(2000);
	Exit();

}

void MenuSound()
{
	return;
	PlaySound(TEXT("MenuSound.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
}
void GameSound()
{
	return;
	PlaySound(TEXT("GameSound.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
}
void endSound()
{
	return;
	PlaySound(TEXT("CongratulationsSound.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
}
