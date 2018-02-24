#ifndef ENGINE_HASH_H
#define ENGINE_HASH_H
 
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
 
#include "engine_common.h"
#include "engine_move.h"
 
using namespace std;
 
//----------------------------------------------------------------------------------------------------
//���-�������
//----------------------------------------------------------------------------------------------------
 
//������ �������
#define MAX_HASH_TABLE_SIZE (1UL<<21UL)
//���� ���������
 
//���� ������
#define HASH_TABLE_TYPE_EMPTY 0
//������ ������
#define HASH_TABLE_TYPE_EXACT 1
//��������� �� �����
#define HASH_TABLE_TYPE_ALPHA 2
//��������� �� ����
#define HASH_TABLE_TYPE_BETA  3
//���� ��������, �� �������� ���
#define HASH_TABLE_TYPE_OLD   4
//��������� �������
struct SHTable
{
 unsigned __int64 HKey;//���-����
 long Level;//������� ����
 long HashType;//��� ����
 long Score;//������ ����
 SMove sMove;//������ ��� (���� �� ����)
};
//----------------------------------------------------------------------------------------------------
//��������� �������
//----------------------------------------------------------------------------------------------------
void Hash_Init(void);//������������� ���-�������
void Hash_Release(void);//��������������� ���-�������
inline unsigned __int64 Hash_Randomize64UI(void);//��������� 64-� ������� ���������� �����
unsigned __int64 Hash_GetHKey(void);//�������� ���-����
void Hash_RecordHash(long level,long score,long flag,unsigned __int64 hkey,SMove *sMove_Ptr);//�������� � ���-�������
 
#endif
