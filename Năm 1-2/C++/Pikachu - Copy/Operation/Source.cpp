#include <iostream>
#include <cmath>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include<stdio.h>
#include "Console.h"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ESC_KEY 27
#define ENTER_KEY 13

using namespace std;


// For creating board
int inRow, inCol;
int width, length, board[50][100], copiedBoard[50][100];
int num_type_Pokemon[100];
int checkPokemon[50][100];

void SET_COLOR(int color)
{
	WORD wColor;
     

     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
          wColor = (csbi.wAttributes & 0xF0) + (color & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
     }
}

void setSizeOfBoard()
{
	system("cls");
	cout << "PLEASE ENTER YOUR BOARD SIZE\n";

	cout << "Size of row (MAX 8): ";
	cin >> inRow;
	if (inRow > 8 || inRow <= 0) 
		setSizeOfBoard();

	cout << "Size of column(MAX 8): ";
	cin >> inCol;
	if (inCol > 8 || inCol <= 0) 
		setSizeOfBoard();

}

void settingStartingBoard()
{

	width = inRow * 4 + 1;
	length = inCol * 8 + 1                                                                                                                                                                                                            ;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < length; j++)
		{
			if (i % 4 == 0) // draw horizontal lines
			{
				if (j % 8 == 0)
					board[i][j] = 32; // symbolizing for ' '

				else
					board[i][j] = 45; // symbolizing for '-'
			}

			else// draw vertical lines
			{
				if (j % 8 == 0)
					board[i][j] = 124; // symbolizing for '|'

				else
					board[i][j] = 32; // symbolizing for ' '
			}
		}
	}
}

void createPokemon()
{
	for (int i = 0; i < 100; i++)
		num_type_Pokemon[i] = 0;

	srand(time(NULL));
	int numPokemons = inRow * inCol;
	for (int i = 0; i < numPokemons / 2; i++)
		num_type_Pokemon[(rand() % 26) + 65] += 2;
}

void setPokemonIntoBoard()
{
	int counttype[100] = {};
	int row = 1, col = 1;
	for (int i = 2; i < width; i += 4)
	{
		for (int j = 4; j < length; j += 8)
		{
			int type;
			do
			{
				type = rand() % 26 + 65;
			} while (counttype[type] == num_type_Pokemon[type]);

			board[i][j] = type;
			checkPokemon[row][col] = type;
			col += 8;
			counttype[type]++;
		}
		row += 4;
	}
}

// void createStartingBoard()
// {
// 	system("cls");
// 	for (int i = 0; i < width; i++)
// 	{
// 		for (int j = 0; j < length; j++)
// 		{
// 			cout << char(board[i][j]);
// 		}
// 			cout << endl;
// 	}
// }

void copyBoard()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < length; j++)
			copiedBoard[i][j] = board[i][j];
}

// For
// source: https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key
int selected_x[5], selected_y[5], selected_cell[50][100] = {}, timeSelected = 0;
int move_x = 1, move_y = 1;
int cellCleared = inRow * inCol;
int line[50][100] = {};

// For checking
bool checkI()
{
	// Check horizontal line
	if (selected_x[1] == selected_x[2])
	{
		if(abs(selected_y[1] - selected_y[2]) == 8)
			return true;

		else
		{
			int smaller = selected_y[1];
			int big = selected_y[2];

			if(smaller > selected_y[2])
			{
				smaller = selected_y[2];
				big = selected_y[1];
			}

			for ( int j = smaller + 8; j < big; j+= 8)
			{
				if(checkPokemon[selected_x[1]][j] != -1)
					return false;
			}

			return true;
		}
	}

	// check vertical line
	else if (selected_y[1] == selected_y[2])
	{
		if(abs(selected_x[1] - selected_x[2]) == 4)
			return true;

		else
		{
			int smaller = selected_y[1];
			int big = selected_y[2];

			if(smaller > selected_y[2])
			{
				smaller = selected_y[2];
				big = selected_y[1];
			}

			for ( int i = smaller + 4; i < big; i += 4)
			{
				if(checkPokemon[i][selected_y[1]] != -1)
					return false;
			}

			return true;
		}
	}
	
}

bool checkL();
bool checkU();
bool checkZ();

bool isTheSame()
{
	int x1, y1, x2, y2;
	x1 = selected_x[1];
	y1 = selected_y[1];
	
	x2 = selected_x[2];
	y2 = selected_y[2];

	if( checkPokemon[x1][y1] == checkPokemon[x2][y2] )
	{
		if(checkI())
		// checkL();
		// checkU();
		// checkZ();
			return true;
	}

	return false;
}

