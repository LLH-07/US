#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include <mmsystem.h>
#include "console.h"
#include "move.h"
#include "navigation.h"
#include "menuBoard.h"
#include "game.h"
#include "Background.h"
#include "playerInfo.h"

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

#define ENTER_KEY 13

#define WIDTH 1280
#define HEIGHT 720

using namespace std;

Player pp[15];

// Main menu
void titleGame()
{
	string h1 = "  (   (   (( ((   (  (\n";
	string h2 = " )\\: )\\ (\\()))\\  )\\ )\\n";
	string h3 = "((_)((_))(_)(_)))(_)(_)\n";
	string h4 = "|  \\/  | __| \\| | | | |\n";
	string h5 = "| |\\/| | _|| .  | |_| |\n";
	string h6 = "|_|  |_|___|_|\\_|\\___/ \n";


	GotoXY((172 / 2) - (h1.size() / 2), (17 / 2 - 3));
	PrintTextColor(h1, 0);

	GotoXY((172 / 2) - (h2.size() / 2), (17 / 2 - 2));
	PrintTextColor(h2, 0);

	GotoXY((172 / 2) - (h3.size() / 2), (17 / 2 - 1));
	PrintTextColor(h3, 0);

	GotoXY((172 / 2) - (h4.size() / 2), (17 / 2 - 0));
	PrintTextColor(h4, 0);

	GotoXY((172 / 2) - (h5.size() / 2), (17 / 2 + 1));
	PrintTextColor(h5, 0);

	GotoXY((172 / 2) - (h6.size() / 2), (17 / 2 + 2));
	PrintTextColor(h6, 0);
}

void MenuOption()
{
	// Set menu board
	GotoXY(66, 17);
	wcout << '>';
	GotoXY(103, 17);
	wcout << '<';
	drawMenuCell("START", 15);
	drawMenuCell("TUTORIAL", 20);
	drawMenuCell("RANKING", 25);
	drawMenuCell("EXIT", 30);
}

void menuBoard()
{
	system("cls");
	ShowCur(0);

	//Player Pl = { "UNKNOWN", "UNKNOWN", 0 };

	// Set window ( couldn't be resized )
	CreateConsoleWindow(WIDTH, HEIGHT);
	titleGame();
	menuBackground();
	//savePlayerInfo(Pl);
	getPlayerInfo();

	// Set menu board
	MenuOption();

	string isAt = "start";
	toMove(isAt);
}

void backToMenu()
{

	string temp = "PRESS ENTER TO RETURN!";
	GotoXY((172 / 2) - (temp.size() / 2), 35);
	PrintTextColor(temp, 0);

	GotoXY(66, 38);
	cout << '>';
	GotoXY(103, 38);
	cout << '<';
	drawMenuCell("BACK", 36);
	char ch = _getch();

	while (ch != ENTER_KEY)
		ch = _getch();

	menuBoard();
}


// Tutorial
void tutorialTitle()
{
	wstring t1 = L"████████╗██╗   ██╗████████╗ ██████╗ ██████╗ ██╗ █████╗ ██╗     \n";
	wstring t2 = L"╚══██╔══╝██║   ██║╚══██╔══╝██╔═══██╗██╔══██╗██║██╔══██╗██║     \n";
	wstring t3 = L"   ██║   ██║   ██║   ██║   ██║   ██║██████╔╝██║███████║██║     \n";
	wstring t4 = L"   ██║   ██║   ██║   ██║   ██║   ██║██╔══██╗██║██╔══██║██║     \n";
	wstring t5 = L"   ██║   ╚██████╔╝   ██║   ╚██████╔╝██║  ██║██║██║  ██║███████╗\n";
	wstring t6 = L"   ╚═╝    ╚═════╝    ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝\n";

	GotoXY((172 / 2) - (t1.size() / 2), (17 / 2 - 3));
	PrintTextColor(t1, 0);

	GotoXY((172 / 2) - (t2.size() / 2), (17 / 2 - 2));
	PrintTextColor(t2, 0);

	GotoXY((172 / 2) - (t3.size() / 2), (17 / 2 - 1));
	PrintTextColor(t3, 0);

	GotoXY((172 / 2) - (t4.size() / 2), (17 / 2 - 0));
	PrintTextColor(t4, 0);

	GotoXY((172 / 2) - (t5.size() / 2), (17 / 2 + 1));
	PrintTextColor(t5, 0);

	GotoXY((172 / 2) - (t6.size() / 2), (17 / 2 + 2));
	PrintTextColor(t6, 0);
}

