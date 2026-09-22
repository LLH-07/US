#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include "playerInfo.h"
#include "game.h"

using namespace std;

void getPlayerInfo()
{
	ifstream fin;
	fin.open("PlayerInfo.bin", ios::binary);
	 

	string num = "";
	string temp = "";

	for (int i = 0; i < 10; i++)
	{
		fin.read((char*)&pp[i].Name, 30);
		fin.read((char*)&pp[i].ID, 30);
		fin.read((char*)&pp[i].score, sizeof(pp[i].score));
	}

	fin.close();
}

void savePlayerInfo(Player Pl)
{
	ofstream finout;
	finout.open("PlayerInfo.bin", ios::binary);


	if (pp[9].score < Pl.score)
	{
		strcpy(pp[9].Name,Pl.Name);
		strcpy(pp[9].ID, Pl.ID);
		pp[9].score = Pl.score;
	}

	sortPlayer();

	for (int i = 0; i < 10; i++)
	{
		finout.write((char*)&pp[i].Name, 30);
		finout.write((char*)&pp[i].ID, 30);
		finout.write((char*)&pp[i].score, sizeof(pp[i].score));

	}

	finout.close();
}
