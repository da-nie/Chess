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
//����
//----------------------------------------------------------------------------------------------------
 
//���� �����
enum MOVE_TYPE
{
 MOVE_TYPE_EMPTY=-1,//���� ���
 MOVE_TYPE_SIMPLY=0,//������� ���
 MOVE_TYPE_CASTLING=1,//���������
 MOVE_TYPE_WHITE_PASSED_PAWN_EAT=2,//������ ��������� �����
 MOVE_TYPE_BLACK_PASSED_PAWN_EAT=3,//������ ��������� �����
 MOVE_TYPE_CONVERSION=4,//����������� �����
};
 
//��� �������
struct SMove
{
 //��������� �������
 COORD Coord256_1;
 //�������� �������
 COORD Coord256_2;  
 MOVE_TYPE MoveType;//��� ����
 FIGURE_TYPE NewFigureType;//����� ��� ������, ���� ��� ���������� �� �����
 COORD Coord256_PassedPawn;//��� ��������� ������ (���� �� ����. 0- ��������� ����� ���)
 ENGINE_BOOL IsEat;//���-������
 //��������� ���� ���� (���������� ��� ���������� �����)
 long Score;
 //��������� �� ��������� �������
 SMove *sMove_NextPtr;
};
//�������� �����
struct SMoveKit
{
 SMove sMove_Hash;//��� � ���-�������
 SMove sMove_Killer;//��� killer
 SMove sMove_Last;//������� ���
 SMove sMove_PrincipalVariation;//��� �� �������� �����
};
 
//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void Move_Init(void);//�������������
void Move_Release(void);//���������������
void Move_StartPos(void);//��������� ����� � �������� ���������
void Move_SetFigureState(CELL index,FIGURE_TYPE FigureType,ENGINE_BOOL ismoved,char x,char y,FIGURE_COLOR color);//������� ���������� ������
inline ENGINE_BOOL Move_TestMove(COORD coord256,FIGURE_COLOR color,ENGINE_BOOL &eat,ENGINE_BOOL &check);//�����������, ����� �� ������ �� ������ ������
inline void Move_AddMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_Ptr,long &move,SMove* &sMove_FirstPtr,SMoveKit *sMoveKitPtr);//�������� ��� ��� ������
inline void Move_AddEatMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_EatPtr,long &move_eat,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//�������� ��� �� �������
inline ENGINE_BOOL Move_IsCheck(COORD coord256);//����������� ����� ������
void Move_CreateMove(COORD coord256,SMove *sMove_Ptr,long &move,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstPtr,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//������� ���� ��� ������
void Move_CreateEatMove(COORD coord256,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//����������� ���� ��������� ����� ������ �� �������
ENGINE_BOOL Move_CheckEatKing(FIGURE_COLOR color);//��������� �� ����� �� ������
inline ENGINE_BOOL Move_CheckKing(FIGURE_COLOR color);//��������� ��� ������
inline CELL Move_MoveFigure(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,FIGURE_TYPE &old_figure_type,CELL &ismoved);//����������� ������ � ��������� ��������� � ������� �������� ������
inline void Move_UnMoveFigure(COORD coord256_1,COORD coord256_2,CELL ismoved,FIGURE_TYPE old_figure_type,CELL eat_b);//����������� ������ ������� � ��������������� �������� ������
void Move_LegalMoveOff(SMove sMove,FIGURE_TYPE old_figure_type,CELL ismoved,CELL eat_b);//�������� ���, �������� �� ��������
ENGINE_BOOL Move_LegalMoveOn(SMove sMove,FIGURE_TYPE &old_figure_type,CELL &ismoved,CELL &eat_b,COORD &coord256_passedpawn);//������� ��� �� ��������, ������� �������� �� ��� � ���������� ��������� �� ��������� ����� � ��������� ������
ENGINE_BOOL Move_MoveOn(FIGURE_COLOR color,COORD x1,COORD y1,COORD x2,COORD y2,COORD &coord256_passedpawn,FIGURE_TYPE NewFigureType);//������� ���
SMove* Move_SortMove(SMove *sMove_FirstPtr,SMove *sMove_FirstEatPtr,SMoveKit *sMoveKitPtr);//���������� �����
#endif