void Tutorial()
{
	system("cls");
	tutorialTitle();
	//backTutorial();

	string s1 = "PIKACHU is a classic game that was first released in Japan.\n";
	string s2 = "Phan Vu Gia Han and Ly Lien Hoa developed this Pikachu Puzzle Game based on that.\n\n";
	string s3 = "Game includes a board of multiple cells, each of which presents a particular firgure.\n";
	string s4 = "The players's missions are finding and matching a pair of cells that contains the same firgure\n";
	string s5 = "with the principle that paths connecting two matching firgures are less than 3.\n";
	string s6 = "The game ends when all matching pairs are found in a certain time.\n\n";
	string s7 = "Thera are 3 modes for players to choose : Easy, Normal, and Hard.\n";
	string s8 = "Each mode has a different number of cells, firgures and stages to make players feel more interested \n";
	string s9 = "Players will use W,A,S,D keys to move between the cells and ENTER key to select the cells.\n";
	string s10 = "Futhermore, players also can press 'H' key to have some extra functions for helping you complete the game.\n ";
	string s11 = "However, your score will be deducted by any score depending on each help.\n ";
	string s12 = "ENJOY THE GAME!";

	TextColor(1);
	int y = 15;
	GotoXY(172 / 2 - s1.size() / 2, y);
	cout << s1;

	GotoXY(172 / 2 - s2.size() / 2, y + 1);
	cout << s2;

	GotoXY(172 / 2 - s3.size() / 2, y + 3);
	cout << s3;

	GotoXY(172 / 2 - s4.size() / 2, y + 4);
	cout << s4;

	GotoXY(172 / 2 - s5.size() / 2, y + 5);
	cout << s5;

	GotoXY(172 / 2 - s6.size() / 2, y + 6);
	cout << s6;

	GotoXY(172 / 2 - s7.size() / 2, y + 8);
	cout << s7;

	GotoXY(172 / 2 - s8.size() / 2, y + 9);
	cout << s8;

	GotoXY(172 / 2 - s9.size() / 2, y + 11);
	cout << s9;

	GotoXY(172 / 2 - s10.size() / 2, y + 12);
	cout << s10;

	GotoXY(172 / 2 - s11.size() / 2, y + 13);
	cout << s11;

	GotoXY(172 / 2 - s12.size() / 2, y + 16);
	cout << s12;


	backToMenu();

	char enter = _getch();
	while (enter != ENTER_KEY)
		enter = _getch();

}

// Ranking
void rankingTitle()
{

	system("cls");

	wstring r1 = L"██████╗  █████╗ ███╗   ██╗██╗  ██╗██╗███╗   ██╗ ██████╗ \n";
	wstring r2 = L"██╔══██╗██╔══██╗████╗  ██║██║ ██╔╝██║████╗  ██║██╔════╝ \n";
	wstring r3 = L"██████╔╝███████║██╔██╗ ██║█████╔╝ ██║██╔██╗ ██║██║  ███╗\n";
	wstring r4 = L"██╔══██╗██╔══██║██║╚██╗██║██╔═██╗ ██║██║╚██╗██║██║   ██║\n";
	wstring r5 = L"██║  ██║██║  ██║██║ ╚████║██║  ██╗██║██║ ╚████║╚██████╔╝\n";
	wstring r6 = L"╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n";

	GotoXY((172 / 2) - (r1.size() / 2), (17 / 2 - 3));
	PrintTextColor(r1, 0);

	GotoXY((172 / 2) - (r2.size() / 2), (17 / 2 - 2));
	PrintTextColor(r2, 0);

	GotoXY((172 / 2) - (r3.size() / 2), (17 / 2 - 1));
	PrintTextColor(r3, 0);

	GotoXY((172 / 2) - (r4.size() / 2), (17 / 2 - 0));
	PrintTextColor(r4, 0);

	GotoXY((172 / 2) - (r5.size() / 2), (17 / 2 + 1));
	PrintTextColor(r5, 0);

	GotoXY((172 / 2) - (r6.size() / 2), (17 / 2 + 2));
	PrintTextColor(r6, 0);
}

