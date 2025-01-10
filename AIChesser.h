#ifndef AICHESSER_H
#define AICHESSER_H

#include"BoardWidget.h"

#define INF 1000000000

#define MAX_DEPTH 4

typedef struct {
    int history[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
    int size;
} HistoryTable;

typedef struct {
    int positions[6][2];  // 存储获胜所需的6个位置坐标
    int count;           // 当前已占据的位置数
} WinPattern;

//#define SAFE_FREE(p) do { if(p) { free(p); p = NULL; } } while(0)
//#define CHECK_NULL(p, msg) do { if(!p) { printf("Error: %s\n", msg); exit(1); } } while(0)
/*
typedef struct {
    Move* moves;
    int capacity;
    int count;
} MoveList;*/

//MoveList* createMoveList(int initial_capacity);

//void freeMoveList(MoveList* list);

int max(int a, int b);

int min(int a, int b);

bool currentPlayerIsAI(GameState* state);

bool CheckValidDropl(int y1, int x1, int y2, int x2, GameState* gameState);

int generateLegalMoves(GameState* state, Move moves[], int* moveCount);

int Manhattan_distance(int start_row, int start_col, int end_row, int end_col);

int evaluateNormal(GameState* state, char mysymbol);

int evaluateGameState(GameState* state, char mysymbol);

Move alphabeta(GameState* state, int alpha, int beta, int depth, bool maximizingPlayer);

Move findBestMove(GameState* state);

int evaluateWinningPosition(GameState* state, char mysymbol);

int evaluateWinningPosition_detailed(GameState* state, char mysymbol, int targetArea[][2]);

int analyzeRemainingMoves(GameState* state, char mysymbol, int targetArea[][2]);

int distanceScore(GameState* state, char mysymbol, int status);

int groupScore(GameState* state, char mysymbol, int status);

int jumpScore(GameState* state, char mysymbol, int status);


#endif
