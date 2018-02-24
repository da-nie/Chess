#ifndef ENGINE_MOVE_H
#define ENGINE_MOVE_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
 
using namespace std;
 
//----------------------------------------------------------------------------------------------------
//Ходы
//----------------------------------------------------------------------------------------------------
 
//Типы ходов
enum MOVE_TYPE
{
 MOVE_TYPE_EMPTY=-1,//хода нет
 MOVE_TYPE_SIMPLY=0,//простой ход
 MOVE_TYPE_CASTLING=1,//рокировка
 MOVE_TYPE_WHITE_PASSED_PAWN_EAT=2,//взятие проходной пешки
 MOVE_TYPE_BLACK_PASSED_PAWN_EAT=3,//взятие проходной пешки
 MOVE_TYPE_CONVERSION=4,//превращение пешки
};
 
//ход фигурой
struct SMove
{
 //начальная позиция
 COORD Coord256_1;
 //конечная позиция
 COORD Coord256_2;  
 MOVE_TYPE MoveType;//тип хода
 FIGURE_TYPE NewFigureType;//новый тип фигуры, если она получилась из пешки
 COORD Coord256_PassedPawn;//ход проходной пешкой (если он есть. 0- проходной пешки нет)
 ENGINE_BOOL IsEat;//ход-взятие
 //изменение веса хода (используем для сортировки ходов)
 long Score;
 //указатель на следующий элемент
 SMove *sMove_NextPtr;
};
//комплект ходов
struct SMoveKit
{
 SMove sMove_Hash;//ход в хэш-таблице
 SMove sMove_Killer;//ход killer
 SMove sMove_Last;//прошлый ход
 SMove sMove_PrincipalVariation;//ход из основной линии
};
 
//----------------------------------------------------------------------------------------------------
//Прототипы функций
//----------------------------------------------------------------------------------------------------
void Move_Init(void);//инициализация
void Move_Release(void);//деинициализация
void Move_StartPos(void);//установка фигур в исходное состояние
void Move_SetFigureState(CELL index,FIGURE_TYPE FigureType,ENGINE_BOOL ismoved,char x,char y,FIGURE_COLOR color);//задание параметров фигуры
inline ENGINE_BOOL Move_TestMove(COORD coord256,FIGURE_COLOR color,ENGINE_BOOL &eat,ENGINE_BOOL &check);//определение, можно ли ходить на данную клетку
inline void Move_AddMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_Ptr,long &move,SMove* &sMove_FirstPtr,SMoveKit *sMoveKitPtr);//добавить ход без взятия
inline void Move_AddEatMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_EatPtr,long &move_eat,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//добавить ход со взятием
inline ENGINE_BOOL Move_IsCheck(COORD coord256);//определение шахов королю
void Move_CreateMove(COORD coord256,SMove *sMove_Ptr,long &move,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstPtr,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//создать ходы для фигуры
void Move_CreateEatMove(COORD coord256,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//определение всех возможных ходов фигуры со взятием
ENGINE_BOOL Move_CheckEatKing(FIGURE_COLOR color);//проверить не съели ли короля
inline ENGINE_BOOL Move_CheckKing(FIGURE_COLOR color);//проверить шах королю
inline CELL Move_MoveFigure(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,FIGURE_TYPE &old_figure_type,CELL &ismoved);//переместить фигуру с возможным съедением и вернуть съеденую фигуру
inline void Move_UnMoveFigure(COORD coord256_1,COORD coord256_2,CELL ismoved,FIGURE_TYPE old_figure_type,CELL eat_b);//переместить фигуру обратно с восстановлением съеденой фигуры
void Move_LegalMoveOff(SMove sMove,FIGURE_TYPE old_figure_type,CELL ismoved,CELL eat_b);//отменить ход, сделаный по правилам
ENGINE_BOOL Move_LegalMoveOn(SMove sMove,FIGURE_TYPE &old_figure_type,CELL &ismoved,CELL &eat_b,COORD &coord256_passedpawn);//сделать ход по правилам, вернуть допустим ли ход и установить указатели на проходную пешку и съеденную фигуру
ENGINE_BOOL Move_MoveOn(FIGURE_COLOR color,COORD x1,COORD y1,COORD x2,COORD y2,COORD &coord256_passedpawn,FIGURE_TYPE NewFigureType);//сделать ход
SMove* Move_SortMove(SMove *sMove_FirstPtr,SMove *sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//сортировка ходов
#endif
