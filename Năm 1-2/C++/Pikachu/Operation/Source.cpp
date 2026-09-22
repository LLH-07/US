#include <iostream>
#include <iomanip>
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

// Point
struct Point
{
	int x;
	int y;
};



int inRow, inCol; // Board size
int width, length, board[50][100], copiedBoard[50][100];
int rawBoard[50][100];
int boardBeforeBeingChoosen[50][100];

// the number of 'type' pokemon
int num_type_Pokemon[100];

// identity of 'type' pokemon for checking
// -1: deleted at the position, 65 - 90: indentity
int checkPokemon[50][100];

int line[50][100] = {}; // -2 : deleted, -1 : chosen, 0 : temporaryly deleted(moving), 1 - 2 : times can be deleted

// 2 points are selected
Point point_selected[2]; 

// Count numbers of cells are selected for checking
int times = 0;

int checkSelect[50][100] = {}; // 0: not chosen, 1 : chosen, -1 : deleted

Point cellMove;
int cellCleared = inRow * inCol;


// FOR FIRST CREATE BOARD

void SET_COLOR(int color)
{
	WORD wColor;


	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		wColor = (csbi.wAttributes & 0xF0) + (color & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
}

void setSizeOfBoard()
{
	system("cls");
	int choice;
	cout << "PLEASE CHOOSE LEVEL\n";
	cout << "(1) Easy\n" << "(2) Normal\n" << "(3) Hard\n";
	cout << "Your choice(1,2,3): ";
	cin >> choice;
	if (choice == 1)
		inRow = 4, inCol = 4;

	else if (choice == 2)
		inRow = 6, inCol = 6;

	else if (choice == 3)
		inRow = 8, inCol = 8;

	else
		setSizeOfBoard();
}

void settingStartingBoard()
{

	width = inRow * 4 + 1;
	length = inCol * 8 + 1;

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
	{
		int type = rand() % 26 + 65;
		num_type_Pokemon[type] += 2;
	}
}

void setPokemonIntoBoard()
{
	int counttype[100] = {};
	int row = 1, col;
	for (int i = 2; i < width; i += 4)
	{
		col = 1;
		for (int j = 4; j < length; j += 8)
		{
			int type;
			do
			{
				type = rand() % 26 + 65;
			} while (counttype[type] >= num_type_Pokemon[type]);

			board[i][j] = type;
			checkPokemon[row][col] = type;
			col += 8;
			counttype[type]++;
		}
		row += 4;
	}
}


// FOR DRAWING THE BOARD
void copyBoard()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < length; j++)
			copiedBoard[i][j] = board[i][j];
}

void copyRawBoard()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < length; j++)
			rawBoard[i][j] = board[i][j];
}

