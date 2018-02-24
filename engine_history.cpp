#include "engine_history.h"
 
//���������� ������ ����� ��� �����
long WhiteHistory[FIGURE_TYPE_PAWN+1][256];
//���������� ������ ����� ��� ������
long BlackHistory[FIGURE_TYPE_PAWN+1][256];

//----------------------------------------------------------------------------------------------------
//�������������
//----------------------------------------------------------------------------------------------------
void History_Init(void)
{
 History_Reset();
}
//----------------------------------------------------------------------------------------------------
//���������������
//----------------------------------------------------------------------------------------------------
void History_Release(void)
{
}
//----------------------------------------------------------------------------------------------------
//�������� ��������� �������
//----------------------------------------------------------------------------------------------------
void History_Reset(void)
{
 for(long t=0;t<=FIGURE_TYPE_PAWN;t++)
 {
  for(long h=0;h<256;h++)
  {
   WhiteHistory[t][h]=0;
   BlackHistory[t][h]=0;
  }
 }
}
//----------------------------------------------------------------------------------------------------
//�������� ������� ���
//----------------------------------------------------------------------------------------------------
void History_AddGoodMove(FIGURE_COLOR color,FIGURE_TYPE FigureType,COORD coord256_2,long depth)
{
 if (color==WHITE)
 {
  WhiteHistory[FigureType][coord256_2]+=depth*depth;
  if (WhiteHistory[FigureType][coord256_2]>=HISTORY_MAX)
  {
   //��������� ��� �������� � 2 ����
   for(long t=0;t<=FIGURE_TYPE_PAWN;t++)
   {
    for(long h=0;h<256;h++)
	{
	 WhiteHistory[t][h]/=2;
	}		
   }
  }
 }
 else
 {
  BlackHistory[FigureType][coord256_2]+=depth*depth;
  if (BlackHistory[FigureType][coord256_2]>=HISTORY_MAX)
  {
   //��������� ��� �������� � 2 ����
   for(long t=0;t<=FIGURE_TYPE_PAWN;t++)
   {
    for(long h=0;h<256;h++)
	{
	 BlackHistory[t][h]/=2;
	}
   }
  }
 }
}
