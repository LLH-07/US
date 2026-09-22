#pragma once


using namespace std;

struct Player {
    char Name[31] = "UNKNOWN";
    char ID[31] = "UNKNOWN";
    int score = 0;
};

extern Player pp[15];

void getPlayerInfo();
void savePlayerInfo(Player Pl);
