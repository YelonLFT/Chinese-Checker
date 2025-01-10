
#include"BoardWidget.h"




int max(int a, int b) {
    return (a > b) ? a : b;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}


void drop_piece(GameState* state, Move move) {


    char piece = state->board_data[move.start_row][move.start_col];
    
    state->board_data[move.end_row][move.end_col] = state->board_data[move.start_row][move.start_col];
    state->board_data[move.start_row][move.start_col] = '_';
}

void de_drop_piece(GameState* state, Move move) {

    char piece = state->board_data[move.end_row][move.end_col];
    
    state->board_data[move.start_row][move.start_col] = state->board_data[move.end_row][move.end_col];
    state->board_data[move.end_row][move.end_col] = '_';
}

bool isAJump(int y1, int x1, int y2, int x2, GameState* state)
{
    int mid_y = (y1 + y2) / 2;
    int mid_x = (x1 + x2) / 2;

    int ifmid_y = (y1 + y2) % 2;
    int ifmid_x = (x1 + x2) % 2;

    if (state->board_data[y2][x2] != '_')
        return false;
    else
    {
        if (abs(y2 - y1) <= 1 && abs(x2 - x1) <= 1)/*在相邻的八个格子里走*/
        {
            return false;
        }
        else if (ifmid_y != 0 || ifmid_x != 0)/*没走相邻的，但也没跳成功（没有中间点）*/
        {
            return false;
        }
        else/*有中间点的跳着走*/
        {
            if (y2 != y1 && x1 != x2)  /*斜着跳*/
            {
                if (abs(y2 - y1) != abs(x2 - x1))  /*没有沿着对角线跳*/
                    return false;
                else  /*沿着对角线 能不能跳*/
                {
                    if (state->board_data[mid_y][mid_x] != '_')
                    {
                        if (abs(y2 - y1) >= 4)   /*不是紧挨着跳的*/
                        {
                            for (int i = 1; i < (abs(y2 - y1) / 2); i++) {
                                if (state->board_data[mid_y - i][mid_x - i] != '_' || state->board_data[mid_y + i][mid_x + i] != '_')
                                    return false;

                            }return true;
                        }
                        else/*紧挨着跳*/
                        {
                            return true;
                        }
                    }
                    else
                        return false;
                }
            }
            else if (y2 == y1 || x1 == x2)
            {
                if (state->board_data[mid_y][mid_x] != '_')
                {
                    if (y1 == y2)
                    {
                        if (abs(x1 - x2) >= 4) {
                            for (int i = 1; i < (abs(x1 - x2) / 2); i++) {
                                if (state->board_data[y1][mid_x + i] != '_' || state->board_data[y1][mid_x - i] != '_')
                                    return false;
                            }return true;
                        }
                        else
                            return true;
                    }
                    if (x1 == x2)
                    {
                        if (abs(y1 - y2) >= 4) {
                            for (int i = 1; i < (abs(y1 - y2) / 2); i++) {
                                if (state->board_data[mid_y - i][x1] != '_' || state->board_data[mid_y + i][x1] != '_')
                                    return false;
                            }return true;
                        }
                        else
                            return true;
                    }
                }
                else
                    return false;
            }
        }
    }
    return false;
}

bool isAMove(int y1, int x1, int y2, int x2, GameState* state)
{
    if (state->board_data[y2][x2] != '_')
        return false;
    else
    {
        if (abs(y2 - y1) <= 1 && abs(x2 - x1) <= 1) /*在相邻的八个格子里走*/
        {
            return true;
        }
        else
            return false;
    }
}


bool CanJump(int y1, int x1, int y2, int x2, GameState *gameState) 
{

   // static bool visited[BOARD_SIZE][BOARD_SIZE];
   // memset(visited, 0, sizeof(visited));

    if (isAJump(y1, x1, y2, x2, gameState)) {
        return true;
    }
    for (int i = 1; i < 9; i++) {
        for (int j = 1; j < 9; j++) {
            if (isAJump(y1, x1, i, j, gameState) && gameState->jump_memo[i][j] == 0) {
                //visited[i][j] = true;
                gameState->jump_memo[y1][x1] = 1;
                if (CanJump(i, j, y2, x2, gameState)) {
                    gameState->jump_memo[y1][x1] = 0;
                    return true;
                }
            }
        }
    }
    return false;
}


bool CheckValidDropl(int y1, int x1, int y2, int x2, GameState * gameState)
{
    resetJumpMemo(gameState);
    if (y1 < 0 || y1 >= BOARD_SIZE || x1 < 0 || x1 >= BOARD_SIZE ||
        y2 < 0 || y2 >= BOARD_SIZE || x2 < 0 || x2 >= BOARD_SIZE) {
        return false;
    }
    if (isAMove(y1, x1, y2, x2, gameState) || CanJump(y1, x1, y2, x2, gameState))
        return true;
    else
        return false;
}

