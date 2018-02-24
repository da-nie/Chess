#include "engine.h"
 
extern CELL Board[256];
extern COORD FigureWhiteCoord256[16];//позиции белых фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
extern COORD FigureBlackCoord256[16];//позиции чёрных фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
extern unsigned __int64 HKeyLevel[MAX_PLY+1000];//ключи позиции
extern unsigned long FirstLevel;//начальный уровень
 
 
//----------------------------------------------------------------------------------------------------
//запустить движок
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
//остановить движок
//----------------------------------------------------------------------------------------------------
void EngineStop(void)
{
 Move_Release();
 Hash_Release();
 Score_Release();
 Search_Release();
}
 
//----------------------------------------------------------------------------------------------------
//сохранить состояние
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
//загрузить состояние
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
//инициализация игры
//----------------------------------------------------------------------------------------------------
void Initialize(void)
{
 Move_StartPos();
}
//----------------------------------------------------------------------------------------------------
//поиск лучшего хода
//----------------------------------------------------------------------------------------------------
SMove Search(FIGURE_COLOR color,COORD coord256_passedpawn,long &positions,long &levels,long &score_value,long max_depth)
{ 
 return(Search_Search(color,coord256_passedpawn,positions,levels,score_value,max_depth));
}