#include"AIChesser.h"
/*
MoveList* createMoveList(int initial_capacity) {
    MoveList* list = (MoveList*)malloc(sizeof(MoveList));
    if (!list) {
        printf("Failed to allocate MoveList\n");
        return NULL;
    }

    list->moves = (Move*)malloc(initial_capacity * sizeof(Move));
    if (!list->moves) {
        free(list);
        printf("Failed to allocate moves array\n");
        return NULL;
    }

    list->capacity = initial_capacity;
    list->count = 0;
    return list;
}

void freeMoveList(MoveList* list) {
    if (list) {
        SAFE_FREE(list->moves);
        SAFE_FREE(list);
    }
} */

static clock_t start_time;
static bool timeout_flag = false;

HistoryTable historyTable = { 0 };

void resetJumpMemo(GameState* state) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            state->jump_memo[i][j] = 0;
        }
    }
}

bool currentPlayerIsAI(GameState* state) {
    if (state->current_piece == state->AiPiece)
        return true;
    else
        return false;
}

int getMoveScore(Move* move, GameState* gameState, char symbol) {
    char mysymbol = gameState->current_piece;
    char opsymbol = (mysymbol == 'X') ? 'O' : 'X';
    int score = 0;
    drop_piece(gameState, *move);
    score = evaluateGameState(gameState, mysymbol);
    de_drop_piece(gameState, *move);
    //manhatten score
    return score;
}

int generateLegalMoves(GameState* state, Move moves[], int* moveCount) {
    resetJumpMemo(state);
    *moveCount = 0;
    for (int y1 = 0; y1 < BOARD_SIZE; y1++) {
        for (int x1 = 0; x1 < BOARD_SIZE; x1++) {
            if (state->board_data[y1][x1] == state->current_piece) {
                for (int y2 = 0; y2 < BOARD_SIZE; y2++) {
                    for (int x2 = 0; x2 < BOARD_SIZE; x2++) {
                        if (y1 == y2 && x1 == x2)
                            continue;
                        if (CheckValidDropl(y1, x1, y2, x2, state)) {
                            Move move;
                            move.start_row = y1;
                            move.start_col = x1;
                            move.end_row = y2;
                            move.end_col = x2;
                            moves[(*moveCount)++] = move;
                        }
                    }
                }
            }
        }
    }
    return *moveCount;
}

void sortMoves(Move moves[], int count, HistoryTable* historyTable, GameState* state) {
    for (int i = 0; i < count; i++) {
        moves[i].score = historyTable->history[moves[i].start_row][moves[i].start_col][moves[i].end_row][moves[i].end_col];

        // 捕获移动获得更高优先级
        if (isAJump(moves[i].start_row, moves[i].start_col,
            moves[i].end_row, moves[i].end_col, state)) {
            moves[i].score += 1000;
        }
        
        drop_piece(state, moves[i]);
        int winningScore = evaluateWinningPosition(state, state -> current_piece);
        if (abs(winningScore) >= 900) {  // 接近必胜/必败的阈值
            moves[i].score += 6000;
        }
        de_drop_piece(state, moves[i]);
    }

    // 简单冒泡排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (moves[j].score < moves[j + 1].score) {
                Move temp = moves[j];
                moves[j] = moves[j + 1];
                moves[j + 1] = temp;
            }
        }
    }
}

