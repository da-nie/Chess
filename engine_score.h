#ifndef ENGINE_SCORE_H
#define ENGINE_SCORE_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
 
using namespace std;
 
//----------------------------------------------------------------------------------------------------
//Веса фигур
//----------------------------------------------------------------------------------------------------
#define WEIGHT_PAWN     (100)
#define WEIGHT_BISHOP   (423)
#define WEIGHT_KNIGHT   (423)
#define WEIGHT_ROOK     (650)
#define WEIGHT_QUEEN    (1268)
#define WEIGHT_INFINITY (WEIGHT_QUEEN*1000)
#define WEIGHT_KING     WEIGHT_INFINITY
 
//атака фигур
#define CELL_EAT long

#define CELL_EAT_NONE         0
#define CELL_EAT_MASK_PAWN    (1<<0)
#define CELL_EAT_MASK_BISHOP  (1<<1)
#define CELL_EAT_MASK_KNIGHT  (1<<2)
#define CELL_EAT_MASK_ROOK    (1<<3)
#define CELL_EAT_MASK_QUEEN   (1<<4)
#define CELL_EAT_MASK_KING    (1<<5) 


//структура для оценки позиции
struct SEvaluate
{
 //белые
 long W_WeightFromType[FIGURE_TYPE_PAWN+1];//оценка фигур
 long W_MoveFromType[FIGURE_TYPE_PAWN+1];//количество ходов белых
 long W_EatFromType[FIGURE_TYPE_PAWN+1];//количество ходов взятия белых
 CELL_EAT W_EatBoard[256];//поле атак белых
 COORD W_Coord256MoveFromIndex[16][64];//координаты ходов белых фигур (кроме пешек)
 long W_MoveAmountFromIndex[16];//общее количество ходов белых фигур для всех фигур, кроме пешек для каждой фигуры
  
 //чёрные
 long B_WeightFromType[FIGURE_TYPE_PAWN+1];//оценка фигур
 long B_MoveFromType[FIGURE_TYPE_PAWN+1];//количество ходов чёрных
 long B_EatFromType[FIGURE_TYPE_PAWN+1];//количество ходов взятия чёрных
 CELL_EAT B_EatBoard[256];//поле атак чёрных
 COORD B_Coord256MoveFromIndex[16][64];//координаты ходов чёрных фигур (кроме пешек)
 long B_MoveAmountFromIndex[16];//общее количество ходов чёрных фигур, кроме пешек для каждой фигуры
};
 
//----------------------------------------------------------------------------------------------------
//Прототипы функций
//----------------------------------------------------------------------------------------------------
void Score_Init(void);//инициализация
void Score_Release(void);//деинициализация
inline void Score_CreateFigureSpace(COORD coord256);//создать поле атак фигуры и определить количество ходов
float Score_GetMaterialScoreAndCreateSpace(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//подсчёт материала и заполнение массивов ходов, определение фазы игры
void Score_GetPositionScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//подсчёт оценок за позицию
void Score_GetSpaceScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//бонусы за захват пространства
void Score_GetKingScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//бонусы за параметры короля
void Score_GetPawnScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//бонусы за параметры пешек
long Score_Evaluate(FIGURE_COLOR color);//оценка позиции
#endif
