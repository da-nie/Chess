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
//������� �����
//----------------------------------------------------------------------------------------------------
 
#define BYTE8(b7,b6,b5,b4,b3,b2,b1,b0) ((CELL)((b7<<7)|(b6<<6)|(b5<<5)|(b4<<4)|(b3<<3)|(b2<<2)|(b1<<1)|(b0<<0)))
 
//����� �����
#define BLACK BYTE8(0,0,1,0,0,0,0,0)
#define WHITE BYTE8(0,1,0,0,0,0,0,0)

//���� �������� ���������
#define CASTLING_O_O (BYTE8(0,0,0,1,0,0,0,0)<<8)
//���� ������� ���������
#define CASTLING_O_O_O (BYTE8(0,1,0,0,0,0,0,0)<<8)
 
//��������� ����� �����:AHIIIIEWB0MFFF
//W-������ �����
//B-������ ������
//F-��� ������ (0-������ ���)
//M-������ �� ������
//E-���� �����
//I-������ ������ � ������� ��� ������ ����� (0-15)
//H-���� �������� ��������� (���� �������� ������ � ������ � �����)
//A-���� ������� ��������� (���� �������� ������ � ������ � �����)
 
//����� �������
#define INDEX_SHIFT 8
//����� �����
#define MASK_WHITE      WHITE
//����� ������
#define MASK_BLACK      BLACK
//����� �����
#define MASK_COLOR      (MASK_WHITE|MASK_BLACK)
//����� ����
#define MASK_TYPE       BYTE8(0,0,0,0,0,1,1,1)
//����� �������
#define MASK_BORDER     BYTE8(1,0,0,0,0,0,0,0)
//�����,������ �� ������
#define MASK_IS_MOVED   BYTE8(0,0,0,0,1,0,0,0)
//����� ������� ������ � �������
#define MASK_INDEX      ((BYTE8(0,0,0,0,1,1,1,1))<<INDEX_SHIFT)
//����� ���������
#define MASK_CASTLING (BYTE8(0,0,1,1,0,0,0,0)<<8)
 
//----------------------------------------------------------------------------------------------------
//���� ������
//----------------------------------------------------------------------------------------------------
//������ �������� ������
#define CELL_FULL_VALUE 0xffffffff
//��� ������
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
//���� �����
//----------------------------------------------------------------------------------------------------
//������
#define FIGURE_TYPE_KING   1
//�����
#define FIGURE_TYPE_QUEEN  2
//�����
#define FIGURE_TYPE_ROOK   3
//����
#define FIGURE_TYPE_BISHOP 4
//����
#define FIGURE_TYPE_KNIGHT 5
//�����
#define FIGURE_TYPE_PAWN   6
 
#endif