Move alphabeta(GameState* state, int alpha, int beta, int depth, bool maximizingPlayer) {

    if (depth > 2 && (double)(clock() - start_time) / CLOCKS_PER_SEC >= 4.3) {
        timeout_flag = true;
        Move timeoutMove = { 0,0,0,0, maximizingPlayer ? -INF : INF };
        return timeoutMove;
    }
    char mysymbol = state->current_piece;
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';

    Move bestMove = { 0,0,0,0, maximizingPlayer ? -INF : INF };

    if (isGameOver(state) || depth <= 0) {
        bestMove.score = evaluateGameState(state, mysymbol);
        return bestMove;
    }

    Move moves[200] = { 0,0,0,0,0 };
    int moveCount = 0;
    moveCount = generateLegalMoves(state, moves, &moveCount);


    if (moveCount == 0) {
        bestMove.score = evaluateGameState(state, mysymbol);
        return bestMove;
    }

    sortMoves(moves, moveCount, &historyTable, state);


    if (maximizingPlayer) {
        // bestMove.score = -INF;
        for (int i = 0; i < moveCount; i++) {
            drop_piece(state, moves[i]);
            Move move = alphabeta(state, alpha, beta, depth - 1, false);
            de_drop_piece(state, moves[i]);

            if (timeout_flag) {
                return bestMove;
            }

            if (move.score > bestMove.score) {
                bestMove = moves[i];
                bestMove.score = move.score;
            }
            alpha = max(alpha, bestMove.score);
            if (alpha >= beta) {
                historyTable.history[moves[i].start_row][moves[i].start_col]
                    [moves[i].end_row][moves[i].end_col] += depth * depth;
                break;
            }
        }
    }

    else {
        // bestMove.score = INF;
        for (int i = 0; i < moveCount; i++) {
            drop_piece(state, moves[i]);
            Move move = alphabeta(state, alpha, beta, depth - 1, true);
            de_drop_piece(state, moves[i]);
            if (timeout_flag) {
                return bestMove;
            }
            if (move.score < bestMove.score) {
                bestMove = moves[i];
                bestMove.score = move.score;
            }
            beta = min(beta, bestMove.score);
            if (alpha >= beta) {
                historyTable.history[moves[i].start_row][moves[i].start_col]
                    [moves[i].end_row][moves[i].end_col] += depth * depth;
                break;
            }
        }
    }

    return bestMove;
}

Move findBestMove(GameState* state) {
    Move bestMove = { 0 };
    Move tempMove;

    // 重置时间控制变量
    start_time = clock();
    timeout_flag = false;

    // 先进行最浅层搜索确保有结果
    bestMove = alphabeta(state, -INF, INF, 2, true);

    // 迭代加深搜索
    for (int depth = 3; depth <= MAX_DEPTH; depth += 1) {
        // 检查是否还有足够时间进行下一次迭代
        if ((double)(clock() - start_time) / CLOCKS_PER_SEC >= 4.3) {
            break;
        }

        timeout_flag = false;  // 重置超时标志
        tempMove = alphabeta(state, -INF, INF, depth, true);

        // 如果这次迭代完整完成（没有超时），则更新最佳移动
        if (!timeout_flag) {
            bestMove = tempMove;
        }
        else {
            break;
        }

        // 如果找到必胜局面，可以提前结束
        if (bestMove.score >= 1900) {
            break;
        }
    }

    return bestMove;
}

int distanceScore(GameState* state, char mysymbol,int status) {
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';
    int my_end_row = (mysymbol == 'O') ? 1 : 8;
    int my_end_col = (mysymbol == 'O') ? 1 : 8;
    int score = 0;
    for (int i = 1; i < BOARD_SIZE; i++) {
        for (int j = 1; j < BOARD_SIZE; j++) {
            if (state->board_data[i][j] == mysymbol) {
                int distance = Manhattan_distance(i, j, my_end_row, my_end_col);
                if(status==1)
                    score += (37 - distance) * 8;
                if (status == 2)
                    score += (46 - distance) * 7;
                if (status == 3)
                    score += (54 - distance) * 9;
            }
        }
    }
    return score;
}

int groupScore(GameState* state, char mysymbol, int status) {
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';
    int score = 0;
    
    for (int i = 1; i < BOARD_SIZE; i++) {
        for (int j = 1; j < BOARD_SIZE; j++) {
            if (state->board_data[i][j] == mysymbol) {
                int grouped = 0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (i + di >= 0 && i + di < BOARD_SIZE && j + dj >= 0 && j + dj < BOARD_SIZE) {
                            if (state->board_data[i + di][j + dj] == mysymbol) {
                                grouped++;  //集中度权重
                            }
                        }
                    }
                }
                if (status == 1)
                    score += grouped*1;
                if (status == 2)
                    score += grouped *2;
                if (status == 3)
                    score += grouped*3;  // 奖励集中的棋子
            }
        }
    }
    return score;
}

