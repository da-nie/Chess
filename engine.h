#ifndef ENGINE_H
#define ENGINE_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
#include "engine_move.h"
#include "engine_score.h"
#include "engine_search.h"
 
using namespace std;
 
//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void EngineRun(void);//��������� ������
void EngineStop(void);//���������� ������
void SaveState(void);//��������� ���������
void LoadState(void);//��������� ���������
void Initialize(void);//������������� ����
SMove Search(FIGURE_COLOR color,COORD coord256_passedpawn,long &positions,long &levels,long &score_value,long max_depth);//����� ������� ����
#endif