void drawFrame()
{

	// Head
	GotoXY(20, 13);
	PrintTextColor_Char(LEFT_TOP, 1);
	for (int i = 0; i < 128; i++)
		//cout << HORIZONTAL_LINE;
		PrintTextColor_Char(HORIZONTAL_LINE, 1);
	PrintTextColor_Char(RIGHT_TOP, 1);

	// Body
	for (int i = 0; i < 24; i++)
	{
		GotoXY(20, 14 + i);
		//cout << VERTICAL_LINE;
		PrintTextColor_Char(VERTICAL_LINE, 1);
		for (int j = 0; j < 128; j++)
			cout << ' ';

		//cout << VERTICAL_LINE;
		PrintTextColor_Char(VERTICAL_LINE, 1);
	}

	// Bottom
	GotoXY(20, 37);
	PrintTextColor_Char(LEFT_BOTTOM, 1);
	for (int i = 0; i < 128; i++)
		PrintTextColor_Char(HORIZONTAL_LINE, 1);

	PrintTextColor_Char(RIGHT_BOTTOM, 1);
}

void drawRanking()
{
	// DRAW TOP 3 BOARD
	int rate = 15;
	for (int i = 0; i < rate; i++)
	{
		for (int j = 21; j < 149; j++)
		{
			GotoXY(j, 34 - i);
			// 3rd
			if (i < rate / 3 && j >= 108 && j <= 143)
			{
				TextColor(2);
				wcout << char(219);
			}

			// 2nd
			if (i < (rate / 3) * 2 && j >= 26 && j <= 61)
			{
				TextColor(6);
				wcout << char(219);
			}

			// 1st
			if (i < rate && j >= 67 && j <= 102)
			{
				TextColor(4);
				wcout << char(219);
			}

			cout << ' ';
		}
	}

	// NAME OF TOP 3

	// 1st
	GotoXY(82, 35);
	cout << pp[0].Name;
	GotoXY(84, 18);
	cout << pp[0].score;

	// 2nd
	TextColor(6);
	GotoXY(41, 35);
	cout << pp[1].Name;
	GotoXY(43, 23);
	cout << pp[1].score;

	// 3rd
	TextColor(2);
	GotoXY(123, 35);
	cout << pp[2].Name;
	GotoXY(126, 28);
	cout << pp[2].score;;
}

void drawRankBoard()
{
	rankingTitle();
	drawFrame();

	// Vertical
	// Head

	for (int i = 0; i <= 70; i += 35)
	{
			GotoXY(50 + i, 13);
			PrintTextColor_Char(T_SHAPE_DOWN, 1);
	}

	// Body
	for (int i = 0; i <= 70; i += 35)
		for (int j = 0; j < 23; j++)
		{
			GotoXY(50 + i, 14 + j);
			PrintTextColor_Char(VERTICAL_LINE, 1);
		}

	// Bottom
	for (int i = 0; i <= 70; i += 35)
	{
		GotoXY(50 + i, 37);
		PrintTextColor_Char(T_SHAPE_UP, 1);
	}

	//redraw
	for (int i = 0; i <= 27; i++)
		PrintTextColor_Char(HORIZONTAL_LINE, 1);

	PrintTextColor_Char(RIGHT_BOTTOM, 1);


	// Horizontal
	GotoXY(20, 17);
	PrintTextColor_Char(T_SHAPE_RIGHT, 1);
	for (int i = 0; i < 128; i++)
	{
		if (i == 29 ||  i == 64 || i == 99)
			PrintTextColor_Char(CROSS, 1);
		else PrintTextColor_Char(HORIZONTAL_LINE, 1);
	}

	PrintTextColor_Char(T_SHAPE_LEFT, 1);

	//
	rankingTop10();
}