int jumpScore(GameState* state, char mysymbol, int status) {
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';
    int score = 0;
    int jumpchoose = 0;
    int opjumpchoose = 0;

    for (int i = 1; i < BOARD_SIZE; i++) {
        for (int j = 1; j < BOARD_SIZE; j++) {
            if (state->board_data[i][j] == mysymbol) {

                for (int k = 1; k < BOARD_SIZE; k++) {
                    for (int l = 1; l < BOARD_SIZE; l++) {
                        if (CanJump(i, j, k, l, state)) {
                            jumpchoose++;
                            if (mysymbol == 'O') {
                                if (k == 1 && l == 1 || k == 1 && l == 2 || k == 1 && l == 3 || k == 2 && l == 1 || k == 2 && l == 2 || k == 3 && l == 1)
                                    jumpchoose += 6;
                            }
                            else if (mysymbol == 'X') {
                                if (k == 6 && l == 8 || k == 7 && l == 7 || k == 7 && l == 8 || k == 8 && l == 6 || k == 8 && l == 7 || k == 8 && l == 8)
                                    jumpchoose += 6;
                            }
                        }
                    }
                }
            }
            if (state->board_data[i][j] == opsymbol) {
                for (int k = 1; k < BOARD_SIZE; k++) {
                    for (int l = 1; l < BOARD_SIZE; l++) {
                        if (CanJump(i, j, k, l, state)) {
                            opjumpchoose++;
                            if (opsymbol == 'O') {
                                if (k == 1 && l == 1 || k == 1 && l == 2 || k == 1 && l == 3 || k == 2 && l == 1 || k == 2 && l == 2 || k == 3 && l == 1)
                                    opjumpchoose += 5;
                            }
                            else if (opsymbol == 'X') {
                                if (k == 6 && l == 8 || k == 7 && l == 7 || k == 7 && l == 8 || k == 8 && l == 6 || k == 8 && l == 7 || k == 8 && l == 8)
                                    opjumpchoose += 5;
                            }
                        }
                    }
                }
            }
        }
    }
    if (status == 1) {
        score += jumpchoose * 8 - opjumpchoose * 2.5;
    }
    if (status == 2) {
        score += jumpchoose * 7 - opjumpchoose * 2.1;
    }
    if (status == 3) {
        score = 0;
    }
    return score;
}

int evaluateBegin(GameState* state, char mysymbol) {
    int score = 300;
    score += distanceScore(state, mysymbol, 1) + groupScore(state, mysymbol, 1) + jumpScore(state, mysymbol, 1);
    return score;
}

int evaluateNormal(GameState* state, char mysymbol) {
    int score = 300;
    score += distanceScore(state, mysymbol, 2) + groupScore(state, mysymbol,2) + jumpScore(state, mysymbol, 2) ;
    return score;
}

int evaluateEnd(GameState* state, char mysymbol) {
    int score = 300;
    score += distanceScore(state, mysymbol, 3) + groupScore(state, mysymbol, 3) + jumpScore(state, mysymbol, 3) ;
    return score;

}

int evaluateGameState(GameState* state, char mysymbol) {
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';
    if (state->count > 2) {
        if (isWinning(state, mysymbol))
            return INF;
        if (isWinning(state, opsymbol))
            return -INF;
    }
    int piececount=0;
    for (int i = 2; i < 8; i++) {
            for (int j = 2; j < 8; j++ ) {
                if (state->board_data[i][j] ==mysymbol )
                    piececount++;
            }
    }
    if (state->count <= 5) {
        return evaluateBegin(state, mysymbol);
    }
    if (state->count>5 && state->count <= 8) {
        return evaluateNormal(state, mysymbol);
    }
    if ( state->count > 8 ){
        return evaluateEnd(state, mysymbol);
    }
}


