#pragma once
#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>

using namespace std;

void press(string& isAt, char ch);
void deleteStart();
void deleteTutorial();
void deleteRanking();
void deleteExit();
void deleteEasy();
void deleteNormal();
void deleteHard();
void deleteBack();
void moveToStart();
void moveToTutorial();
void moveToRanking();
void moveToExit();
void moveToEasy();
void moveToNormal();
void moveToHard();
void moveToBack();
void toMove(string isAt);