#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#include "console.h"
#include "menuBoard.h"
#include "playerInfo.h"

using namespace std;

struct highlight {
    int x, y;
};

struct GameData {
    int size;
    int numCha;
    char** CharBoard;
    int deletedCell;
};

// set surrounded cells
void SetSurroundCell(GameData& Pikachu);

//Get char into Pikachu.CharBoard
void GetCharIntoBoard(GameData& Pikachu, int mode);

void printBoard(GameData& Pikachu, Player p);
void printBoard1(GameData& Pikachu, highlight highlight1, Player p);
void printBoard2(GameData& Pikachu, highlight highlight1, highlight highlight2, Player p);

//delete 2 highlighted cells
void deleteUp(GameData& Pikachu, highlight C);
void deleteDown(GameData& Pikachu, highlight C);
void deleteRight(GameData& Pikachu, highlight C);
void deleteLeft(GameData& Pikachu, highlight C);
void deleteChar(GameData& Pikachu, highlight C);

bool checkUp(GameData& Pikachu, highlight C);
bool checkDown(GameData& Pikachu, highlight C);
bool checkRight(GameData& Pikachu, highlight C);
bool checkLeft(GameData& Pikachu, highlight C);

void deleteCell(GameData& Pikachu, highlight c);

void printHelp(GameData & Pikachu, int helpChoice);
int displayHelp(GameData & Pikachu);

// Checking I Matching
bool check_I_horizontal(highlight c1, highlight c2);
bool check_I_vertical(highlight c1, highlight c2);
bool checkI(highlight c1, highlight c2);

// Checking L matching
// check horizontal then vertical: tu ngang den doc
bool check_H_V(highlight c1, highlight c2);
// check vertical the horizontal
bool check_V_H(highlight c1, highlight c2);
bool checkL(highlight c1, highlight c2)
;
// Checking U matching
bool rightU(GameData & Pikachu, highlight c1, highlight c2);
bool leftU(GameData& Pikachu, highlight c1, highlight c2);
bool upU(GameData& Pikachu, highlight c1, highlight c2);
bool downU(GameData & Pikachu, highlight c1, highlight c2);
bool checkU(GameData & Pikachu, highlight c1, highlight c2);

// Checking Z matching
bool rightZ(highlight c1, highlight c2);
bool leftZ(highlight c1, highlight c2);
bool downZ(highlight c1, highlight c2);
bool checkZ(highlight c1, highlight c2);

void getHelpFunction(GameData& Pikachu, Player& p);

highlight Select1(GameData& Pikachu, highlight select1, Player& p);

highlight Select2(GameData& Pikachu, highlight select1, highlight select2, Player& p);

void highlightHelpCell(GameData& Pikachu, Player& p);

void sortPlayer();

void playGame(GameData& Pikachu, highlight c1, highlight c2, Player& p);

void startGame(int mode, Player& player);