bool validinput(int input[4], GameState* board)
{
    char inputl[10] ; // Extra space for null terminator

    if (fgets(inputl, sizeof(inputl), stdin))// Limit input to 4 characters    
    {
        /*if (sscanf_s(inputl, "%1d%1d%1d%1d", &input[0], &input[1], &input[2], &input[3]) != 4) {
            printf("Invalid input format, please input again:\n");
            return false;
        }*/
        int len = strlen(inputl);
        if (len < 4) {
            printf("Input too short, please input again:\n");
            return false;
        }
        for (int i = 0; i < 4; i++) {
            if (isdigit(inputl[i])) {
                input[i] = inputl[i] - '0'; // Convert char to int
            }
            else {
                printf("Invalid input format, please input again:\n");
                return false;
            }
        }
    }

    if (input[0] < 1 || input[0]>8 || input[1] < 1 || input[1]>8 ||
        input[2] < 1 || input[2]>8 || input[3] < 1 || input[3]>8) {
        printf("Input out of the game board, please input again:\n");
        return false;
    }

    else if (board->board_data[input[0]][input[1]] != board->current_piece) {
        printf("Invalid starting location, please input again:\n");
        return false;
    }
    else if (board->board_data[input[2]][input[3]] != '_') {
        printf("Invalid ending location, please input again:\n");
        return false;
    }
    return true;
}

bool CheckWin(GameState* board)
{
    if (board->count > 3)
    {
        if (board->board_data[6][8] != '_' && board->board_data[7][7] != '_' && board->board_data[7][8] != '_' && board->board_data[8][6] != '_' && board->board_data[8][7] != '_' && board->board_data[8][8] != '_') {
            
            return true;
        }
        if (board->board_data[1][1] != '_' && board->board_data[1][2] != '_' && board->board_data[1][3] != '_' && board->board_data[2][1] != '_' && board->board_data[2][2] != '_' && board->board_data[3][1] != '_') {
            
            return true;
        }
    }
    return false;
}

void printWinner(GameState* board) {
    if (CheckWin(board)) {
        if (board->board_data[6][8] != '_' && board->board_data[7][7] != '_' && board->board_data[7][8] != '_' && board->board_data[8][6] != '_' && board->board_data[8][7] != '_' && board->board_data[8][8] != '_') {
            printf("# Wins!\n");
        }
        if (board->board_data[1][1] != '_' && board->board_data[1][2] != '_' && board->board_data[1][3] != '_' && board->board_data[2][1] != '_' && board->board_data[2][2] != '_' && board->board_data[3][1] != '_') {
            printf("O Wins!\n");
        }
    }
    else
        return;
}

bool isWinning(GameState* board, char symbol) {

    if (symbol == 'X') {
        if (board->board_data[6][8] != '_' && board->board_data[7][7] != '_' && board->board_data[7][8] != '_' && board->board_data[8][6] != '_' && board->board_data[8][7] != '_' && board->board_data[8][8] != '_') {
            return true;
        }
    }
    if (symbol == 'O') {
        if (board->board_data[1][1] != '_' && board->board_data[1][2] != '_' && board->board_data[1][3] != '_' && board->board_data[2][1] != '_' && board->board_data[2][2] != '_' && board->board_data[3][1] != '_') {
            return true;
        }
    }

    return false;
}

bool isGameOver(GameState* gameState) 
{
    if (gameState->count > 2) {
        if (CheckWin(gameState))
            return true;
        if (gameState->count >= 100)
            return true;
    }
    return false;
}

void print_map(GameState* board) 
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board->board_data[i][j] == 'X')
                printf("# ");
            else if (board->board_data[i][j] == '_')
                printf(". ");
            else if (board->board_data[i][j] == 'O')
                printf("0 ");
            else
                printf("%c ", board->board_data[i][j]);
        }
        printf("\n");
    }
}

void updateGameState(GameState* state) 
{
    print_map(state);
    state->current_piece = (state->current_piece == 'O') ? 'X' : 'O';

    resetJumpMemo(state);
}

void PVPmode(GameState* gameState) {
    print_map(gameState);
    while (!isGameOver(gameState))
    {
        int input[4];
        if (validinput(input, gameState)) {
            Move move_human = { 0,0,0,0 };
            move_human.start_row = input[0];
            move_human.start_col = input[1];
            move_human.end_row = input[2];
            move_human.end_col = input[3];
            if (CheckValidDropl(input[0], input[1], input[2], input[3], gameState)) {
                drop_piece(gameState, move_human);
                updateGameState(gameState);
                gameState->count++;
            }
            else
            {
                printf("The move violates the game rule, please input again:\n");
            }
        }
        if (gameState->count >= 100)
        {
            printf("Draw\n");
            return;
        }

        if (CheckWin(gameState) == true)
        {
            printWinner(gameState);
            return;
        }
    }
}