// For redraw board

void setTimesDeleteCell()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < length; j++)
		{
			if (i % 4 == 0 || j % 8 == 0)
			{
				if (i == 0 || j == 0 || i == width - 1 || j == length - 1)
					line[i][j] = 1;

				else
					line[i][j] = 2;
			}
			else
				line[i][j] = 1;
		}
}

void redrawBoard()
{
	system("cls");
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < length; j++)
		{
			if (i % 4 == 2 && j % 8 == 4)
				SET_COLOR(2);

			else
				SET_COLOR(7);

			cout << char(board[i][j]);
		}
		cout << endl;
	}
}

void redrawCell()
{
	for (int i = move_x - 1; i < move_x + 4; i++)
		for (int j = move_y - 1; j < move_y + 8; j++)
		{
			line[i][j] += 1;
			board[i][j] = copiedBoard[i][j];
		}
}

void deleteCell(int x, int y)
{
	for (int i = x - 1; i < x + 4; i++)
		for (int j = y - 1; j < y  + 8; j++)
		{
			line[i][j] -= 1;
			if (line[i][j] == 0)
			{
				if (i % 4 == 0 || j % 8 == 0)
					board[i][j] = 32;
				
				else if(i % 4 == 2 && j % 8 == 4)
				{
					if(board[i][j] != -1)
						board[i][j]= board[i][j];

					else if (board[i][j] == -1)
						board[i][j] = 47;
				}

				else board[i][j] = 47;
				
			}
		}
}

// For moving

void moveUp()
{
	if(selected_cell[move_x][move_y] <= 0)
		redrawCell();
	
	if (move_x == 1)
		move_x = width - 4;

	else
		move_x -= 4;
	
	deleteCell(move_x, move_y);
	redrawBoard();
}

void moveDown()
{
	if(selected_cell[move_x][move_y] <= 0)
		redrawCell();

	if (move_x == width - 4)
		move_x = 1;

	else
		move_x += 4;

	deleteCell(move_x, move_y);
	redrawBoard();
}

void moveLeft()
{
	if(selected_cell[move_x][move_y] <= 0)
		redrawCell();

	if (move_y == 1)
		move_y = length - 8;

	else
		move_y -= 8;

	deleteCell(move_x, move_y);
	redrawBoard();
}

void moveRight()
{
	if(selected_cell[move_x][move_y] <= 0)
		redrawCell();

	if (move_y == length - 8)
		move_y = 1;

	else
		move_y += 8;

	deleteCell(move_x, move_y);
	redrawBoard();
}

void getArrow()
{
	unsigned char ch;
	ch = _getch();

	switch (ch)
	{
	case KEY_UP: case 'w' : case 'W' :
		moveUp();
		break;

	case KEY_DOWN: case 's' : case 'S':
		moveDown();
		break;

	case KEY_LEFT: case 'a' : case 'A':
		moveLeft();
		break;

	case KEY_RIGHT: case 'd' : case 'D':
		moveRight();
		break;

	case ENTER_KEY: case '\n':

		if(selected_cell[move_x][move_y] <= 0)
			break;
		
		selected_cell[move_x][move_y] = 1;
		selected_x[timeSelected] = move_x;
		selected_y[timeSelected] = move_y;
		deleteCell(move_x, move_y);
		redrawBoard();

		timeSelected++;
		if(timeSelected == 2)
		{
			if(isTheSame())
			{
				board[selected_x[1]][selected_y[1]] = -1;
				board[selected_x[2]][selected_y[2]] = -1;

				selected_cell[selected_x[1]][selected_y[1]] = -1;
				selected_cell[selected_x[2]][selected_y[2]] = -1;

				deleteCell(selected_x[1], selected_y[1]);
				deleteCell(selected_x[2], selected_y[2]);

				redrawBoard();
			}
			timeSelected = 0;
		}
	}
}

int main()
{
	//SetWindowSize(40, 100);
	//SetScreenBufferSize(40, 100);
	//DisableResizeWindow();
	
	setSizeOfBoard();
	settingStartingBoard();
	
	createPokemon();
	setPokemonIntoBoard();

	//createStartingBoard();
	redrawBoard();
	copyBoard();

	setTimesDeleteCell();
	deleteCell(move_x, move_y);

	while (true)
		getArrow();

	return 0;
}