// 判断必胜局面的函数
int evaluateWinningPosition(GameState* state, char mysymbol) {
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';

    // 1. 直接获胜判断
    if (isWinning(state, mysymbol)) return INF;
    if (isWinning(state, opsymbol)) return -INF;

    // 2. 获取目标区域信息
    int targetArea[6][2];
    int targetPieceCount = 0;

    if (mysymbol == 'O') {
        // O的目标区域 (左上角)
        int positions[][2] = { {1,1}, {1,2}, {1,3}, {2,1}, {2,2}, {3,1} };
        memcpy(targetArea, positions, sizeof(positions));
    }
    else {
        // X的目标区域 (右下角)
        int positions[][2] = { {6,8}, {7,7}, {7,8}, {8,6}, {8,7}, {8,8} };
        memcpy(targetArea, positions, sizeof(positions));
    }

    // 3. 计算必胜局面分数
    int winningScore = evaluateWinningPosition_detailed(state, mysymbol, targetArea);

    return winningScore;
}

int evaluateWinningPosition_detailed(GameState* state, char mysymbol, int targetArea[][2]) {
    int score = 0;
    int myPieces = 0;
    int opPieces = 0;
    char opsymbol = (mysymbol == 'O') ? 'X' : 'O';

    // 1. 目标区域控制评估
    for (int i = 0; i < 6; i++) {
        int row = targetArea[i][0];
        int col = targetArea[i][1];

        if (state->board_data[row][col] == mysymbol) {
            myPieces++;
            score += 100;
        }
        /*
        else if (state->board_data[row][col] == opsymbol) {
            opPieces++;
            score -= 150;  // 对手占据目标格的惩罚更大
        }*/
    }

    // 2. 关键位置控制判断
    if (mysymbol == 'O') {
        // 判断角落位置的控制
        if (state->board_data[1][1] == mysymbol) score += 50;
        if (state->board_data[1][3] == mysymbol) score += 30;
        if (state->board_data[3][1] == mysymbol) score += 30;
    }
    else {
        if (state->board_data[8][8] == mysymbol) score += 50;
        if (state->board_data[8][6] == mysymbol) score += 30;
        if (state->board_data[6][8] == mysymbol) score += 30;
    }

    // 3. 必胜局面判断
    if (myPieces >= 4) {
        // 分析剩余棋子到达目标区域的可能性
        int remainingMoves = analyzeRemainingMoves(state, mysymbol, targetArea);
        if (remainingMoves <= 2) {
            score += 500;  // 非常接近获胜
        }
    }
    /*
    // 4. 防守局面判断
    if (opPieces >= 4) {
        score -= 1000;  // 对手接近获胜，需要优先防守
    }
    */
    return score;
}

// 分析剩余棋子到达目标区域的可能性
int analyzeRemainingMoves(GameState* state, char mysymbol, int targetArea[][2]) {
    int minMoves = INF;
    int piecesNeeded = 6;  // 获胜需要的总棋子数
    int currentPieces = 0;

    // 计算已在目标区域的棋子
    for (int i = 0; i < 6; i++) {
        if (state->board_data[targetArea[i][0]][targetArea[i][1]] == mysymbol) {
            currentPieces++;
        }
    }

    piecesNeeded -= currentPieces;

    // 找出离目标区域最近的己方棋子
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state->board_data[i][j] == mysymbol) {
                // 计算到最近目标位置的距离
                int minDistance = -INF;
                for (int k = 0; k < 6; k++) {
                    if (state->board_data[targetArea[k][0]][targetArea[k][1]] == '_') {
                        int distance = Manhattan_distance(i, j,
                            targetArea[k][0],
                            targetArea[k][1]);
                        minDistance = max(minDistance, distance);
                    }
                }
                minMoves = min(minMoves, minDistance);
            }
        }
    }

    // 考虑跳子可能性
    minMoves = (minMoves + 1) / 2;  // 跳子可以减少移动次数

    return minMoves;
}


int Manhattan_distance(int start_row, int start_col, int end_row, int end_col) {

    int x = abs(start_col - end_col);
    int y = abs(start_row - end_row);
    return x + y;
}