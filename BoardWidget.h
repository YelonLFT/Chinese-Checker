#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<time.h>

#define BOARD_SIZE 9


typedef struct {
    int start_row;
    int start_col;
    int end_row;
    int end_col;
    int score;
}Move;


typedef struct {
    char board_data[BOARD_SIZE][BOARD_SIZE];
    char jump_memo[BOARD_SIZE][BOARD_SIZE];
    char current_piece;
    int gamemode;   //1:PVP   2:PVE
    int firstdrop;   //1:human   2:Bot
    char AiPiece;
    int count;
}GameState;


void drop_piece(GameState* state, Move move);

void de_drop_piece(GameState* state, Move move);

bool isAJump(int y1, int x1, int y2, int x2, GameState* state);

bool isAMove(int y1, int x1, int y2, int x2, GameState* state);

bool CanJump(int y1, int x1, int y2, int x2, GameState* gameState);

bool CheckValidDropl(int y1, int x1, int y2, int x2, GameState* gameState);

bool validinput(int input[4], GameState* board);

bool CheckWin(GameState* board);

bool isGameOver(GameState* gameState);

void print_map(GameState* board);

void updateGameState(GameState* state);

void resetJumpMemo(GameState* state);

void PVPmode(GameState* gameState);

bool isWinning(GameState* board, char symbol);

void printWinner(GameState* board);

#endif