void setTimesDeleteCell()
{
	for (int i = 0; i < width; i++)
		for (int j = 0; j < length; j++)
		{
			if (i == 0 || j == 0 || i == width - 1 || j == length - 1)
				line[i][j] = 1;

			else if (i % 4 == 0 || j % 8 == 0)
				line[i][j] = 2;

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

void redrawCell(Point cell)
{
	for (int i = cell.x - 1; i < cell.x + 4; i++)
		for (int j = cell.y - 1; j < cell.y + 8; j++)
		{
			if(line[i][j] != -2 )
				line[i][j] += 1;

			board[i][j] = copiedBoard[i][j];
		}
}

void moving(Point cell)
{
	for (int i = cell.x ; i < cell.x + 4 - 1; i++)
		for (int j = cell.y ; j < cell.y + 8 - 1; j++)
		{
			//line[i][j] -= 1;

			if (line[i][j] == 1 || line[i][j] == -1)
			{
				if (i % 4 == 0 || j % 8 == 0)
					board[i][j] = 32;

				else if (i % 4 == 2 && j % 8 == 4)
					board[i][j] = board[i][j];

				else board[i][j] = 46;

				if(line[i][j] != -1) --line[i][j];

			}
		}
}

void deleteCell(Point cell)
{
	for (int i = cell.x - 1; i < cell.x + 4; i++)
		for (int j = cell.y - 1; j < cell.y + 8; j++)
		{
			if (line[i][j] == -1)
			{
				line[i][j] = -1;
				//if (i % 4 == 0 || j % 8 == 0)
					//board[i][j] = 32;

				board[i][j] = 35;

			}
		}

	copyBoard();
}

void recoverCell(Point cell)
{
	for (int i = cell.x; i < cell.x + 4 - 1; i++)
		for (int j = cell.y; j < cell.y + 8 - 1; j++)
		{
			copiedBoard[i][j] = rawBoard[i][j];
			line[i][j] = 1;
		}
}


void draw_I_connection()
{
	// draw line connection

	redrawBoard();

	// delete 2 cells & delete line connection

	redrawBoard();
}
void draw_L_connection();
void draw_U_connection();
void draw_Z_connection();

//

void endGame()
{
	system("slc");
	cout << setw(10) << "CONGRATULATION!";
	cout << setw(12) << "YOU WON!";
}

// For
// source: https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key

// FOR CHECKING

// Checking I Matching
bool check_I_horizontal(Point c1, Point c2) // c1 - c2: cell 1 - cell 2
{
	if (c1.x != c2.x)
		return false;

	// if c1 and c2 are next to each other
	if (abs(c1.y - c2.y) == 8)
		return true;

	int i = c1.x;
	int minCol, maxCol;
	minCol = min(c1.y, c2.y);
	maxCol = max(c1.y, c2.y);

	// check FROM the next of the first cell TO the previous of the second cell horizontally
	// if checkPokemon[][] in range (65, 90) --> there is a cell at the positon --> can't match  c1 - c2
	for (int j = minCol + 8; j <= maxCol - 8; j += 8)
		if (checkPokemon[i][j] != -1)
			return false;
	
	return true;
}

bool check_I_vertical(Point c1, Point c2) // c1 - c2: cell 1 - cell 2
{
	if (c1.y != c2.y)
		return false;

	// if c1 and c2 are next to each other
	if (abs(c1.x - c2.x) == 4)
		return true;

	int j = c1.y;
	int minRow, maxRow;
	minRow = min(c1.x, c2.x);
	maxRow = max(c1.x, c2.x);

	// check FROM the next of the first cell TO the previous of the second cell vertically
	// if checkPokemon[][] in range (65, 90) --> there is a cell at the positon --> can't match 2 c1 - c2
	for (int i = minRow + 4; i <= maxRow - 4; i += 4)
		if (checkPokemon[i][j] != -1)
			return false;

	return true;
}

bool checkI()
{
	Point c1, c2;

	c1.x = point_selected[0].x;
	c1.y = point_selected[0].y;

	c2.x = point_selected[1].x;
	c2.y = point_selected[1].y;

	if (check_I_horizontal(c1, c2) || check_I_vertical(c1, c2))
		return true;

	return false;
}


// Checking L matching
// check horizontal then vertical
bool check_H_V(Point c1, Point c2)
{
	// c3 is postion of cell before changing direction
	Point c3;
	c3.x = c1.x;
	c3.y = c2.y;

	// if cell c3 is a pokemon
	if (checkPokemon[c3.x][c3.y] != -1)
		return false;

	if (!check_I_horizontal(c1, c3))
		return false;

	if (!check_I_vertical(c3, c2))
		return false;

	return true;
}

// check vetical the horizontal
bool check_V_H(Point c1, Point c2)
{
	// c3 is postion of cell before changing direction
	Point c3;
	c3.x = c2.x;
	c3.y = c1.y;

	if (checkPokemon[c3.x][c3.y] != -1)
		return false;

	if (!check_I_vertical(c1, c3))
		return false;

	if (!check_I_horizontal(c3, c2))
		return false;

	return true;
}

bool checkL()
{
	// It's a combination of 2 I-matching
	Point c1, c2;

	c1.x = point_selected[0].x;
	c1.y = point_selected[0].y;

	c2.x = point_selected[1].x;
	c2.y = point_selected[1].y;

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);

	if (check_H_V(c1,c2) || check_V_H(c1, c2))
		return true;

	return false;
}


// Checking U matching
bool rightU(Point c1, Point c2)
{
	// c3 is postion of cell before changing direction
	Point c3;
	c3.x = c1.x;
	
	for (int j = c2.y + 8; j <= length - 8; j += 8)
	{
		c3.y = j;
		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool leftU(Point c1, Point c2)
{
	// c3 is postion of cell before changing direction
	Point c3;
	c3.x = c1.x;

	for (int j = c2.y - 8; j >= 1; j -= 8)
	{
		c3.y = j;
		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool upU(Point c1, Point c2)
{
	Point c3;
	c3.y = c1.y;
	for (int i = c1.x - 4; i >= 1; i -= 4)
	{
		c3.x = i;
		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_vertical(c1, c3))
		{
			if (check_H_V(c3, c2))
				return true;
		}
	}

	return false;
}

bool downU(Point c1, Point c2)
{
	Point c3;
	c3.y = c1.y;

	for (int i = c2.x + 4; i <= width - 4; i += 4)
	{
		c3.x = i;

		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_vertical(c1, c3))
		{
			if (check_H_V(c2, c3))
				return true;
		}
	}

	return false;
}

bool checkU()
{
	Point c1, c2;
	c1.x = point_selected[0].x;
	c1.y = point_selected[0].y;

	c2.x = point_selected[1].x;
	c2.y = point_selected[1].y;

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);

	if (rightU(c1,c2) || leftU(c1,c2) || upU(c1,c2) || downU(c1,c2))
		return true;

	return false;
}


// Checking Z matching
bool rightZ(Point c1, Point c2)
{
	Point c3;
	c3.x = c1.x;

	for (int j = c1.y + 8; j <= c2.y - 8; j += 8)
	{
		c3.y = j;
		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool leftZ(Point c1, Point c2)
{
	Point c3;
	c3.x = c1.x;

	for (int j = c1.y - 8; j >= c2.y + 8; j -= 8)
	{
		c3.y = j;
		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool downZ(Point c1, Point c2)
{
	Point c3;
	c3.y = c1.y;

	for (int i = c1.x + 4; i <= c2.x - 4; i += 4)
	{
		c3.x = i;

		if (checkPokemon[c3.x][c3.y] != -1)
			return false;

		if (check_I_vertical(c1, c3))
		{
			if (check_H_V(c3, c2))
				return true;
		}
	}

	return false;
}

bool checkZ()
{
	Point c1, c2;
	c1.x = point_selected[0].x;
	c1.y = point_selected[0].y;

	c2.x = point_selected[1].x;
	c2.y = point_selected[1].y;

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);

	if (rightZ(c1, c2) || leftZ(c1, c2) || downZ(c1, c2))
		return true;

	return false;
}
       
void checkTheSame()
{
	bool same = false;

	if (checkPokemon[point_selected[0].x][point_selected[0].y] != checkPokemon[point_selected[1].x][point_selected[1].y])
		same = false;

	else 
		if (checkI() || checkL() || checkU() || checkZ())
		{
			same = true;
			checkPokemon[point_selected[0].x][point_selected[0].y] = -1;
			checkPokemon[point_selected[1].x][point_selected[1].y] = -1;
			checkSelect[point_selected[0].x][point_selected[0].y] = -1;
			checkSelect[point_selected[1].x][point_selected[1].y] = -1;
			deleteCell(point_selected[0]);
			deleteCell(point_selected[1]);
			redrawCell(point_selected[0]);
			redrawCell(point_selected[1]);
			cellCleared -= 2;
		}

	if (same == false)
	{
		checkSelect[point_selected[0].x][point_selected[0].y] = 0;
		checkSelect[point_selected[1].x][point_selected[1].y] = 0;
		recoverCell(point_selected[0]);
		recoverCell(point_selected[1]);
		redrawBoard();
	}

}


// FOR SELECTION

void afterChosen(Point& cell)
{
	// Move to the right 1 cell
	if (cell.y != length - 8)
		cell.y += 8;

	else
		cell.y -= 8;

	moving(cell);
	if (times == 2)
	{
		checkTheSame();
		recoverCell(cell);
	}

	redrawBoard();
}

void choose(Point& cell)
{
	for (int i = cell.x; i < cell.x + 4 - 1; i++)
		for (int j = cell.y; j < cell.y + 8 - 1; j++)
		{
			if (line[i][j] == 0)
			{
				if (i % 4 == 0 || j % 8 == 0)
					board[i][j] = 32;

				else if (i % 4 == 2 && j % 8 == 4)
					board[i][j] = board[i][j];

				else board[i][j] = 42;

				line[i][j]--;

			}
		}

	copyBoard();
}

void Select(Point& cell) // noveCell
{
	// check if cell is chosen / deleted or not
	if (checkSelect[cell.x][cell.y] == 1 || checkSelect[cell.x][cell.y] == -1)
		return;

	// mark cell at point (x,y) is chosen
	checkSelect[cell.x][cell.y] = 1;
	point_selected[times].x = cell.x;
	point_selected[times].y = cell.y;

	times++;
	choose(cell);
	afterChosen(cell);
}

// FOR MOVING

void moveUp()
{
		redrawCell(cellMove);

	if (cellMove.x == 1)
		cellMove.x = width - 4;

	else
		cellMove.x -= 4;

	moving(cellMove);
	redrawBoard();
}

void moveDown()
{
//	if (checkSelect[cellMove.x][cellMove.y] == 0)
		redrawCell(cellMove);

	if (cellMove.x == width - 4)
		cellMove.x = 1;

	else
		cellMove.x += 4;

	moving(cellMove);
	redrawBoard();
}

void moveLeft()
{
//	if (checkSelect[cellMove.x][cellMove.y] == 0)
		redrawCell(cellMove);

	if (cellMove.y == 1)
		cellMove.y = length - 8;

	else
		cellMove.y -= 8;

	moving(cellMove);
	redrawBoard();
}

void moveRight()
{
//	if(checkSelect[cellMove.x][cellMove.y] == 0)
		redrawCell(cellMove);

	if (cellMove.y == length - 8)
		cellMove.y = 1;

	else
		cellMove.y += 8;

	moving(cellMove);
	redrawBoard();
}

void getArrow()
{
	unsigned char ch;
	ch = _getch();

	switch (ch)
	{
	case KEY_UP: case 'w': case 'W':
		moveUp();
		break;

	case KEY_DOWN: case 's': case 'S':
		moveDown();
		break;

	case KEY_LEFT: case 'a': case 'A':
		moveLeft();
		break;

	case KEY_RIGHT: case 'd': case 'D':
		moveRight();
		break;

	case ENTER_KEY: case '\n':
		Select(cellMove);
		if (times == 2)
		{
			
			times = 0;
			recoverCell(cellMove);
		}

		break;
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

	copyBoard();
	copyRawBoard();

	cellMove.x = 1;
	cellMove.y = 1;
	setTimesDeleteCell();
	moving(cellMove);
	redrawBoard();

	while (true)
		getArrow();

	endGame();

	return 0;
}