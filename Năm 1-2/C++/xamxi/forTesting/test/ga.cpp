#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <string.h>
#include <cmath>
using namespace std;

struct highlight{
    int x, y;
};

struct GameData{
    int size;
    int numCha;
    char CharBoard[100][100];
    int deletedCell;
};

struct Player{
    string Name;
    string ID;
    int score = 0;
};

int Was[50][50];
// set surrounded cells
void SetSurroundCell(GameData& Pikachu){
    for (int i = 0; i < (Pikachu.size) * 4 + 1; i++){
        for (int j = 0; j < (Pikachu.size) * 8 + 1; j++){
            if (i % 4 == 0){
                if (j % 8 == 0) {
                    Pikachu.CharBoard[i][j] = ' ';
                }
                else{
                    Pikachu.CharBoard[i][j] = '-';
                }
            }
            else {
                if (j % 8 == 0){
                    Pikachu.CharBoard[i][j] = '|';
                }
                else{
                    Pikachu.CharBoard[i][j] = ' ';
                }
            }
        }
     }
}

//Get char into Pikachu.CharBoard
void GetCharIntoBoard(GameData& Pikachu, int mode) {
    srand(time(NULL));
    int num_type_Pokemon[130];
    int countType[130];

    // Number of types
    Pikachu.numCha = mode;

    // Number of types
    Pikachu.numCha = mode;

    // Create a random number of Pokemon for each type

    for (int i = 0; i < 130; i++)
    {
        num_type_Pokemon[i] = 0;
        countType[i] = 0;
     }

    int numPokemons = Pikachu.size * Pikachu.size;

    for (int i = 0; i < (numPokemons - Pikachu.deletedCell) / 2; i++)
    {
        int type = rand() % Pikachu.numCha + 65;
        num_type_Pokemon[type] += 2;
    }

    // Get pokemon into board6

    for (int i = 2; i < Pikachu.size * 4 + 1; i += 4)
    {
        for (int j = 4; j < Pikachu.size * 8 + 1; j += 8)
            if (Was[i / 4][j / 8] == 0)
        {
            int random = 65;

            while (countType[random] >= num_type_Pokemon[random])
            {
                random = rand() % Pikachu.numCha + 65;
            }

            Pikachu.CharBoard[i][j] = random;
            countType[random]++;
        }
    }
}


