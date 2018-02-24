#ifndef ENGINE_SEARCH_H
#define ENGINE_SEARCH_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
#include "engine_score.h"
#include "engine_move.h"
#include "engine_hash.h"
#include "engine_history.h"
 
using namespace std;
 
//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void Search_Init(void);//�������������
void Search_Release(void);//���������������
ENGINE_BOOL Search_IsChangedKingSafety(SMove *sMove_CurrentPtr,FIGURE_COLOR color);//����������� ��������� ������������ ������
SMove* Search_FindMove(SMove *sMove_Ptr,SMove sMove);//����� ���� � ������ �����
long Search_AlphaBeta(long alpha,long beta,long depth,long level,FIGURE_COLOR color,COORD coord256_passedpawn,SMove sMove_Last,ENGINE_BOOL PVNode,ENGINE_BOOL NullMove);//������� ������ ������� ����
long Search_Quies(long alpha,long beta,long depth,long level,FIGURE_COLOR color,COORD coord256_passedpawn,SMove sMove_Last,ENGINE_BOOL PVNode,ENGINE_BOOL NullMove);//������� ��������� ������
SMove Search_Search(FIGURE_COLOR color,COORD coord256_passedpawn,long &positions,long &levels,long &score_value,long max_depth);//����� ������� ����
 
#endif
