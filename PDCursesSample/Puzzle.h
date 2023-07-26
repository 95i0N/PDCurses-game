#pragma once
// INCLUDE-------------------------------
#include <random>

// DEFINE--------------------------------
#define WIDTH (6)
#define HEIGHT (5)
#define DROP_MAX (6)
#define TIME_LIMIT (10)
#define RED_PAIR (1)
#define BLUE_PAIR (2)
#define YELLOW_PAIR (3)
#define GREEN_PAIR (4)
#define PURPLE_PAIR (5)
#define CYAN_PAIR (6)
#define WHITE_PAIR (7)

// STRUCT-------------------------------
struct Point {
	int x, y;
};

// VARIABLES-----------------------------

int board[HEIGHT][WIDTH] = {
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
};
bool mark[HEIGHT][WIDTH] = {
	{false,false,false,false,false,false},
	{false,false,false,false,false,false},
	{false,false,false,false,false,false},
	{false,false,false,false,false,false},
	{false,false,false,false,false,false}
};
int order[HEIGHT][WIDTH] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};


// PROTOTYPE DECLARATION----------
void initBoard();
bool markCombo();
int orderCombo(int _combo);
void removeCombo(int num);
bool fallDrop();

void initBoard() {
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			board[y][x] = rand() % DROP_MAX + 1;
		}
	}
	while (markCombo()) {
		for (y = 0; y < HEIGHT; y++) {
			for (x = 0; x < WIDTH; x++) {
				if (mark[y][x])board[y][x] = rand() % DROP_MAX + 1;
			}
		}
	}
}

bool markCombo() {
	int x, y;
	bool flag = false;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			mark[y][x] = false;
		}
	}
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH - 2; x++) {
			if ((board[y][x] == board[y][x + 1]) && (board[y][x] == board[y][x + 2])) {
				mark[y][x] = true;
				mark[y][x + 1] = true;
				mark[y][x + 2] = true;
				flag = true;
			}
		}
	}
	for (y = 0; y < HEIGHT - 2; y++) {
		for (x = 0; x < WIDTH; x++) {
			if ((board[y][x] == board[y + 1][x]) && (board[y][x] == board[y + 2][x])) {
				mark[y][x] = true;
				mark[y + 1][x] = true;
				mark[y + 2][x] = true;
				flag = true;
			}
		}
	}
	return flag;
}

int orderCombo(int _combo) {
	int x, y;
	int bc = 0;
	int d=0;
	int count = 0;
	
	for (y = HEIGHT - 1; y >= 0; y--) {
		for (x = 0; x < WIDTH; x++) {
		
			if (mark[y][x] == true) {
				int tmp1 = 0;
				int tmp2 = 0;
				if (y < HEIGHT - 1) {
					if (mark[y + 1][x] == true) {
						if (board[y][x] == board[y + 1][x])tmp1 = order[y + 1][x];
					}
				}
				if (x > 0) {
					if (mark[y][x - 1] == true) {
						if (board[y][x] == board[y][x - 1])tmp2 = order[y][x - 1];
					}
				}
				if (tmp1 == 0) {
					if (tmp2 == 0) {
						count++;
						order[y][x] = count;
					}
					else {
						order[y][x] = tmp2;
					}
				}
				else if (tmp2 == 0) {
					order[y][x] = tmp1;
				}
				else if (tmp1 == tmp2) {
					order[y][x] = tmp1;
				}
				else {
					int min = tmp1 < tmp2 ? tmp1 : tmp2;
					int max = tmp1 > tmp2 ? tmp1 : tmp2;
					order[y][x] = min;
					int i, j;
					for (j = 0; j < HEIGHT; j++) {
						for (i = 0; i < WIDTH; i++) {
							if (order[j][i] == max)order[j][i] = min;
						}
					}
					bc++;

				}

			}
		}
	}
	if (bc > 0) {
		bool* blank = new bool[count];
		int k;
		for (k = 0; k < count; k++) {
			blank[k] = false;
		}

		// check blank order
		for (k = 0; k < count; k++) {
			for (y = 0; y < HEIGHT; y++) {
				for (x = 0; x < WIDTH; x++) {
					if (order[y][x] == k + 1) {
						blank[k] = true;
						goto OUTER_LOOP_CONTINUE_1;
					}
				}
			}
			OUTER_LOOP_CONTINUE_1:;
		}
		for (k = 0; k < count; k++) {
			if (blank[k] == false) {
				d++;
			}
			else {
				for (y = 0; y < HEIGHT; y++) {
					for (x = 0; x < WIDTH; x++) {
						if (order[y][x] == k + 1) {
							order[y][x] = k - d + 1;
						}
					}
				}
			}
		}
	}
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (order[y][x] > 0) {
				order[y][x] += _combo;
			}
		}
	}
	return count-d;
}

void removeCombo(int num) {
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (order[y][x] == num) {
				order[y][x] = 0;
				board[y][x] = 0;
			}
		}
	}
}

bool fallDrop() {
	int x, y;
	for (y = HEIGHT - 2; y >= 0; y--) {
		for (x = 0; x < WIDTH; x++) {
			if (board[y][x] != 0 && board[y + 1][x] == 0) {
				board[y + 1][x] = board[y][x];
				board[y][x] = 0;
			}
		}
	}
	for (x = 0; x < WIDTH; x++) {
		if (board[0][x] == 0) {
			board[0][x] = rand() % DROP_MAX + 1;
		}
	}
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (board[y][x] == 0)return true;
		}
	}
	return false;
}