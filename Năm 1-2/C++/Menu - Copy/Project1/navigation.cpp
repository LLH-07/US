#include <iostream>
#include <windows.h>
#include <fstream>
#include <cstring>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include "console.h"
#include "move.h"
#include "navigation.h"
#include "menuBoard.h"
#include "game.h"
#include "Background.h"

using namespace std;

void navigateConsole(string isAt)
{
	// Main menu
	if (isAt == "start")
	{
		string isAtMode = "easy";
		modeMenu(isAtMode);
	}

	else if (isAt == "tutorial")
		Tutorial();

	else if (isAt == "ranking")
		RankingTop3();

	else if (isAt == "exit")
	{
		exitGameConsole();
		Exit();
	}

	// Back to main menu
	else if (isAt == "back")
		menuBoard();

	// After choosing mode
	else if (isAt == "easy" || isAt == "normal" || isAt == "hard")
	{
		Player Pl;
		system("cls");
		smallFrame();
		nameBackground();
		// Player's information

		GotoXY(55, 15);
		cout << "Please enter your name (max 30 characters): ";
		cin.getline(Pl.Name,30);
		if (strlen(Pl.Name) == 0)
		{
			strcpy(Pl.Name, "UNKNOWN");
			GotoXY(99, 15);
			cout << Pl.Name;
		}


		GotoXY(55, 17);
		cout << "Please enter your ID (max 30 characters):  ";
		cin.getline(Pl.ID, 30);
		if (strlen(Pl.ID) == 0)
		{
			strcpy(Pl.ID, "UNKNOWN");
			GotoXY(97, 17);
			cout << Pl.ID;
		}

		Pl.score = 0;

		if(isAt == "easy")
			startGame(1, Pl);

		else if (isAt == "normal")
			startGame(2, Pl);

		else if (isAt == "hard")
			startGame(3, Pl);
	}

}

void stage1finished()
{
	system("cls");
	Sleep(300);
	ShowCur(0);
	string s1 = "Congratulation you have finished the first stage!\n";
	string s2 = "It's easy, right?\n";
	string s3 = "The next stage will be harder, and it will be more interesting!\n";
	string s4 = "We hope you can pass it!\n";
	string s5 = "GOOD LUCK!\n";

	int y = 19;

	GotoXY((172 / 2) - (s1.size() / 2), y);
	for (int i = 0; i < s1.size(); i++)
	{
		Sleep(50);
		cout << s1[i];
	}
	Sleep(200);

	GotoXY((172 / 2) - (s2.size() / 2), y + 1);
	for (int i = 0; i < s2.size(); i++)
	{
		Sleep(50);
		cout << s2[i];
	}

	Sleep(500);
	system("cls");
	GotoXY((172 / 2) - (s3.size() / 2), y);
	for (int i = 0; i < s3.size(); i++)
	{
		Sleep(50);
		cout << s3[i];
	}

	Sleep(200);

	GotoXY((172 / 2) - (s4.size() / 2), y + 1);
	for (int i = 0; i < s4.size(); i++)
	{
		Sleep(50);
		cout << s4[i];
	}

	Sleep(500);
	system("cls");
	GotoXY((172 / 2) - (s5.size() / 2), y);
	for (int i = 0; i < s5.size(); i++)
	{
		Sleep(50);
		cout << s5[i];
	}
	Sleep(1000);
}

void stage2finished()
{
	system("cls");
	Sleep(300);
	ShowCur(0);
	string s1 = "WOW! You are not the average kind!\n";
	string s2 = "ARE YOU?\n";
	string s3 = "The next stage will be the last one, and it's the hardest stage of this mode!\n";
	string s4 = "ARE YOU READY?\n";
	string s5 = "OK! LET'S GO!!!\n";

	int y = 19;

	GotoXY((172 / 2) - (s1.size() / 2), y);
	for (int i = 0; i < s1.size(); i++)
	{
		Sleep(50);
		cout << s1[i];
	}
	Sleep(200);

	GotoXY((172 / 2) - (s2.size() / 2), y + 1);
	for (int i = 0; i < s2.size(); i++)
	{
		Sleep(50);
		cout << s2[i];
	}

	Sleep(600);
	system("cls");
	GotoXY((172 / 2) - (s3.size() / 2), y);
	for (int i = 0; i < s3.size(); i++)
	{
		Sleep(50);
		cout << s3[i];
	}

	Sleep(500);
	system("cls");

	GotoXY((172 / 2) - (s4.size() / 2), y);
	for (int i = 0; i < s4.size(); i++)
	{
		Sleep(100);
		cout << s4[i];
	}

	Sleep(1000);
	system("cls");
	GotoXY((172 / 2) - (s5.size() / 2), y);
	for (int i = 0; i < s5.size(); i++)
	{
		Sleep(30);
		cout << s5[i];
	}
	Sleep(1000);
}