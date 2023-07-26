#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <curses.h>
#include "Setting.h"
#include "Puzzle.h"

// PROTOTYPE DECLARATION------------------------
int getKey(char _ch);
void DrawChar(int posX, int posY, char cr, int colorNum);
void DrawString(int posX, int posY, const char* string, int colorNum);
void DrawBoard();
void DrawCombo();
void DrawPhase();

// VARIABLES------------------------------------
Point pos = { 0, 0 };
int toErase = 0;
int comboNum = 0;
int tmpPhase = 0;					// 0:grab	1:swipe		2:remove		3:fall
int bestCombo = 0;
char str[CHARBUFF];


int main(int argc, char* argv[])
{
	//Initialize Screen
    initscr();
	nodelay(stdscr, true);
    noecho();
    start_color();
    init_pair(RED_PAIR, COLOR_RED, COLOR_BLACK);
    init_pair(BLUE_PAIR, COLOR_CYAN, COLOR_BLACK);
    init_pair(YELLOW_PAIR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GREEN_PAIR, COLOR_GREEN, COLOR_BLACK);
    init_pair(PURPLE_PAIR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(WHITE_PAIR, COLOR_WHITE, COLOR_BLACK);
	resize_term(16, 32);
	
	char currentDirectory[CHARBUFF];
	getCurrentDirectory(currentDirectory);
	char section[CHARBUFF];
	char keyWord[CHARBUFF];
	char settingFile[CHARBUFF];
	char keyValue[CHARBUFF];
	sprintf_s(section, "Settings");
	sprintf_s(keyWord, "FilePath");
	sprintf_s(settingFile, "%s¥¥setting.ini", currentDirectory);
	GetPrivateProfileString(section, keyWord, "none", keyValue, CHARBUFF, settingFile);
	
	int x, y;
	int tmp;
	int top = 0;
	int preCombo = 0;
	bool flag1 = true;
	Point next;
	srand((unsigned int)time(NULL));
	initBoard();
	DrawBoard();

	while (true) {
		int key = getKey(getch());
		switch (tmpPhase) {
		case 0:// Grab Drop Phase
			switch (key) {
			case 0://w -> UP
				if (pos.y > 0) {
					pos.y -= 1;
				}
				break;
			case 1://a -> LEFT
				if (pos.x > 0) {
					pos.x -= 1;
				}
				break;
			case 2://s -> DOWN
				if (pos.y < HEIGHT - 1) {
					pos.y += 1;
				}
				break;
			case 3://d -> RIGHT
				if (pos.x < WIDTH - 1) {
					pos.x += 1;
				}
				break;
			case 4://o -> Select
				tmpPhase++;
				break;
			default:
				break;
			}

			break;
		case 1:// Swipe Drop Phase
			switch (key) {
			case 0://w -> UP
				if (pos.y > 0) {
					next = { pos.x, pos.y - 1 };
					tmp = board[next.y][next.x];
					board[next.y][next.x] = board[pos.y][pos.x];
					board[pos.y][pos.x] = tmp;
					pos.y -= 1;
				}
				break;
			case 1://a -> LEFT
				if (pos.x > 0) {
					next = { pos.x - 1, pos.y };
					tmp = board[next.y][next.x];
					board[next.y][next.x] = board[pos.y][pos.x];
					board[pos.y][pos.x] = tmp;
					pos.x -= 1;
				}
				break;
			case 2://s -> DOWN
				if (pos.y < HEIGHT - 1) {
					next = { pos.x, pos.y + 1 };
					tmp = board[next.y][next.x];
					board[next.y][next.x] = board[pos.y][pos.x];
					board[pos.y][pos.x] = tmp;
					pos.y += 1;
				}
				break;
			case 3://d -> RIGHT
				if (pos.x < WIDTH - 1) {
					next = { pos.x + 1, pos.y };
					tmp = board[next.y][next.x];
					board[next.y][next.x] = board[pos.y][pos.x];
					board[pos.y][pos.x] = tmp;
					pos.x += 1;
				}
				break;
			case 4://o -> Select
				flag1 = true;
				comboNum = 0;
				tmpPhase = 2;
				break;
			default:
				break;
			}
			break;
		case 2:// Remove Drop Phase
			markCombo();
			if (flag1) {
				flag1 = false;
				toErase = orderCombo(comboNum);
			}

			
			if (toErase > 0) {
				napms(160);
				if (comboNum - preCombo < toErase)
				{
					removeCombo(++comboNum);
				}
				else
				{
					preCombo = comboNum;
					tmpPhase = 3;
					break;
				}
			}
			else {
				comboNum = 0;
				preCombo = 0;
				tmpPhase = 0;
				break;
			}
		case 3:// Fall Drop Phase
			napms(120);
			if (!fallDrop()) {
				flag1=true;
				tmpPhase=2;
			}
			break;
		default:
			break;
		}

		// Check Highest Score and Write
		if (comboNum > bestCombo)bestCombo = comboNum;
		FILE* fp;
		errno_t error = fopen_s(&fp, keyValue, "r");
		if (error == 0) {
			while (fgets(str, CHARBUFF, fp) != NULL) {
				top = atoi(str);
			}
			fclose(fp);
		}
		if (bestCombo > top) {
			top = bestCombo;
		}
		error = fopen_s(&fp, "Score.txt", "a");
		if (error == 0) {
			sprintf_s(str, "%d\n", top);
			fputs(str, fp);
			fclose(fp);
		}



		// Display
		refresh();
		DrawBoard();
		DrawCombo();
		DrawPhase();
		refresh();
		napms(20);
	}
	return 0;
}

int getKey(char _ch) {
	int button[] = { 'w','a','s','d','o'};
	for (int i = 0; i < 5; i++) {
		if (_ch == button[i])return i ;
	}
	return -1;
}

void DrawChar(int posX, int posY, char cr, int colorNum) {
	attron(COLOR_PAIR(colorNum));
	mvaddch(posY, posX, cr);
}

void DrawString(int posX, int posY, const char* string, int colorNum) {
	attron(COLOR_PAIR(colorNum));
	mvaddstr(posY, posX, string);
}

void DrawBoard() {
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (board[y][x] == 0) {
				DrawString(1 + 2 * x, 2 + y, " ", 1);
			}
			else if((tmpPhase<2)&&(pos.x==x)&&(pos.y==y)){
				DrawString(1 + 2 * x, 2 + y, "@", board[y][x]);
			}
			else {
				DrawString(1 + 2 * x, 2 + y, "*", board[y][x]);
			}
		}
	}
}

void DrawCombo() {
	sprintf_s(str, "%02d", comboNum);
	DrawString(0, 7, "COMBO : ", YELLOW_PAIR);
	DrawString(8, 7, str, YELLOW_PAIR);
	sprintf_s(str, "%02d", bestCombo);
	DrawString(0, 8, "BEST : ", RED_PAIR);
	DrawString(8, 8, str, RED_PAIR);
}

void DrawPhase() {
	if (tmpPhase == 0) {
		DrawString(0, 0, "Grab a drop with o ", WHITE_PAIR);
	}
	else if (tmpPhase == 1) {
		DrawString(0, 0, "Chain over 3 drops ", WHITE_PAIR);
	}
	else if (tmpPhase == 2) {
		DrawString(0, 0, "Chains are removed ", WHITE_PAIR);
	}
	else if (tmpPhase == 3) {
		DrawString(0, 0, "New drops fill blank", WHITE_PAIR);
	}
}

