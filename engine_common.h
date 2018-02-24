#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#define MAX_DEPTH 8
#define MAX_LEVEL 30
#define MAX_PLY   100

#define FUTILITY_DEPTH 4
#define NULL_MOVE_DEPTH 4
#define VERIFICATOIN_SEARCH_REDUCTION 2

#define MAX_MOVES 10000
 
//----------------------------------------------------------------------------------------------------
//Ѕитовые маски
//----------------------------------------------------------------------------------------------------
 
#define BYTE8(b7,b6,b5,b4,b3,b2,b1,b0) ((CELL)((b7<<7)|(b6<<6)|(b5<<5)|(b4<<4)|(b3<<3)|(b2<<2)|(b1<<1)|(b0<<0)))
 
//цвета фигур
#define BLACK BYTE8(0,0,1,0,0,0,0,0)
#define WHITE BYTE8(0,1,0,0,0,0,0,0)

//флаг короткой рокировки
#define CASTLING_O_O (BYTE8(0,0,0,1,0,0,0,0)<<8)
//флаг длинной рокировки
#define CASTLING_O_O_O (BYTE8(0,1,0,0,0,0,0,0)<<8)
 
//структура полей доски:AHIIIIEWB0MFFF
//W-фигура бела€
//B-фигура чЄрна€
//F-тип фигуры (0-фигуры нет)
//M-ходила ли фигура
//E-край доски
//I-индекс фигуры в массиве дл€ поиска фигур (0-15)
//H-была коротка€ рокировка (флаг ставитс€ только у корол€ и ладьи)
//A-была длинна€ рокировка (флаг ставитс€ только у корол€ и ладьи)
 
//сдвиг индекса
#define INDEX_SHIFT 8
//маска белых
#define MASK_WHITE      WHITE
//маска чЄрных
#define MASK_BLACK      BLACK
//маска цвета
#define MASK_COLOR      (MASK_WHITE|MASK_BLACK)
//маска типа
#define MASK_TYPE       BYTE8(0,0,0,0,0,1,1,1)
//маска границы
#define MASK_BORDER     BYTE8(1,0,0,0,0,0,0,0)
//маска,ходила ли фигура
#define MASK_IS_MOVED   BYTE8(0,0,0,0,1,0,0,0)
//маска индекса фигуры в массиве
#define MASK_INDEX      ((BYTE8(0,0,0,0,1,1,1,1))<<INDEX_SHIFT)
//маска рокировки
#define MASK_CASTLING (BYTE8(0,0,1,1,0,0,0,0)<<8)
 
//----------------------------------------------------------------------------------------------------
//“ипы данных
//----------------------------------------------------------------------------------------------------
//полное значение €чейки
#define CELL_FULL_VALUE 0xffffffff
//тип фигуры
#define FIGURE_TYPE  long
#define FIGURE_COLOR long
#define COORD        long
#define CELL         long
#define ENGINE_BOOL  long
#define ZOBRIST_COLOR long
#define ENGINE_TRUE  1L
#define ENGINE_FALSE 0L
#define ZOBRIST_BLACK 0
#define ZOBRIST_WHITE 1
 
#define CELL_EMPTY  0 
//----------------------------------------------------------------------------------------------------
//“ипы фигур
//----------------------------------------------------------------------------------------------------
//король
#define FIGURE_TYPE_KING   1
//ферзь
#define FIGURE_TYPE_QUEEN  2
//ладь€
#define FIGURE_TYPE_ROOK   3
//слон
#define FIGURE_TYPE_BISHOP 4
//конь
#define FIGURE_TYPE_KNIGHT 5
//пешка
#define FIGURE_TYPE_PAWN   6
 
#endif
