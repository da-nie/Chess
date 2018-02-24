#ifndef ENGINE_HISTORY_H
#define ENGINE_HISTORY_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
#include "engine_score.h"
 
using namespace std;
 
#define HISTORY_MAX (WEIGHT_INFINITY>>4)

//----------------------------------------------------------------------------------------------------
//Ходы
//----------------------------------------------------------------------------------------------------
 
//----------------------------------------------------------------------------------------------------
//Прототипы функций
//----------------------------------------------------------------------------------------------------
void History_Init(void);//инициализация
void History_Release(void);//деинициализация
void History_Reset(void);//очистить эвристику истории
void History_AddGoodMove(FIGURE_COLOR color,FIGURE_TYPE FigureType,COORD coord256_2,long depth);//добавить хороший ход
#endif