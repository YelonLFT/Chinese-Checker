#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include"BoardWidget.h"
#include"AIChesser.h"

void setGameMode(GameState* state, int GameMode, int FirstDrop);

void initializeGame(GameState* gameState, int gameMode, int firstDrop);

void initgame(void);

#endif