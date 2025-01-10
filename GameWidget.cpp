#include"GameWidget.h"

void setGameMode(GameState* state, int GameMode, int FirstDrop) {
    state->gamemode = GameMode;
    state->firstdrop = FirstDrop;
}


void initializeGame(GameState* gameState, int gameMode, int firstDrop) {
    // 初始化游戏状态
    gameState->gamemode = gameMode;
    gameState->firstdrop = firstDrop;

    if (gameState->gamemode == 2) { //PVE模式
        if (gameState->firstdrop == 1) {
            gameState->AiPiece = gameState->current_piece;
        }
        else {
            gameState->AiPiece  = (gameState->current_piece == 'X') ? 'O' : 'X';
        }
    }
}



void initgame(void) {
    printf("ChineseChecker!\n");
    printf("规则\n");
    printf("向周围八个方向可以挪动一格\n");
    printf("当且仅当：起始位置与末位置连线上 只存在正中间一个棋子时，可以跳\n");
    printf("每一回合可以挪动一格，或者跳任意次\n");
    printf("输入为连续4个整数，起始行+起始列+终点行+终点列\n");
    printf("对面的角里全是棋子就算赢\n");
    printf("输入1代表PVP: 默认先手为 “ # ”\n");
    printf("输入2代表PVE：\n");
   


    int GameMode;
    scanf_s("%d", &GameMode);
    getchar();

    GameState gameState =
    {
        {
        {' ','1','2','3','4','5','6','7','8'},
        {'1','X','X','X','_','_','_','_','_'},
        {'2','X','X','_','_','_','_','_','_'},
        {'3','X','_','_','_','_','_','_','_'},
        {'4','_','_','_','_','_','_','_','_'},
        {'5','_','_','_','_','_','_','_','_'},
        {'6','_','_','_','_','_','_','_','O'},
        {'7','_','_','_','_','_','_','O','O'},
        {'8','_','_','_','_','_','O','O','O'},
        },
         {
            {1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,0,0}
        },
         'X',
        0,
        0
    };

    gameState.gamemode = GameMode;


    if (gameState.gamemode == 2)//PVE
    {
        printf("再次输入 1 代表 电脑先手\n再次输入 2 代表 你先手\n");
        int FirstDrop;
        scanf_s("%d", &FirstDrop);
        getchar();

        initializeGame(&gameState, GameMode, FirstDrop);

        print_map(&gameState);

        while (!isGameOver(&gameState)) {
            if (currentPlayerIsAI(&gameState)) {
                Move bestMove = findBestMove(&gameState);
                printf("\nAI moves from (%d,%d) to (%d,%d)\n",
                    bestMove.start_row, bestMove.start_col,
                    bestMove.end_row, bestMove.end_col);
                drop_piece(&gameState, bestMove);
                printf("AI score is : %d\n\n", bestMove.score);
                int status;
                if (gameState.count <= 6) {
                    status=1;
                }
                if (gameState.count>6 && gameState.count <= 10) {
                    status = 2;
                }
                if (gameState.count > 10) {
                    status = 3;
                }
                printf("status : %d\n", status);
                printf("distance score is : %d\n", distanceScore(&gameState, gameState.current_piece, status));
                printf("group score is : %d\n", groupScore(&gameState, gameState.current_piece, status));
                printf("jump score is : %d\n\n", jumpScore(&gameState, gameState.current_piece, status));
                updateGameState(&gameState);
                printf("\n");
            }
            else {
                int input[4];
                if (validinput(input, &gameState)) {
                    Move move_human = { 0,0,0,0,0 };
                    move_human.start_row = input[0];
                    move_human.start_col = input[1];
                    move_human.end_row = input[2];
                    move_human.end_col = input[3];
                    if (CheckValidDropl(input[0], input[1], input[2], input[3], &gameState)) {
                        printf("\nHuman moves from (%d,%d) to (%d,%d)\n",
                            move_human.start_row, move_human.start_col,
                            move_human.end_row, move_human.end_col);
                        drop_piece(&gameState, move_human);
                        
                        gameState.count++;
                        updateGameState(&gameState);
                        printf("\n");
                    }
                    else
                    {
                        printf("\nThe move violates the game rule, please input again:\n");
                    }
                }
            }
            if (CheckWin(&gameState)) {
                printWinner(&gameState);
            }
        }
    }

    else if (gameState.gamemode == 1) {
        initializeGame(&gameState, GameMode, 1);
        PVPmode(&gameState); //PVP
    }
}