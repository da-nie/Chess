#include "engine.h"
 
extern CELL Board[256];
extern COORD FigureWhiteCoord256[16];//������� ����� ����� �� ����� (��� �������� ������� � �������. 0- ������ ���)
extern COORD FigureBlackCoord256[16];//������� ������ ����� �� ����� (��� �������� ������� � �������. 0- ������ ���)
extern unsigned __int64 HKeyLevel[MAX_PLY+1000];//����� �������
extern unsigned long FirstLevel;//��������� �������
 
 
//----------------------------------------------------------------------------------------------------
//��������� ������
//----------------------------------------------------------------------------------------------------
void EngineRun(void)
{
 EngineStop();
 Score_Init();
 Search_Init();
 Move_Init();
 Hash_Init();
}
//----------------------------------------------------------------------------------------------------
//���������� ������
//----------------------------------------------------------------------------------------------------
void EngineStop(void)
{
 Move_Release();
 Hash_Release();
 Score_Release();
 Search_Release();
}
 
//----------------------------------------------------------------------------------------------------
//��������� ���������
//----------------------------------------------------------------------------------------------------
void SaveState(void)
{
 FILE *file=fopen("state.txt","wb");
 if (file==NULL) return;
 fwrite(Board,sizeof(CELL),256,file);
 fwrite(FigureWhiteCoord256,sizeof(COORD),16,file);
 fwrite(FigureBlackCoord256,sizeof(COORD),16,file); 
 fwrite(&FirstLevel,sizeof(unsigned long),1,file);
 fwrite(HKeyLevel,sizeof(unsigned __int64),FirstLevel,file);
 fclose(file);
}
//----------------------------------------------------------------------------------------------------
//��������� ���������
//----------------------------------------------------------------------------------------------------
void LoadState(void)
{
 Move_StartPos();
 FILE *file=fopen("state.txt","rb");
 if (file==NULL) return;
 fread(Board,sizeof(CELL),256,file);
 fread(FigureWhiteCoord256,sizeof(COORD),16,file);
 fread(FigureBlackCoord256,sizeof(COORD),16,file);
 fread(&FirstLevel,sizeof(unsigned long),1,file);
 fread(HKeyLevel,sizeof(unsigned __int64),FirstLevel,file);
 fclose(file);
}
//---------------------------------------------------------------------------------------------------
//������������� ����
//----------------------------------------------------------------------------------------------------
void Initialize(void)
{
 Move_StartPos();
}
//----------------------------------------------------------------------------------------------------
//����� ������� ����
//----------------------------------------------------------------------------------------------------
SMove Search(FIGURE_COLOR color,COORD coord256_passedpawn,long &positions,long &levels,long &score_value,long max_depth)
{ 
 return(Search_Search(color,coord256_passedpawn,positions,levels,score_value,max_depth));
}