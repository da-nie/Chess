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
//���� �����
//----------------------------------------------------------------------------------------------------
#define WEIGHT_PAWN     (100)
#define WEIGHT_BISHOP   (423)
#define WEIGHT_KNIGHT   (423)
#define WEIGHT_ROOK     (650)
#define WEIGHT_QUEEN    (1268)
#define WEIGHT_INFINITY (WEIGHT_QUEEN*1000)
#define WEIGHT_KING     WEIGHT_INFINITY
 
//����� �����
#define CELL_EAT long

#define CELL_EAT_NONE         0
#define CELL_EAT_MASK_PAWN    (1<<0)
#define CELL_EAT_MASK_BISHOP  (1<<1)
#define CELL_EAT_MASK_KNIGHT  (1<<2)
#define CELL_EAT_MASK_ROOK    (1<<3)
#define CELL_EAT_MASK_QUEEN   (1<<4)
#define CELL_EAT_MASK_KING    (1<<5) 


//��������� ��� ������ �������
struct SEvaluate
{
 //�����
 long W_WeightFromType[FIGURE_TYPE_PAWN+1];//������ �����
 long W_MoveFromType[FIGURE_TYPE_PAWN+1];//���������� ����� �����
 long W_EatFromType[FIGURE_TYPE_PAWN+1];//���������� ����� ������ �����
 CELL_EAT W_EatBoard[256];//���� ���� �����
 COORD W_Coord256MoveFromIndex[16][64];//���������� ����� ����� ����� (����� �����)
 long W_MoveAmountFromIndex[16];//����� ���������� ����� ����� ����� ��� ���� �����, ����� ����� ��� ������ ������
  
 //������
 long B_WeightFromType[FIGURE_TYPE_PAWN+1];//������ �����
 long B_MoveFromType[FIGURE_TYPE_PAWN+1];//���������� ����� ������
 long B_EatFromType[FIGURE_TYPE_PAWN+1];//���������� ����� ������ ������
 CELL_EAT B_EatBoard[256];//���� ���� ������
 COORD B_Coord256MoveFromIndex[16][64];//���������� ����� ������ ����� (����� �����)
 long B_MoveAmountFromIndex[16];//����� ���������� ����� ������ �����, ����� ����� ��� ������ ������
};
 
//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void Score_Init(void);//�������������
void Score_Release(void);//���������������
inline void Score_CreateFigureSpace(COORD coord256);//������� ���� ���� ������ � ���������� ���������� �����
float Score_GetMaterialScoreAndCreateSpace(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//������� ��������� � ���������� �������� �����, ����������� ���� ����
void Score_GetPositionScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//������� ������ �� �������
void Score_GetSpaceScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//������ �� ������ ������������
void Score_GetKingScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//������ �� ��������� ������
void Score_GetPawnScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score);//������ �� ��������� �����
long Score_Evaluate(FIGURE_COLOR color);//������ �������
#endif