void rankingTop10()
{
	// HEADER
	GotoXY(35, 15);
	cout << "ID";
	GotoXY(66, 15);
	cout << "NAME";
	GotoXY(100, 15);
	cout << "SCORE";
	GotoXY(133, 15);
	cout << "TOP";


	// CONTENT
	int top = 1;

	for (int i = 0; i < 20; i++)
	{
		if (i % 2 == 0)
		{
			// ID
			GotoXY(32, 18 + i);
			cout << pp[i / 2].ID;


			// Name
			GotoXY(65, 18 + i);
			cout << pp[i / 2].Name;

			// Score
			GotoXY(101, 18 + i);
			cout << pp[i / 2].score;

			// Top
			GotoXY(134, 18 + i);
			if (i % 2 == 0 && top != 10) cout << top, top++;
			else if (i % 2 == 0 && top == 10)
			{
				GotoXY(133, 18 + i);
				cout << 1 << ' ' << 0;
			}
		}
	}
}

void RankingTop3()
{
	//Player PP[15];
	rankingTitle();
	drawFrame();
	drawRanking();
	
	string con1 = "Press (c) to continue seeing top 10 ranking\n";
	string con2 = "Press (r) to return\n";

	GotoXY((172 / 2) - (con1.size() / 2), (78 / 2 + 0));
	TextColor(0);
	cout << con1;
	GotoXY((172 / 2) - (con2.size() / 2), (78 / 2 + 1));
	cout << con2;

	char ch;
	ch = _getch();
	ch = tolower(ch);

	while (ch != 'c' && ch != 'r')
		ch = _getch();

	if (ch == 'c')
		rankingBoard();

	if (ch == 'r')
		menuBoard();

}

void rankingBoard()
{
	drawRankBoard();

	string con1 = "Press (r) to return\n";

	GotoXY((174 / 2) - (con1.size() / 2), 39);
	cout << con1;

	char ch;
	ch = _getch();
	ch = tolower(ch);

	while (ch != 'r')
		ch = _getch();

	RankingTop3();
}

//Mode
void modeTitle()
{
	system("cls");
	wstring m1 = L"███╗   ███╗ ██████╗ ██████╗ ███████╗\n";
	wstring m2 = L"████╗ ████║██╔═══██╗██╔══██╗██╔════╝\n";
	wstring m3 = L"██╔████╔██║██║   ██║██║  ██║█████╗  \n";
	wstring m4 = L"██║╚██╔╝██║██║   ██║██║  ██║██╔══╝  \n";
	wstring m5 = L"██║ ╚═╝ ██║╚██████╔╝██████╔╝███████╗\n";
	wstring m6 = L"╚═╝     ╚═╝ ╚═════╝ ╚═════╝ ╚══════╝\n";

	GotoXY((172 / 2) - (m1.size() / 2), (17 / 2 - 3));
	PrintTextColor(m1, 0);

	GotoXY((172 / 2) - (m2.size() / 2), (17 / 2 - 2));
	PrintTextColor(m2, 0);

	GotoXY((172 / 2) - (m3.size() / 2), (17 / 2 - 1));
	PrintTextColor(m3, 0);

	GotoXY((172 / 2) - (m4.size() / 2), (17 / 2 - 0));
	PrintTextColor(m4, 0);

	GotoXY((172 / 2) - (m5.size() / 2), (17 / 2 + 1));
	PrintTextColor(m5, 0);

	GotoXY((172 / 2) - (m6.size() / 2), (17 / 2 + 2));
	PrintTextColor(m6, 0);
}

void ModeOption()
{
	// Set mode board
	GotoXY(66, 17);
	cout << '>';
	GotoXY(103, 17);
	cout << '<';
	drawMenuCell("EASY", 15);
	drawMenuCell("NORMAL", 20);
	drawMenuCell("HARD", 25);
	drawMenuCell("BACK", 30);
}

void modeMenu(string isAtMode)
{
	modeTitle();
	modeBackground();
	ModeOption();
	toMove(isAtMode);
}