void printBoard(GameData& Pikachu, Player p[10], int id){
    system("CLS");
    for (int i = 0; i < Pikachu.size * 4 + 1; i++){
        for (int j = 0; j < Pikachu.size * 8 + 1; j++){
            cout << Pikachu.CharBoard[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << p[id].score;
}

void printBoard1(GameData& Pikachu, highlight highlight1, Player p[10], int id){
    system("CLS");
    HANDLE col;
    //printHelpCell(Pikachu);
    col =  GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < Pikachu.size * 4 + 1; i++){
        for (int j = 0; j < Pikachu.size * 8 + 1; j++) {
            if (highlight1.x * 4 < i && i < highlight1.x * 4 + 4 && highlight1.y * 8 < j && j < highlight1.y * 8 + 8) {
                SetConsoleTextAttribute(col, 240);
                cout << Pikachu.CharBoard[i][j];
                SetConsoleTextAttribute(col, 7);
            } else cout << Pikachu.CharBoard[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << p[id].score;
}
void printBoard2(GameData& Pikachu, highlight highlight1, highlight highlight2, Player p[10], int id){
    system("CLS");
    HANDLE col;
    //printHelpCell(Pikachu);
    col =  GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < Pikachu.size * 4 + 1; i++){
        for (int j = 0; j < Pikachu.size * 8 + 1; j++) {
            if (highlight1.x * 4 < i && i < highlight1.x * 4 + 4 && highlight1.y * 8 < j && j < highlight1.y * 8 + 8) {
                SetConsoleTextAttribute(col, 240);
                cout << Pikachu.CharBoard[i][j];
                SetConsoleTextAttribute(col, 7);
            }
            else if (highlight2.x * 4 < i && i < highlight2.x * 4 + 4 && highlight2.y * 8 < j && j < highlight2.y * 8 + 8) {
                SetConsoleTextAttribute(col, 240);
                cout << Pikachu.CharBoard[i][j];
                SetConsoleTextAttribute(col, 7);
            }
            else
                cout << Pikachu.CharBoard[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << p[id].score;
}

//delete 2 highlighted cells
void deleteUp(GameData& Pikachu, highlight C){
    for (int i = C.y * 8 + 1; i < C.y * 8 + 8; i++)
        Pikachu.CharBoard[C.x * 4][i] = ' ';
}
void deleteDown(GameData& Pikachu, highlight C){
    for (int i = C.y * 8 + 1; i < C.y * 8 + 8; i++)
        Pikachu.CharBoard[C.x * 4 + 4][i] = ' ';
}
void deleteRight(GameData& Pikachu, highlight C){
    for (int i = C.x * 4 + 1; i < C.x * 4 + 4; i++)
        Pikachu.CharBoard[i][C.y * 8 + 8] = ' ';
}
void deleteLeft(GameData& Pikachu, highlight C){
    for (int i = C.x * 4 + 1; i < C.x * 4 + 4; i++)
        Pikachu.CharBoard[i][C.y * 8] = ' ';
}
void deleteChar(GameData& Pikachu, highlight C){
    Pikachu.CharBoard[C.x * 4 + 2][C.y * 8 + 4] = ' ';
}

bool checkUp (GameData& Pikachu, highlight C){
    if (C.x == 0)
        return false;
    else {
        if (Was[C.x - 1][C.y] == 1)
            return true;
        return false;
    }
}
bool checkDown (GameData& Pikachu, highlight C){
    if (C.x == Pikachu.size -1)
        return false;
    else {
        if (Was[C.x + 1][C.y] == 1)
            return true;
        return false;




    }
}
bool checkRight (GameData& Pikachu, highlight C){
    if (C.y == Pikachu.size - 1)
        return false;
    else {
        if (Was[C.x][C.y + 1] == 1)
            return true;
        return false;
    }
}
bool checkLeft (GameData& Pikachu, highlight C){
    if (C.y == 0)
        return false;
    else {
        if (Was[C.x][C.y - 1] == 1)
            return true;
        return false;
    }
}
void deleteCell(GameData& Pikachu, highlight c){
    ++Pikachu.deletedCell;
    deleteChar(Pikachu, c);
    Was[c.x][c.y] = 1;
    if (checkUp(Pikachu, c) == true)
        deleteUp(Pikachu, c);
    if (checkDown(Pikachu, c) == true)
        deleteDown(Pikachu, c);
    if (checkRight(Pikachu, c) == true)
        deleteRight(Pikachu, c);
    if (checkLeft(Pikachu, c) == true)
        deleteLeft(Pikachu, c);
}

void printHelp(GameData& Pikachu, int helpChoice) {
    --helpChoice;
    system("CLS");
    HANDLE col;
    col =  GetStdHandle(STD_OUTPUT_HANDLE);
    const vector<string> helpMenu = {
        " ------------------- ",
        "|                   |",
        "| REPRINT THE BOARD |",
        "|                   |",
        " ------------------- ",
        "|                   |",
        "|   HELP TO MATCH   |",
        "|                   |",
        " ------------------- ",
        "|                   |",
        "|     QUIT HELP     |",
        "|                   |",
        " ------------------- ",
    };
    for (int i = 0; i < helpMenu.size(); i++) {
        if (i % 4 != 0 && helpChoice * 4 < i && i < helpChoice * 4 + 4) {
            string s = helpMenu[i];
            cout << s[0];
            SetConsoleTextAttribute(col, 240);
            cout << s.substr(1, s.size() - 2);
            SetConsoleTextAttribute(col, 7);
            cout << s[s.size() - 1];
            cout << endl;
        } else cout << helpMenu[i] << endl;
    }

}
int displayHelp(GameData& Pikachu) {
    // Clear the screen
    system("CLS");
    char ch = 0;
    // Display the game board with the help menu
    int i = 1;
    printHelp(Pikachu, 1);
    do
     {
         ch = _getch();

        // Skip processing of arrow keys
        if (ch == -32 || ch == 0)
        {
            _getch();
            continue;
        }
         switch(ch)
         {
             case 'W':
             case 'w':
                 if (i > 1) {
                     --i;
                 }
                 else if (i == 1){
                    i = 3;
                 }
                 printHelp(Pikachu, i);
                 break;
             case 's':
             case 'S':
                 if (i < 3) {
                     ++i;
                 }
                 else if (i == 3){
                    i = 1;
                 }
                 printHelp(Pikachu, i);
                 break;

         }

     } while (ch != 13);
     return i;
 }

// Checking I Matching
bool check_I_horizontal(highlight c1, highlight c2) // c1 - c2: cell 1 - cell 2
{
	if (c1.x != c2.x)
		return false;

	// if c1 and c2 are next to each other
	if (abs(c1.y - c2.y) == 1) //
		return true;

	int i = c1.x;
	int minCol, maxCol;
	minCol = min(c1.y, c2.y);
	maxCol = max(c1.y, c2.y);

	// check FROM the next of the first cell TO the previous of the second cell horizontally
	// if Was[][] in range (65, 90) --> there is a cell at the positon --> can't match  c1 - c2
	for (int j = minCol + 1; j < maxCol; j++)
		if (Was[i][j] != 1)
			return false;

	return true;
}

bool check_I_vertical(highlight c1, highlight c2) // c1 - c2: cell 1 - cell 2
{
	if (c1.y != c2.y)
		return false;

	// if c1 and c2 are next to each other
	if (abs(c1.x - c2.x) == 1)
		return true;

	int j = c1.y;
	int minRow, maxRow;
	minRow = min(c1.x, c2.x);
	maxRow = max(c1.x, c2.x);

	// check FROM the next of the first cell TO the previous of the second cell vertically
	// if Was[][] in range (65, 90) --> there is a cell at the positon --> can't match 2 c1 - c2
	for (int i = minRow + 1; i < maxRow; i++)
		if (Was[i][j] != 1)
			return false;

	return true;
}

bool checkI(highlight c1, highlight c2)
{

	if (check_I_horizontal(c1, c2) || check_I_vertical(c1, c2))
		return true;

	return false;
}


// Checking L matching
// check horizontal then vertical: tu ngang den doc
bool check_H_V(highlight c1, highlight c2)
{
	// c3 is position of cell before changing direction
	highlight c3;

	c3.x = c1.x;
	c3.y = c2.y;

	// if cell c3 is a Pokemon
	if (Was[c3.x][c3.y] != 1)
		return false;

	if (!check_I_horizontal(c1, c3))
		return false;

	if (!check_I_vertical(c3, c2))
		return false;

	return true;
}

 // check vertical the horizontal
bool check_V_H(highlight c1, highlight c2)
{
	// c3 is postion of cell before changing direction
	highlight c3;
	c3.x = c2.x;
	c3.y = c1.y;

	if (Was[c3.x][c3.y] != 1)
		return false;

	if (!check_I_vertical(c1, c3))
		return false;

	if (!check_I_horizontal(c3, c2))
		return false;

	return true;
}

bool checkL(highlight c1, highlight c2)
{

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);

	if (check_H_V(c1, c2) || check_V_H(c1, c2))
		return true;

    return false;
}


// Checking U matching
bool rightU(GameData& Pikachu, highlight c1, highlight c2)
{
	// c3 is postion of cell before changing direction
	highlight c3;
	c3.x = c1.x;

	for (int j = c1.y + 1; j < Pikachu.size; j++)
	{
		c3.y = j;
		if (Was[c3.x][c3.y] != 1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
                return true;
		}
	}
	c3.y = Pikachu.size;
    bool ok1 = false, ok2 = false;
    if (check_I_horizontal(c1, c3))
		{
            ok1 = true;
		}
	c3.x = c2.x;
    if (check_I_horizontal(c2, c3))
		{
            ok2 = true;
		}
    if (ok1 && ok2) {
        return true;
    }
	return false;
}

bool leftU(GameData& Pikachu,highlight c1, highlight c2)
{
	// c3 is postion of cell before changing direction
	highlight c3;
	c3.x = c1.x;

	for (int j = c1.y - 1; j >= 0; j--)
	{
		c3.y = j;
		if (Was[c3.x][c3.y] != 1)
			return false;

//		if (check_I_horizontal(c1, c3))
//		{
			if (check_V_H(c3, c2))
				return true;
//		}
	}

	c3.y = -1;
    bool ok1 = false, ok2 = false;
    if (check_I_horizontal(c3, c1))
		{
            ok1 = true;
		}
	c3.x = c2.x;
    if (check_I_horizontal(c3, c2))
		{
            ok2 = true;
		}
    if (ok1 && ok2) {
        return true;
    }
	return false;
}

bool upU(GameData& Pikachu, highlight c1, highlight c2)
{
	highlight c3;
	c3.y = c1.y;
	for (int i = c1.x - 1; i >= 0; i--)
	{
		c3.x = i;
		if (Was[c3.x][c3.y] != 1)
			return false;
        if (check_H_V(c3, c2))
            return true;
	}
    c3.x = -1;
    bool ok1 = false, ok2 = false;
    if (check_I_vertical(c3, c1))
		{
            ok1 = true;
		}
	c3.y = c2.y;
    if (check_I_vertical(c3, c2))
		{
            ok2 = true;
		}
    if (ok1 && ok2) {
        return true;
    }
	return false;
}

bool downU(GameData& Pikachu, highlight c1, highlight c2) //GameData& Pikachu,
{
	highlight c3;
	c3.y = c1.y;

	for (int i = c1.x + 1; i < Pikachu.size; i++)
	{
		c3.x = i;

		if (Was[c3.x][c3.y] != 1)
			return false;
        if (check_V_H(c2, c3)) {
            return true;
        }
	}
	c3.x = Pikachu.size;
    bool ok1 = false, ok2 = false;
    if (check_I_vertical(c3, c1))
		{
            ok1 = true;
		}
	c3.y = c2.y;
    if (check_I_vertical(c3, c2))
		{
            ok2 = true;
		}
    if (ok1 && ok2) {
        return true;
    }

	return false;
}



bool checkU(GameData& Pikachu,highlight c1, highlight c2)
{

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);
	if (rightU(Pikachu, c1, c2) || leftU(Pikachu,c1, c2) || upU(Pikachu, c1, c2) || downU(Pikachu, c1, c2))

		return true;

	return false;
}


// Checking Z matching
bool rightZ(highlight c1, highlight c2)
{
	highlight c3;
	c3.x = c1.x;

	for (int j = c1.y; j <= c2.y; j++)
	{
		c3.y = j;
		if (Was[c3.x][c3.y] != 1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool leftZ(highlight c1, highlight c2)
{
	highlight c3;
	c3.x = c1.x;

	for (int j = c1.y; j >= c2.y; j--)
	{
		c3.y = j;
		if (Was[c3.x][c3.y] != 1)
			return false;

		if (check_I_horizontal(c1, c3))
		{
			if (check_V_H(c3, c2))
				return true;
		}
	}

	return false;
}

bool downZ(highlight c1, highlight c2)
{
	highlight c3;
	c3.y = c1.y;

	for (int i = c1.x; i <= c2.x; i++)
	{
		c3.x = i;

		if (Was[c3.x][c3.y] != 1)
			return false;

		if (check_I_vertical(c1, c3))
		{
			if (check_H_V(c3, c2))
				return true;
		}
	}

	return false;
}

bool checkZ(highlight c1, highlight c2)
{

	// 1st cell is a higher one
	if (c1.x > c2.x)
		swap(c1, c2);

	if (rightZ(c1, c2) || leftZ(c1, c2) || downZ(c1, c2))
		return true;

	return false;
}
 void getHelpFunction(GameData& Pikachu, Player p[10], int id){
    switch(displayHelp(Pikachu)){
        case 1:
            GetCharIntoBoard(Pikachu, 26);
            p[id].score--;
            break;
        case 2:
            for (int i = 0; i < Pikachu.size; i++){
                for (int j = 0; j < Pikachu.size; j++) {
                    if (!Was[i][j])
                        for (int u = 0; u < Pikachu.size; u++) {
                            for (int v = 0; v < Pikachu.size; v++) {
                             if ((!Was[u][v]) && (i != u || j != v)){
                                 highlight c1 = {i, j};
                                 highlight c2 = {u, v};
                                 if (i == u && j == v) continue;
                                 if ((checkI(c1, c2) || checkL(c1, c2) || checkU(Pikachu, c1, c2) || checkZ(c1, c2)) && (Pikachu.CharBoard[c1.x * 4 + 2][c1.y * 8 + 4] == Pikachu.CharBoard[c2.x * 4 + 2][c2.y * 8 + 4])) {
                                    int timeDelay = 100;
                                    printBoard2(Pikachu, c1, c2, p, id);
                                    Sleep(timeDelay);
                                    printBoard(Pikachu, p, id);
                                    Sleep(timeDelay);
                                    printBoard2(Pikachu, c1, c2, p, id);
                                    Sleep(timeDelay);
                                    printBoard(Pikachu, p, id);
                                    Sleep(timeDelay);
                                    printBoard2(Pikachu, c1, c2, p, id);
                                    Sleep(timeDelay);
                                    printBoard(Pikachu, p, id);
                                    Sleep(timeDelay);
                                    p[id].score -= 3;
                                    return;

                                 }
                             }
                        }
                    }
                }
            }
            break;
        case 3:
            break;

        }
}

highlight Select1(GameData& Pikachu, highlight select1, Player p[10], int id) {
    int i = select1.x, j = select1.y;
    char ch = 0;
    printBoard1(Pikachu, { select1.x, select1.y }, p, id);
    do
    {
        ch = _getch();

        // Skip processing of arrow keys
        if (ch == -32 || ch == 0)
        {
            _getch();
            continue;
        }

        switch (ch)
        {
        case 'W':
        case 'w':
            if (i > 0) {
                --i;
            }
            else if (i == 0) {
                i = Pikachu.size - 1;
            }
            printBoard1(Pikachu, { i, j }, p, id);
            break;
        case 'A':
        case 'a':
            if (j > 0) {
                --j;
            }
            else if ((j == 0 && i < Pikachu.size && i != 0)) {
                --i;
                j = Pikachu.size - 1;
            }
            else if (j == 0 && i == 0) {
                i = Pikachu.size - 1;
                j = Pikachu.size - 1;
            }
            printBoard1(Pikachu, { i, j }, p, id);
            break;
        case 's':
        case 'S':
            if (i < Pikachu.size - 1) {
                ++i;
            }
            else if (i == Pikachu.size - 1) {
                i = 0;
            }
            printBoard1(Pikachu, { i, j}, p, id);
            break;
        case 'D':
        case 'd':
            if (j < Pikachu.size - 1) {
                ++j;
            }
            else if (j == Pikachu.size - 1 && i < Pikachu.size - 1) {
                i++;
                j = 0;
            }
            else if (j == Pikachu.size - 1 && i == Pikachu.size - 1) {
                i = 0;
                j = 0;
            }
            printBoard1(Pikachu, { i, j}, p, id);
            break;
        case 'H':
        case 'h':
            getHelpFunction(Pikachu, p, id);
            printBoard1(Pikachu, { i, j}, p, id);
            break;
//        case 27:
//            /**/
        }

    } while (ch != 13);
    return { i, j };
}

highlight Select2(GameData& Pikachu, highlight select1, highlight select2, Player p[10], int id) {
    int i = select2.x, j = select2.y;
    char ch = 0;
    //     cout << "Press Q to quit\n";
    printBoard2(Pikachu, {select1.x, select1.y}, {select2.x, select2.y}, p, id);
    do
    {
        ch = _getch();

        // Skip processing of arrow keys
        if (ch == -32 || ch == 0)
        {
            _getch();
            continue;
        }

        switch (ch)
        {
        case 'W':
        case 'w':
            if (i > 0) {
                --i;
            }
            else if (i == 0) {
                i = Pikachu.size - 1;
            }
            printBoard2(Pikachu, select1, { i, j }, p, id);
            break;
        case 'A':
        case 'a':
            if (j > 0) {
                --j;
            }
            else if ((j == 0 && i < Pikachu.size && i != 0)) {
                --i;
                j = Pikachu.size - 1;
            }
            else if (j == 0 && i == 0) {
                i = Pikachu.size - 1;
                j = Pikachu.size - 1;
            }
            printBoard2(Pikachu, select1, { i, j }, p, id);

            break;
        case 's':
        case 'S':
            if (i < Pikachu.size - 1) {
                ++i;
            }
            else if (i == Pikachu.size - 1) {
                i = 0;
            }
            printBoard2(Pikachu, select1, { i, j }, p, id);
            break;
        case 'D':
        case 'd':
            if (j < Pikachu.size - 1) {
                ++j;
            }
            else if (j == Pikachu.size - 1 && i < Pikachu.size - 1) {
                i++;
                j = 0;
            }
            else if (j == Pikachu.size - 1 && i == Pikachu.size - 1) {
                i = 0;
                j = 0;
            }
            printBoard2(Pikachu, select1, { i, j }, p, id);
            break;
        case 'H':
        case 'h':
            getHelpFunction(Pikachu, p, id);
            printBoard1(Pikachu, { i, j }, p, id);
            break;
        }
//        case 27:
//            /**/

    } while (ch != 13);
    return { i, j };
}

 void highlightHelpCell(GameData& Pikachu, Player p[10], int id) {
    // Set the color to highlight the help cell
    HANDLE col = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(col, 240);

    // Highlight the help cell
    for (int i = Pikachu.size * 4 + 1; i < Pikachu.size * 4 + 4; i++) {
        for (int j = Pikachu.size * 8 + 1; j < Pikachu.size * 8 + 7; j++) {
            Pikachu.CharBoard[i][j] = '-';
        }
    }
Pikachu.CharBoard[Pikachu.size * 4 + 2][Pikachu.size * 8 + 2] = '*';

    // Print the highlighted board
    printBoard1(Pikachu, { -1, -1 }, p, id);

    // Reset the color to default
    SetConsoleTextAttribute(col, 7);
}



//int checkValidMatching(GameData& Pikachu, highlight c1, highlight c2){
//    if ((checkI(c1, c2) || checkL(c1, c2) || checkU(Pikachu, c1, c2) || checkZ(c1, c2)) && (Pikachu.CharBoard[c1.x * 4 + 2][c1.y * 8 + 4] == Pikachu.CharBoard[c2.x * 4 + 2][c2.y * 8 + 4])){
//        deleteCell(Pikachu, c1);
//        deleteCell(Pikachu, c2);
//        printBoard(Pikachu);
//    }
//}



//void NextStage(GameData& Pikachu, highlight c1, highlight c2){
//    //resize the board for the next stage
//    Pikachu.size += 2;
//    // initialize the board with new size
//    SetSurroundCell(Pikachu);
//    GetCharIntoBoard(Pikachu, 26);
//    // again choose c1, c2 with new positions for new choice
//       for (int i = 0; i < Pikachu.size; i++)
//            for (int j = 0; j < Pikachu.size; j++){
//                if (Was[i][j] != 1){
//                    c1.x = c2.x = i;
//                    c1.y = c2.y = j;
//                    break;
//                }
//            }
//        c1 = Select1(Pikachu, c1);
//        c2 = Select2(Pikachu, c1, c1);
//}

void playGame(GameData& Pikachu, highlight c1, highlight c2, int id, Player p[10]){
    int count = pow(Pikachu.size, 2);
    // number of all cells
    while (count - 2 > 0){ // no cell exist
        if ((c1.x != c2.x || c2.y != c1.y) && (checkI(c1, c2) || checkL(c1, c2) || checkU(Pikachu, c1, c2) || checkZ(c1, c2)) && (Pikachu.CharBoard[c1.x * 4 + 2][c1.y * 8 + 4] == Pikachu.CharBoard[c2.x * 4 + 2][c2.y * 8 + 4])){
            deleteCell(Pikachu, c1);
            deleteCell(Pikachu, c2);
            count -= 2;
            p[id].score += 2;
        }
        else
            p[id].score -= 1;
        while (Was[c2.x][c2.y] == 1) {
            if (c2.y < Pikachu.size - 1) {
                ++c2.y;
            }
            else if (c2.y == Pikachu.size - 1 && c2.x < Pikachu.size - 1) {
                c2.x++;
                c2.y = 0;
            }
            else if (c2.y == Pikachu.size - 1 && c2.x == Pikachu.size - 1) {
                c2.x = 0;
                c2.y = 0;
            }
        }
        c1 = c2;
        c1 = Select1(Pikachu, c1, p, id);
        c2 = Select2(Pikachu, c1, c1, p, id);
    }

}


int main(){
    GameData Pikachu;
    Pikachu.size = 4;
    Player player[10];
    int stage = 3;
    int id = 0;
    while (id < 10){
        while (stage > 0){
        //        getline(cin, player[id].Name);
        //        getline(cin, player[id].ID);
                for (int i = 0; i < Pikachu.size; i++)
                    for (int j = 0; j < Pikachu.size; j++)
                        Was[i][j] = 0;
                SetSurroundCell(Pikachu);
                Pikachu.deletedCell = 0;
                GetCharIntoBoard(Pikachu, 26);
                highlight highlight1 = Select1(Pikachu, {0,0}, player, id);
                highlight highlight2 = Select2(Pikachu, highlight1, highlight1, player, id);
                playGame(Pikachu, highlight1, highlight2, id, player);
                player[id].score += 2;
                stage--;
                Pikachu.size += 2;
                //cout << "Your score: " << player[id].score;
        }
        id++;
    }
    return 0;
}