// After choosing mode
void smallFrame()
{
	system("cls");

	// Head line
	GotoXY(50, 13);

	cout << LEFT_TOP;

	for (int i = 0; i < 85; i++)
		cout << HORIZONTAL_LINE;

	cout << RIGHT_TOP;


	// Bottom  line
	GotoXY(50, 19);

	cout << LEFT_BOTTOM;

	for (int i = 0; i < 85; i++)
		cout << HORIZONTAL_LINE;

	cout << RIGHT_BOTTOM;

	cout << endl;
}

// After finishing game
void congratulationTitle()
{
	endSound();
	Sleep(500);
	system("color 70");
	system("cls");
	wstring c1 = L" ██████╗ ██████╗ ███╗   ██╗ ██████╗ ██████╗  █████╗ ████████╗██╗   ██╗██╗      █████╗ ████████╗██╗ ██████╗ ███╗   ██╗███████╗██╗\n";
	wstring c2 = L"██╔════╝██╔═══██╗████╗  ██║██╔════╝ ██╔══██╗██╔══██╗╚══██╔══╝██║   ██║██║     ██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝██║\n";
	wstring c3 = L"██║     ██║   ██║██╔██╗ ██║██║  ███╗██████╔╝███████║   ██║   ██║   ██║██║     ███████║   ██║   ██║██║   ██║██╔██╗ ██║███████╗██║\n";
	wstring c4 = L"██║     ██║   ██║██║╚██╗██║██║   ██║██╔══██╗██╔══██║   ██║   ██║   ██║██║     ██╔══██║   ██║   ██║██║   ██║██║╚██╗██║╚════██║╚═╝\n";
	wstring c5 = L"╚██████╗╚██████╔╝██║ ╚████║╚██████╔╝██║  ██║██║  ██║   ██║   ╚██████╔╝███████╗██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║███████║██╗\n";
	wstring c6 = L" ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚═╝\n";


	GotoXY((172 / 2) - (c1.size() / 2), (17 / 2 - 3));
	PrintTextColor(c1, 4);

	GotoXY((172 / 2) - (c2.size() / 2), (17 / 2 - 2));
	PrintTextColor(c2, 6);

	GotoXY((172 / 2) - (c3.size() / 2), (17 / 2 - 1));
	PrintTextColor(c3, 2);

	GotoXY((172 / 2) - (c4.size() / 2), (17 / 2 - 0));
	PrintTextColor(c4, 1);

	GotoXY((172 / 2) - (c5.size() / 2), (17 / 2 + 1));
	PrintTextColor(c5, 5);

	GotoXY((172 / 2) - (c6.size() / 2), (17 / 2 + 2));
	PrintTextColor(c6, 3);

	cout << endl;

	//string s = "You finished this mode successfully! We hope you enjoy this game!";
	string s1 = " ___                                                                       ___\n";
	string s2 = " (o o)                                                                     (o o)\n";
	string s3 = " (  V  ) You finished this mode successfully! We hope you enjoy this game! (  V  )\n";
	string s4 = "----m - m---------------------------------------------------------------------m - m---\n";
	GotoXY((172 / 2) - (s1.size() / 2), (32 / 2 - 2));
	cout << s1;
	GotoXY((172 / 2) - (s2.size() / 2), (32 / 2 - 1));
	cout << s2;
	GotoXY((172 / 2) - (s3.size() / 2), (32 / 2 + 0));
	cout << s3;
	GotoXY((172 / 2) - (s4.size() / 2), (32 / 2 + 1));
	cout << s4;

	string con1 = "Press (c) to continue\n";
	string con2 = "Press (e) to exit the game\n";

	GotoXY((172 / 2) - (con1.size() / 2), (72 / 2 + 0));
	cout << con1;
	GotoXY((172 / 2) - (con2.size() / 2), (72 / 2 + 1));
	cout << con2;

	char ch;
	ch = _getch();
	ch = tolower(ch);
	
	while (ch != 'c' && ch != 'e')
		ch = _getch();

	if (ch == 'c')
	{
		MenuSound();
		menuBoard();
	}

	if (ch == 'e')
		exitGameConsole();
}