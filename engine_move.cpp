#include "engine_move.h"
#include "engine_hash.h"
#include "engine_score.h"
#include "engine_history.h"
 
//статистика оценки ходов для белых
extern long WhiteHistory[FIGURE_TYPE_PAWN+1][256];
//статистика оценки ходов для чёрных
extern long BlackHistory[FIGURE_TYPE_PAWN+1][256];

SMove sMove_Level[MAX_PLY+5][200];//ходы фигурой
SMove sMove_EatLevel[MAX_PLY+5][200];//ходы фигурой со взятием
extern CELL Board[256];//шахматная доска с полем посередине (16x16)
 
COORD FigureWhiteCoord256[16];//позиции белых фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
COORD FigureBlackCoord256[16];//позиции чёрных фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
COORD *KingWhitePointer=&FigureWhiteCoord256[0];//указатель на короля в массиве позиций белых
COORD *KingBlackPointer=&FigureBlackCoord256[0];//указатель на короля в массиве позиций чёрных

unsigned __int64 HKeyLevel[MAX_MOVES];//ключи позиции
unsigned long FirstLevel=0;//начальный уровень
 
extern unsigned __int64 ZobristKey[FIGURE_TYPE_PAWN+1][2][256];//[тип фигуры][цвет фигуры][позиция фигуры на доске 16x16]
extern unsigned __int64 HKey;//хэш позиции
extern unsigned __int64 ZobristKeyMove;//ключ смены хода

//----------------------------------------------------------------------------------------------------
//ходы фигур
//----------------------------------------------------------------------------------------------------
long KingMove[9]={16,-16,1,-1,17,-17,15,-15,0};//ходы короля
long QueenMove[9]={16,-16,1,-1,17,-17,15,-15,0};//ходы ферзя
long RookMove[5]={16,-16,1,-1,0};//ходы ладьи
long BishopMove[5]={17,-17,15,-15,0};//ходы слона
long KnightMove[9]={32+1,32-1,16+2,16-2,-(32+1),-(32-1),-(16+2),-(16-2),0};//ходы коня
//----------------------------------------------------------------------------------------------------
//таблицы перекодировки координат
//----------------------------------------------------------------------------------------------------
COORD Coord256to64[256];
COORD Coord64to256[64];
 
//----------------------------------------------------------------------------------------------------
//инициализация
//----------------------------------------------------------------------------------------------------
void Move_Init(void)
{
 long x;
 long y;
 Move_Release();
 //создаём массивы перевода координат
 for(long n=0;n<256;n++) Coord256to64[n]=0;
 for(y=0;y<8;y++)
 {
  for(x=0;x<8;x++)
  {
   COORD coord64=(y<<3)|x;
   COORD coord256=((y+4)<<4)|(x+4);
   Coord64to256[coord64]=coord256;
   Coord256to64[coord256]=coord64;
  }
 }
}
//----------------------------------------------------------------------------------------------------
//деинициализация
//----------------------------------------------------------------------------------------------------
void Move_Release(void)
{
}
 
//---------------------------------------------------------------------------------------------------
//установка фигур в исходное состояние
//----------------------------------------------------------------------------------------------------
void Move_StartPos(void)
{
 long n;
 //очищаем доску
 for(n=0;n<256;n++) Board[n]=BYTE8(1,0,0,0,0,0,0,0);
 //заполняем центр доски
 for(n=0;n<64;n++) Board[Coord64to256[n]]=CELL_EMPTY;
 //задаём все фигуры
 for(n=0;n<16;n++)
 {
  FigureWhiteCoord256[n]=0;
  FigureBlackCoord256[n]=0;
 }
 //расставляем фигуры  
 //белые
 Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'E','1',WHITE);
 Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'D','1',WHITE);
 Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A','1',WHITE);
 Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'H','1',WHITE);
 Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'C','1',WHITE);
 Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'F','1',WHITE);
 Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'B','1',WHITE);
 Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'G','1',WHITE);
 for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+static_cast<char>(n),'2',WHITE);
  
 //чёрные
 Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'E','8',BLACK);
 Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'D','8',BLACK);
 Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A','8',BLACK);
 Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'H','8',BLACK);
 Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'C','8',BLACK);
 Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'F','8',BLACK);
 Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'B','8',BLACK);
 Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'G','8',BLACK);
 for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+static_cast<char>(n),'7',BLACK);

 KingWhitePointer=&FigureWhiteCoord256[0];//указатель на короля в массиве позиций белых
 KingBlackPointer=&FigureBlackCoord256[0];//указатель на короля в массиве позиций чёрных

 FirstLevel=0;
 for(n=0;n<MAX_MOVES;n++) HKeyLevel[n]=n;//инициализируем ключи позиций возрастающими значениями
}
//----------------------------------------------------------------------------------------------------
//задание параметров фигуры
//----------------------------------------------------------------------------------------------------
void Move_SetFigureState(CELL index,FIGURE_TYPE FigureType,ENGINE_BOOL ismoved,char x,char y,FIGURE_COLOR color)
{
 x=x-'A';
 y=y-'1';
 CELL state=color|FigureType|(index<<INDEX_SHIFT);
 if (ismoved==ENGINE_TRUE) state|=MASK_IS_MOVED;
 COORD coord256=Coord64to256[(y<<3)|x];
 Board[coord256]=state;
 if (color==WHITE) FigureWhiteCoord256[index]=coord256;
 if (color==BLACK) FigureBlackCoord256[index]=coord256;
}
 
//----------------------------------------------------------------------------------------------------
//добавить ход без взятия
//----------------------------------------------------------------------------------------------------
void Move_AddMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_Ptr,long &move,SMove* &sMove_FirstPtr,SMoveKit *sMoveKitPtr)
{
 CELL cell=Board[coord256_1];
 FIGURE_TYPE type=cell&MASK_TYPE;//тип ходившей фигуры
 FIGURE_COLOR color=cell&MASK_COLOR;//цвет ходившей фигуры
 //узнаем изменение стоимости позиции фигуры (без знака)
 long score=0;
 //добавляем оценку из эвристики истории
 if (color==WHITE) score+=WhiteHistory[type][coord256_2];
              else score+=BlackHistory[type][coord256_2]; 
 if (sMoveKitPtr->sMove_PrincipalVariation.Coord256_2==coord256_2 && sMoveKitPtr->sMove_PrincipalVariation.Coord256_1==coord256_1) score+=WEIGHT_INFINITY;//продвигаем ход из главной линии 
 if (movetype!=MOVE_TYPE_SIMPLY) score+=WEIGHT_INFINITY>>1;//продвигаем необычный ход

 SMove *sMove_SelectPtr=&sMove_Ptr[move];//ход всегда ставим в конец
 //сортируем ходы по возрастанию score
 SMove *sMove_MorePtr=NULL;//больший элемент
 SMove *sMove_LessPtr=sMove_FirstPtr;//меньший элемент
 while (sMove_LessPtr!=NULL)
 {
  if (sMove_LessPtr->Score<score) break;
  sMove_MorePtr=sMove_LessPtr;
  sMove_LessPtr=sMove_LessPtr->sMove_NextPtr;
 }
 sMove_SelectPtr->sMove_NextPtr=sMove_LessPtr;//задаём меньший элемент
 //указываем на элемент, оценка которого ниже вставляемого
 if (sMove_MorePtr==NULL) sMove_FirstPtr=sMove_SelectPtr;//вставляемый элемент будет первым
 else  sMove_MorePtr->sMove_NextPtr=sMove_SelectPtr;
 move++;
 sMove_SelectPtr->IsEat=ENGINE_FALSE;
 sMove_SelectPtr->Score=score;
 sMove_SelectPtr->Coord256_1=coord256_1;
 sMove_SelectPtr->Coord256_2=coord256_2;
 sMove_SelectPtr->MoveType=movetype;
 sMove_SelectPtr->NewFigureType=new_figure_type;
}
//----------------------------------------------------------------------------------------------------
//добавить ход со взятием
//----------------------------------------------------------------------------------------------------
void Move_AddEatMove(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,MOVE_TYPE movetype,SMove *sMove_EatPtr,long &move_eat,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr)
{
 CELL cell=Board[coord256_1];
 FIGURE_TYPE type=cell&MASK_TYPE;//тип ходившей фигуры
 CELL eat_cell=Board[coord256_2];
 FIGURE_TYPE eat_type=eat_cell&MASK_TYPE;//тип съеденной фигуры
 long score=0;
 if (eat_type==FIGURE_TYPE_KING) score+=WEIGHT_KING*10;
 if (eat_type==FIGURE_TYPE_QUEEN) score+=WEIGHT_QUEEN*10;
 if (eat_type==FIGURE_TYPE_ROOK) score+=WEIGHT_ROOK*10;
 if (eat_type==FIGURE_TYPE_BISHOP) score+=WEIGHT_BISHOP*10;
 if (eat_type==FIGURE_TYPE_KNIGHT) score+=WEIGHT_KNIGHT*10;
 if (eat_type==FIGURE_TYPE_PAWN) score+=WEIGHT_PAWN*10;
  
 if (type==FIGURE_TYPE_KING) score-=WEIGHT_KING;
 if (type==FIGURE_TYPE_QUEEN) score-=WEIGHT_QUEEN;
 if (type==FIGURE_TYPE_ROOK) score-=WEIGHT_ROOK;
 if (type==FIGURE_TYPE_BISHOP) score-=WEIGHT_BISHOP;
 if (type==FIGURE_TYPE_KNIGHT) score-=WEIGHT_KNIGHT;
 if (type==FIGURE_TYPE_PAWN) score-=WEIGHT_PAWN; 
 if (sMoveKitPtr->sMove_Last.Coord256_2==coord256_2) score+=WEIGHT_INFINITY;//продвигаем взятие последней фигуры противника вперёд
 if (sMoveKitPtr->sMove_PrincipalVariation.Coord256_2==coord256_2 && sMoveKitPtr->sMove_PrincipalVariation.Coord256_1==coord256_1) score+=WEIGHT_INFINITY>>1;//продвигаем ход из главной линии 
 if (movetype!=MOVE_TYPE_SIMPLY) score+=WEIGHT_INFINITY>>2;//продвигаем необычный ход
 

 SMove *sMove_SelectPtr=&sMove_EatPtr[move_eat];//ход всегда ставим в конец
 //сортируем ходы по возрастанию score
 SMove *sMove_MorePtr=NULL;//больший элемент
 SMove *sMove_LessPtr=sMove_FirstEatPtr;//меньший элемент
 while (sMove_LessPtr!=NULL)
 {
  if (sMove_LessPtr->Score<score) break;
  sMove_MorePtr=sMove_LessPtr;
  sMove_LessPtr=sMove_LessPtr->sMove_NextPtr;
 }
 sMove_SelectPtr->sMove_NextPtr=sMove_LessPtr;//задаём меньший элемент
 //указываем на элемент, оценка которого ниже вставляемого
 if (sMove_MorePtr==NULL) sMove_FirstEatPtr=sMove_SelectPtr;//вставляемый элемент будет первым
 else  sMove_MorePtr->sMove_NextPtr=sMove_SelectPtr;
 move_eat++;
 sMove_SelectPtr->IsEat=ENGINE_TRUE;
 sMove_SelectPtr->Score=score;
 sMove_SelectPtr->Coord256_1=coord256_1;
 sMove_SelectPtr->Coord256_2=coord256_2;
 sMove_SelectPtr->MoveType=movetype;
 sMove_SelectPtr->NewFigureType=new_figure_type;
}
 
//----------------------------------------------------------------------------------------------------
//определение шахов королю
//----------------------------------------------------------------------------------------------------
inline ENGINE_BOOL Move_IsCheck(COORD coord256)
{
 long n;
 CELL cell=Board[coord256]; 
 FIGURE_COLOR color=cell&MASK_COLOR;
 FIGURE_COLOR opponent_color=color^(WHITE|BLACK);
 FIGURE_TYPE type=cell&MASK_TYPE;
 FIGURE_TYPE mask_king=opponent_color|FIGURE_TYPE_KING;
 //--------------------------------------------------
 //король
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KING)
 {
  n=0;
  while(KingMove[n]!=0)
  {
   COORD c256=coord256+KingMove[n];
   cell=Board[c256];
   if (cell!=CELL_EMPTY)
   {
    if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   }
   n++;
  }
  return(ENGINE_FALSE);
 }
 //--------------------------------------------------
 //ферзь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_QUEEN)
 {
  n=0;
  while(QueenMove[n]!=0)
  {
   COORD c256=coord256+QueenMove[n];    
   while(Board[c256]==CELL_EMPTY) c256+=QueenMove[n];//пока можно ходить
   cell=Board[c256];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   n++;
  }
  return(ENGINE_FALSE);
 }
 //--------------------------------------------------
 //ладья
 //--------------------------------------------------
 if (type==FIGURE_TYPE_ROOK)
 {
  n=0;
  while(RookMove[n]!=0)
  {
   COORD c256=coord256+RookMove[n];    
   while(Board[c256]==CELL_EMPTY) c256+=RookMove[n];//пока можно ходить
   cell=Board[c256];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   n++;
  }
  return(ENGINE_FALSE);
 }
 //--------------------------------------------------
 //слон
 //--------------------------------------------------
 if (type==FIGURE_TYPE_BISHOP)
 {
  n=0;
  while(BishopMove[n]!=0)
  {
   COORD c256=coord256+BishopMove[n];    
   while(Board[c256]==CELL_EMPTY) c256+=BishopMove[n];//пока можно ходить
   cell=Board[c256];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   n++;
  }
  return(ENGINE_FALSE);
 }
 //--------------------------------------------------
 //конь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KNIGHT)
 {
  n=0;
  while(KnightMove[n]!=0)
  {
   COORD c256=coord256+KnightMove[n];    
   cell=Board[c256];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   n++;
  }
  return(ENGINE_FALSE);
 }
 //--------------------------------------------------
 //пешка
 //--------------------------------------------------
 if (type==FIGURE_TYPE_PAWN)
 {  
  if (color==WHITE)//если это белая пешка
  {
   //проверим, можно ли есть
   cell=Board[coord256-1+16];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   cell=Board[coord256+1+16];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
  }
  else//если это чёрная пешка
  {
   //проверим, можно ли есть
   cell=Board[coord256-1-16];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
   cell=Board[coord256+1-16];
   if ((cell&(MASK_COLOR|MASK_TYPE))==mask_king) return(ENGINE_TRUE);//король противника
  }
  return(ENGINE_FALSE);
 }
 return(ENGINE_FALSE);
}
 
 
//----------------------------------------------------------------------------------------------------
//определение всех возможных ходов фигуры
//----------------------------------------------------------------------------------------------------
inline void Move_CreateMove(COORD coord256,SMove *sMove_Ptr,long &move,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstPtr,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr)
{
 long n;
 CELL cell=Board[coord256]; 
 FIGURE_COLOR color=cell&MASK_COLOR;
 FIGURE_COLOR opponent_color=color^(WHITE|BLACK);
 FIGURE_TYPE type=cell&MASK_TYPE; 
 //--------------------------------------------------
 //король
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KING)
 {
  CELL ismoved=cell&MASK_IS_MOVED;
  n=0;
  while(KingMove[n]!=0)
  {
   COORD c256=coord256+KingMove[n];
   cell=Board[c256];
   if (cell==CELL_EMPTY) Move_AddMove(coord256,c256,FIGURE_TYPE_KING,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
   else
   {
    if ((cell&MASK_COLOR)==opponent_color)//фигура противника
    {
     Move_AddEatMove(coord256,c256,FIGURE_TYPE_KING,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
   }
   n++;
  }
  //возможные ходы для рокировки  
  if (ismoved==0)//король ещё не ходил
  {
   //короткая рокировка
   if ((Board[coord256+1]|Board[coord256+2])==0)//поля свободны
   {
    CELL rook_b=Board[coord256+3];	
    FIGURE_TYPE rook_type=rook_b&MASK_TYPE;
    if (rook_type==FIGURE_TYPE_ROOK)//фигура на месте
    {
     if ((rook_b&(MASK_COLOR|MASK_IS_MOVED))==color) Move_AddMove(coord256,coord256+2,FIGURE_TYPE_KING,MOVE_TYPE_CASTLING,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
    }
   }
   //длинная рокировка
   if ((Board[coord256-1]|Board[coord256-2]|Board[coord256-3])==0)//поля свободны
   {    
    CELL rook_b=Board[coord256-4];
    FIGURE_TYPE rook_type=rook_b&MASK_TYPE;
    if (rook_type==FIGURE_TYPE_ROOK)
    {
     if ((rook_b&(MASK_COLOR|MASK_IS_MOVED))==color) Move_AddMove(coord256,coord256-2,FIGURE_TYPE_KING,MOVE_TYPE_CASTLING,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
    }
   }
  }
  return;
 }
 //--------------------------------------------------
 //ферзь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_QUEEN)
 {
  n=0;
  while(QueenMove[n]!=0)
  {
   COORD c256=coord256+QueenMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    Move_AddMove(coord256,c256,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    c256+=QueenMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //ладья
 //--------------------------------------------------
 if (type==FIGURE_TYPE_ROOK)
 {
  n=0;
  while(RookMove[n]!=0)
  {
   COORD c256=coord256+RookMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    Move_AddMove(coord256,c256,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    c256+=RookMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //слон
 //--------------------------------------------------
 if (type==FIGURE_TYPE_BISHOP)
 {
  n=0;
  while(BishopMove[n]!=0)
  {
   COORD c256=coord256+BishopMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    Move_AddMove(coord256,c256,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    c256+=BishopMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //конь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KNIGHT)
 {
  n=0;
  while(KnightMove[n]!=0)
  {
   COORD c256=coord256+KnightMove[n];    
   cell=Board[c256];
   if (cell==CELL_EMPTY)//можно ходить в эту клетку
   {     
    Move_AddMove(coord256,c256,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
   }
   else
   {
    if ((cell&MASK_COLOR)==opponent_color)//фигура противника
    {
     Move_AddEatMove(coord256,c256,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //пешка
 //--------------------------------------------------
 if (type==FIGURE_TYPE_PAWN)
 {  
  long y=Coord256to64[coord256];
  y>>=3;
  if (color==WHITE)//если это белая пешка
  {
   //ход пешкой на одну клетку вперёд
   if (Board[coord256+16]==CELL_EMPTY)
   {
    if (y+1==7)//пешка меняет свой тип
    {
     Move_AddMove(coord256,coord256+16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256+16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256+16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256+16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
    }
    else Move_AddMove(coord256,coord256+16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    //если ходить вперёд можно, то проверим не начальное ли это положение
    if ((cell&MASK_IS_MOVED)==0)//пешка ещё не ходила и никого за первый допустимый ход не ела
    {
     //ход пешкой на две клетки вперёд
     if (Board[coord256+32]==CELL_EMPTY) Move_AddMove(coord256,coord256+32,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    }
   }
   //проверим, можно ли есть
   cell=Board[coord256-1+16];
   if ((cell&MASK_COLOR)==BLACK)
   {
    if (y+1==7)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   cell=Board[coord256+1+16];
   if ((cell&MASK_COLOR)==BLACK)
   {
    if (y+1==7)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  else//если это чёрная пешка
  {
   //ход пешкой на одну клетку вперёд
   if (Board[coord256-16]==CELL_EMPTY)
   {
    if (y-1==0)//пешка меняет свой тип
    {
     Move_AddMove(coord256,coord256-16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256-16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256-16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
     Move_AddMove(coord256,coord256-16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);
    }
    else Move_AddMove(coord256,coord256-16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    //если ходить вперёд можно, то проверим не начальное ли это положение
    if ((cell&MASK_IS_MOVED)==0)//пешка ещё не ходила и никого за первый допустимый ход не ела
    {
     //ход пешкой на две клетки вперёд
     if (Board[coord256-32]==CELL_EMPTY) Move_AddMove(coord256,coord256-32,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_Ptr,move,sMove_FirstPtr,sMoveKitPtr);//клетка пустая
    }
   }
   //проверим, можно ли есть
   cell=Board[coord256-1-16];
   if ((cell&MASK_COLOR)==WHITE)
   {
    if (y-1==0)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   cell=Board[coord256+1-16];
   if ((cell&MASK_COLOR)==WHITE)
   {
    if (y-1==0)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);	
    }
    else Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  //проверим, можем ли мы взять проходную пешку
  if (coord256_passedpawn!=0)//была проходная пешка
  {
   CELL passedpawn_b=Board[coord256_passedpawn];
   if (color==WHITE && y==4 && (passedpawn_b&MASK_COLOR)==BLACK)//если играют белые и со своей горизонтали
   {
    if (coord256_passedpawn==coord256-1 || coord256_passedpawn==coord256+1) Move_AddEatMove(coord256,coord256_passedpawn+16,FIGURE_TYPE_PAWN,MOVE_TYPE_BLACK_PASSED_PAWN_EAT,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   if (color==BLACK && y==3 && (passedpawn_b&MASK_COLOR)==WHITE)//если играют чёрные и со своей горизонтали
   {
    if (coord256_passedpawn==coord256-1 || coord256_passedpawn==coord256+1) Move_AddEatMove(coord256,coord256_passedpawn-16,FIGURE_TYPE_PAWN,MOVE_TYPE_WHITE_PASSED_PAWN_EAT,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  return;
 }
}
 
//----------------------------------------------------------------------------------------------------
//определение всех возможных ходов фигуры со взятием
//----------------------------------------------------------------------------------------------------
void Move_CreateEatMove(COORD coord256,SMove *sMove_EatPtr,long &move_eat,COORD coord256_passedpawn,SMove* &sMove_FirstEatPtr,SMoveKit *sMoveKitPtr)
{
 long n;
 CELL cell=Board[coord256]; 
 FIGURE_COLOR color=cell&MASK_COLOR;
 FIGURE_COLOR opponent_color=color^(WHITE|BLACK);
 FIGURE_TYPE type=cell&MASK_TYPE; 
 //--------------------------------------------------
 //король
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KING)
 {
  n=0;
  while(KingMove[n]!=0)
  {
   COORD c256=coord256+KingMove[n];
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_KING,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //ферзь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_QUEEN)
 {
  n=0;
  while(QueenMove[n]!=0)
  {
   COORD c256=coord256+QueenMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    c256+=QueenMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_QUEEN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //ладья
 //--------------------------------------------------
 if (type==FIGURE_TYPE_ROOK)
 {
  n=0;
  while(RookMove[n]!=0)
  {
   COORD c256=coord256+RookMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    c256+=RookMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_ROOK,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //слон
 //--------------------------------------------------
 if (type==FIGURE_TYPE_BISHOP)
 {
  n=0;
  while(BishopMove[n]!=0)
  {
   COORD c256=coord256+BishopMove[n];    
   while(Board[c256]==CELL_EMPTY)//пока можно ходить
   {     
    c256+=BishopMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_BISHOP,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //конь
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KNIGHT)
 {
  n=0;
  while(KnightMove[n]!=0)
  {
   COORD c256=coord256+KnightMove[n];    
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)//фигура противника
   {
    Move_AddEatMove(coord256,c256,FIGURE_TYPE_KNIGHT,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //пешка
 //--------------------------------------------------
 if (type==FIGURE_TYPE_PAWN)
 {  
  long y=Coord256to64[coord256];
  y>>=3;
  if (color==WHITE)//если это белая пешка
  {
   //проверим, можно ли есть
   cell=Board[coord256-1+16];
   if ((cell&MASK_COLOR)==BLACK)
   {
    if (y+1==7)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_QUEEN,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_ROOK,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_BISHOP,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256-1+16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   cell=Board[coord256+1+16];
   if ((cell&MASK_COLOR)==BLACK)
   {
    if (y+1==7)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_QUEEN,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_ROOK,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_BISHOP,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256+1+16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  else//если это чёрная пешка
  {
   //проверим, можно ли есть
   cell=Board[coord256-1-16];
   if ((cell&MASK_COLOR)==WHITE)
   {
    if (y-1==0)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_QUEEN,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_ROOK,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_BISHOP,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
    }
    else Move_AddEatMove(coord256,coord256-1-16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   cell=Board[coord256+1-16];
   if ((cell&MASK_COLOR)==WHITE)
   {
    if (y-1==0)//пешка меняет свой тип
    {
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_QUEEN,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_ROOK,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_KNIGHT,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
     Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_BISHOP,MOVE_TYPE_CONVERSION,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);	
    }
    else Move_AddEatMove(coord256,coord256+1-16,FIGURE_TYPE_PAWN,MOVE_TYPE_SIMPLY,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  //проверим, можем ли мы взять проходную пешку
  if (coord256_passedpawn!=0)//была проходная пешка
  {
   CELL passedpawn_b=Board[coord256_passedpawn];
   if (color==WHITE && y==4 && (passedpawn_b&MASK_COLOR)==BLACK)//если играют белые и со своей горизонтали
   {
    if (coord256_passedpawn==coord256-1 || coord256_passedpawn==coord256+1) Move_AddEatMove(coord256,coord256_passedpawn+16,FIGURE_TYPE_PAWN,MOVE_TYPE_BLACK_PASSED_PAWN_EAT,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
   if (color==BLACK && y==3 && (passedpawn_b&MASK_COLOR)==WHITE)//если играют чёрные и со своей горизонтали
   {
    if (coord256_passedpawn==coord256-1 || coord256_passedpawn==coord256+1) Move_AddEatMove(coord256,coord256_passedpawn-16,FIGURE_TYPE_PAWN,MOVE_TYPE_WHITE_PASSED_PAWN_EAT,sMove_EatPtr,move_eat,sMove_FirstEatPtr,sMoveKitPtr);
   }
  }
  return;
 }
}
 
//----------------------------------------------------------------------------------------------------
//проверить не съели ли короля
//----------------------------------------------------------------------------------------------------
ENGINE_BOOL Move_CheckEatKing(FIGURE_COLOR color)
{
 if (color==WHITE)//проверка для белого короля
 {
  if (*KingWhitePointer==0) return(ENGINE_TRUE);//короля съели
 }
 else//проверка для чёрного короля
 {
  if (*KingBlackPointer==0) return(ENGINE_TRUE);//короля съели
 }
 return(ENGINE_FALSE);
}
//----------------------------------------------------------------------------------------------------
//проверить шах королю
//----------------------------------------------------------------------------------------------------
ENGINE_BOOL Move_CheckKing(FIGURE_COLOR color)
{
 if (color==WHITE)//проверка шаха для белого короля
 {   
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureBlackCoord256[n];
   if (coord256==0) continue;//фигуры нет
   if (Move_IsCheck(coord256)==ENGINE_TRUE) return(ENGINE_TRUE);//фигура шахует белого короля
  }
  return(ENGINE_FALSE);
 }
 else//проверка шаха для чёрного короля
 {
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureWhiteCoord256[n];
   if (coord256==0) continue;//фигуры нет
   if (Move_IsCheck(coord256)==ENGINE_TRUE) return(ENGINE_TRUE);//фигура шахует белого короля
  }
  return(ENGINE_FALSE);
 }
 return(ENGINE_FALSE);
}
//----------------------------------------------------------------------------------------------------
//переместить фигуру с возможным съедением и вернуть съеденую фигуру
//----------------------------------------------------------------------------------------------------
CELL Move_MoveFigure(COORD coord256_1,COORD coord256_2,FIGURE_TYPE new_figure_type,FIGURE_TYPE &old_figure_type,CELL &ismoved)
{
 CELL move_cell=Board[coord256_1];
 CELL eat_cell=Board[coord256_2];
 FIGURE_TYPE type_move=move_cell&MASK_TYPE;
 FIGURE_TYPE type_eat=eat_cell&MASK_TYPE;
 FIGURE_COLOR color_move=move_cell&MASK_COLOR;
 CELL move_index=(move_cell&MASK_INDEX)>>INDEX_SHIFT;
 ZOBRIST_COLOR zcolor_move=ZOBRIST_BLACK;
 if (color_move==WHITE) 
 {
  FigureWhiteCoord256[move_index]=coord256_2;
  zcolor_move=ZOBRIST_WHITE;
 }
 else FigureBlackCoord256[move_index]=coord256_2;
 old_figure_type=type_move;
 if (type_eat!=0)
 {
  FIGURE_COLOR color_eat=eat_cell&MASK_COLOR;
  CELL eat_index=(eat_cell&MASK_INDEX)>>INDEX_SHIFT;
  ZOBRIST_COLOR zcolor_eat=ZOBRIST_BLACK;
  if (color_eat==WHITE) 
  {
   zcolor_eat=ZOBRIST_WHITE;
   FigureWhiteCoord256[eat_index]=0;
  }
  else FigureBlackCoord256[eat_index]=0;
  HKey^=ZobristKey[type_eat][zcolor_eat][coord256_2];//убираем съеденную фигуру из ключа  
 }
 HKey=HKey^ZobristKey[type_move][zcolor_move][coord256_1]^ZobristKey[new_figure_type][zcolor_move][coord256_2];//перемещаем ходившую фигуру
 ismoved=move_cell&MASK_IS_MOVED;
 move_cell&=CELL_FULL_VALUE^MASK_TYPE;
 move_cell|=new_figure_type;
 Board[coord256_2]=move_cell|MASK_IS_MOVED;
 Board[coord256_1]=0;
 return(eat_cell);
}
//----------------------------------------------------------------------------------------------------
//переместить фигуру обратно с восстановлением съеденой фигуры
//----------------------------------------------------------------------------------------------------
void Move_UnMoveFigure(COORD coord256_1,COORD coord256_2,FIGURE_TYPE old_figure_type,CELL ismoved,CELL eat_cell)
{
 CELL move_cell=Board[coord256_2];
 FIGURE_TYPE type_move=move_cell&MASK_TYPE;
 FIGURE_COLOR color_move=move_cell&MASK_COLOR;
 CELL move_index=(move_cell&MASK_INDEX)>>INDEX_SHIFT;
 ZOBRIST_COLOR zcolor_move=ZOBRIST_BLACK;
 if (color_move==WHITE) 
 {
  FigureWhiteCoord256[move_index]=coord256_1;
  zcolor_move=ZOBRIST_WHITE;
 }
 else FigureBlackCoord256[move_index]=coord256_1;
 Board[coord256_2]=eat_cell;
 FIGURE_TYPE type_eat=eat_cell&MASK_TYPE;
 if (type_eat!=0)
 {
  CELL eat_index=(eat_cell&MASK_INDEX)>>INDEX_SHIFT;
  ZOBRIST_COLOR zcolor_eat=ZOBRIST_BLACK; 
  if ((eat_cell&MASK_COLOR)==WHITE)
  {
   zcolor_eat=ZOBRIST_WHITE;
   FigureWhiteCoord256[eat_index]=coord256_2;
  }
  else FigureBlackCoord256[eat_index]=coord256_2;
  HKey^=ZobristKey[type_eat][zcolor_eat][coord256_2];//восстанавливаем съеденную фигуру в ключе
 }
 HKey=HKey^ZobristKey[type_move][zcolor_move][coord256_2]^ZobristKey[old_figure_type][zcolor_move][coord256_1];//перемещаем ходившую фигуру 
 move_cell&=CELL_FULL_VALUE^MASK_TYPE;
 move_cell|=old_figure_type;
 Board[coord256_1]=(move_cell&(CELL_FULL_VALUE^MASK_IS_MOVED))|ismoved;
}
//----------------------------------------------------------------------------------------------------
//отменить ход, сделаный по правилам
//----------------------------------------------------------------------------------------------------
void Move_LegalMoveOff(SMove sMove,FIGURE_TYPE old_figure_type,CELL ismoved,CELL eat_cell)
{
 COORD coord256_1=sMove.Coord256_1;
 COORD coord256_2=sMove.Coord256_2; 
 //отменяем простой ход либо ход с превращением
 if (sMove.MoveType==MOVE_TYPE_SIMPLY || sMove.MoveType==MOVE_TYPE_CONVERSION)
 {
  Move_UnMoveFigure(coord256_1,coord256_2,old_figure_type,ismoved,eat_cell);
  return;
 }
 //отменяем рокировку
 if (sMove.MoveType==MOVE_TYPE_CASTLING)
 {
  if (coord256_2==coord256_1-2)//длинная рокировка (левая)
  {
   //восстанавливаем позицию ладьи
   Move_UnMoveFigure(coord256_1-4,coord256_1-1,FIGURE_TYPE_ROOK,0,0);
   //восстанавливаем позицию короля
   Move_UnMoveFigure(coord256_1,coord256_1-2,FIGURE_TYPE_KING,0,0);
   //снимаем обоим флаг рокировки
   Board[coord256_1-4]&=CELL_FULL_VALUE^MASK_CASTLING;
   Board[coord256_1]&=CELL_FULL_VALUE^MASK_CASTLING;
  }
  if (coord256_2==coord256_1+2)//короткая рокировка (правая)
  {
   //восстанавливаем позицию ладьи
   Move_UnMoveFigure(coord256_1+3,coord256_1+1,FIGURE_TYPE_ROOK,0,0);
   //восстанавливаем позицию короля
   Move_UnMoveFigure(coord256_1,coord256_1+2,FIGURE_TYPE_KING,0,0);
   //снимаем обоим флаг рокировки
   Board[coord256_1+3]&=CELL_FULL_VALUE^MASK_CASTLING;
   Board[coord256_1]&=CELL_FULL_VALUE^MASK_CASTLING;
  }
  return;
 }
 //отменяем взятие белой проходной пешки
 if (sMove.MoveType==MOVE_TYPE_WHITE_PASSED_PAWN_EAT)
 {
  //отменяем ход
  Move_UnMoveFigure(coord256_2+16,coord256_2,FIGURE_TYPE_PAWN,MASK_IS_MOVED,0);
  Move_UnMoveFigure(coord256_1,coord256_2+16,FIGURE_TYPE_PAWN,ismoved,eat_cell);
  return;
 }
 //отменяем взятие чёрной проходной пешки
 if (sMove.MoveType==MOVE_TYPE_BLACK_PASSED_PAWN_EAT)
 {
  //отменяем ход
  Move_UnMoveFigure(coord256_2-16,coord256_2,FIGURE_TYPE_PAWN,MASK_IS_MOVED,0);
  Move_UnMoveFigure(coord256_1,coord256_2-16,FIGURE_TYPE_PAWN,ismoved,eat_cell);
  return;
 }
}
//----------------------------------------------------------------------------------------------------
//сделать ход по правилам, вернуть допустим ли ход и установить указатели на проходную пешку и съеденную фигуру
//----------------------------------------------------------------------------------------------------
ENGINE_BOOL Move_LegalMoveOn(SMove sMove,FIGURE_TYPE &old_figure_type,CELL &ismoved,CELL &eat_cell,COORD &coord256_passedpawn)
{
 eat_cell=0;
 coord256_passedpawn=0;
 COORD coord256_1=sMove.Coord256_1;
 COORD coord256_2=sMove.Coord256_2;
 CELL cell=Board[coord256_1];
 FIGURE_TYPE type=cell&MASK_TYPE;
 FIGURE_TYPE new_figure_type=sMove.NewFigureType; 
 FIGURE_COLOR color=cell&MASK_COLOR;
 old_figure_type=type;
 //делаем простой ход либо ход с превращением (с возможным взятие фигуры оппонента)
 if (sMove.MoveType==MOVE_TYPE_SIMPLY || sMove.MoveType==MOVE_TYPE_CONVERSION)
 {
  eat_cell=Move_MoveFigure(coord256_1,coord256_2,new_figure_type,old_figure_type,ismoved);
  //проверяем допустимость хода
  if (Move_CheckKing(color)==ENGINE_FALSE)//шаха королю нет, можно ходить
  {
   //проверим, что мы не ходили проходной пешкой
   if (type==FIGURE_TYPE_PAWN)
   {
    if (coord256_2-coord256_1==32 || coord256_2-coord256_1==-32) coord256_passedpawn=coord256_2;//пешка пошла на два поля, значит, стала проходной
   }
   return(ENGINE_TRUE);
  }
  //ход запрещён
  Move_UnMoveFigure(coord256_1,coord256_2,old_figure_type,ismoved,eat_cell);
  return(ENGINE_FALSE);
 }
 //делаем рокировку
 if (sMove.MoveType==MOVE_TYPE_CASTLING)
 {
  FIGURE_TYPE old_type;
  if (Move_CheckKing(color)==ENGINE_TRUE) return(ENGINE_FALSE);//шах королю - рокировка невозможна
  //узнаём, какая была рокировка
  if (coord256_2==coord256_1-2)//длинная рокировка (левая)
  {   
   //проверим, что ни на одном поле для рокировки шаха для короля нет
   ENGINE_BOOL enabled=ENGINE_TRUE;
   for(long n=1;n<=2;n++)
   {
    //делаем ход королём
    Move_MoveFigure(coord256_1,coord256_1-n,FIGURE_TYPE_KING,old_type,ismoved);
    if (Move_CheckKing(color)==ENGINE_TRUE) enabled=ENGINE_FALSE;
    //отменяем ход
    Move_UnMoveFigure(coord256_1,coord256_1-n,old_type,ismoved,eat_cell);
    if (enabled==ENGINE_FALSE) break;
   }
   if (enabled==ENGINE_TRUE)//рокировку можно сделать
   {
    //передвигаем короля
    CELL ismoved_king;
    Move_MoveFigure(coord256_1,coord256_1-2,FIGURE_TYPE_KING,old_type,ismoved_king);
    //передвигаем ладью
    CELL ismoved_rook;
    Move_MoveFigure(coord256_1-4,coord256_1-1,FIGURE_TYPE_ROOK,old_type,ismoved_rook);
    //ставим обоим флаг рокировки
    Board[coord256_1-2]|=CASTLING_O_O_O;
    Board[coord256_1-1]|=CASTLING_O_O_O;
    return(ENGINE_TRUE);
   }
  }
  if (coord256_2==coord256_1+2)//короткая рокировка (правая)
  {
   //проверим, что ни на одном поле для рокировки шаха для короля нет
   ENGINE_BOOL enabled=ENGINE_TRUE;
   for(long n=1;n<=2;n++)
   {
    //делаем ход королём
    Move_MoveFigure(coord256_1,coord256_1+n,FIGURE_TYPE_KING,old_type,ismoved);
    if (Move_CheckKing(color)==ENGINE_TRUE) enabled=ENGINE_FALSE;
    //отменяем ход
    Move_UnMoveFigure(coord256_1,coord256_1+n,old_type,ismoved,eat_cell);
    if (enabled==ENGINE_FALSE) break;
   }
   if (enabled==ENGINE_TRUE)//рокировку можно сделать
   {
    //передвигаем короля
    CELL ismoved_king;
    Move_MoveFigure(coord256_1,coord256_1+2,FIGURE_TYPE_KING,old_type,ismoved_king);
    //передвигаем ладью
    CELL ismoved_rook;
    Move_MoveFigure(coord256_1+3,coord256_1+1,FIGURE_TYPE_ROOK,old_type,ismoved_rook);
    //ставим обоим флаг рокировки
    Board[coord256_1+2]|=CASTLING_O_O;
    Board[coord256_1+1]|=CASTLING_O_O;
    return(ENGINE_TRUE);
   }
  }
  return(ENGINE_FALSE);//рокировка невозможна
 }
 //делаем взятие белой проходной пешки
 if (sMove.MoveType==MOVE_TYPE_WHITE_PASSED_PAWN_EAT)
 {
  FIGURE_TYPE old_type;
  //клетка, куда нужно походить всегда пустая
  //снимаем с поля проходную пешку и ставим на её место нашу пешку
  eat_cell=Move_MoveFigure(coord256_1,coord256_2+16,FIGURE_TYPE_PAWN,old_type,ismoved);
  //передвигаем нашу пешку на свободное поле
  CELL ismoved_local;
  Move_MoveFigure(coord256_2+16,coord256_2,FIGURE_TYPE_PAWN,old_type,ismoved_local);
  if (Move_CheckKing(color)==ENGINE_FALSE) return(ENGINE_TRUE);//шаха королю нет, можно ходить
  //отменяем ход
  Move_UnMoveFigure(coord256_2+16,coord256_2,FIGURE_TYPE_PAWN,ismoved_local,0);
  Move_UnMoveFigure(coord256_1,coord256_2+16,FIGURE_TYPE_PAWN,ismoved,eat_cell);
  return(ENGINE_FALSE);
 }
 //делаем взятие чёрной проходной пешки
 if (sMove.MoveType==MOVE_TYPE_BLACK_PASSED_PAWN_EAT)
 {
  FIGURE_TYPE old_type;
  //клетка, куда нужно походить всегда пустая
  //снимаем с поля проходную пешку и ставим на её место нашу пешку
  eat_cell=Move_MoveFigure(coord256_1,coord256_2-16,FIGURE_TYPE_PAWN,old_type,ismoved);
  //передвигаем нашу пешку на свободное поле
  CELL ismoved_local;
  Move_MoveFigure(coord256_2-16,coord256_2,FIGURE_TYPE_PAWN,old_type,ismoved_local);
  if (Move_CheckKing(color)==ENGINE_FALSE) return(ENGINE_TRUE);//шаха королю нет, можно ходить
  //отменяем ход
  Move_UnMoveFigure(coord256_2-16,coord256_2,FIGURE_TYPE_PAWN,ismoved_local,0);
  Move_UnMoveFigure(coord256_1,coord256_2-16,FIGURE_TYPE_PAWN,ismoved,eat_cell);
  return(ENGINE_FALSE);
 }
 //все остальные ходы невозможны
 return(ENGINE_FALSE);
}
//----------------------------------------------------------------------------------------------------
//сделать ход
//----------------------------------------------------------------------------------------------------
ENGINE_BOOL Move_MoveOn(FIGURE_COLOR color,COORD x1,COORD y1,COORD x2,COORD y2,COORD &coord256_passedpawn,FIGURE_TYPE NewFigureType)
{
 HKeyLevel[FirstLevel]=Hash_GetHKey();
 if (color==BLACK) HKeyLevel[FirstLevel]^=ZobristKeyMove;

 long n;
 CELL ismoved;
 //узнаём, что за фигурой мы ходим
 COORD coord64_1=x1|(y1<<3);
 COORD coord64_2=x2|(y2<<3);
 COORD coord256_1=Coord64to256[coord64_1];
 COORD coord256_2=Coord64to256[coord64_2];
  
 CELL cell=Board[coord256_1];
 FIGURE_TYPE type=cell&MASK_TYPE;
 FIGURE_TYPE old_figure_type;
 if (type==0) return(ENGINE_FALSE);//поле пустое
 if ((cell&MASK_COLOR)!=color) return(ENGINE_FALSE);//это фигура оппонента
 //создаём все ходы фигурой
 SMove *sMove_FirstPtr=NULL;
 SMove *sMove_FirstEatPtr=NULL;
 long move=0;
 long move_eat=0;
 SMoveKit sMoveKit;
 sMoveKit.sMove_Hash.Coord256_1=0;
 sMoveKit.sMove_Hash.Coord256_2=0;
 sMoveKit.sMove_Last.Coord256_1=0;
 sMoveKit.sMove_Last.Coord256_2=0;
 sMoveKit.sMove_Killer.Coord256_1=0;
 sMoveKit.sMove_Killer.Coord256_2=0;
 Move_CreateMove(coord256_1,sMove_Level[0],move,sMove_EatLevel[0],move_eat,coord256_passedpawn,sMove_FirstPtr,sMove_FirstEatPtr,&sMoveKit);
 //ищем среди обычных ходов наш ход
 for(n=0;n<move;n++)
 {
  if (sMove_Level[0][n].Coord256_1==coord256_1 && sMove_Level[0][n].Coord256_2==coord256_2)//нашли наш ход
  {
   //смотрим, не дошла ли белая пешка до края доски
   if ((cell&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|WHITE) && y2==7 && NewFigureType!=sMove_Level[0][n].NewFigureType) continue;
   //смотрим, не дошла ли чёрная пешка до края доски
   if ((cell&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|BLACK) && y2==0 && NewFigureType!=sMove_Level[0][n].NewFigureType) continue;
   CELL eat_cell;   
   ENGINE_BOOL enabled=Move_LegalMoveOn(sMove_Level[0][n],old_figure_type,ismoved,eat_cell,coord256_passedpawn);
   if (enabled==ENGINE_TRUE)
   {
    FirstLevel++;
   }
   return(enabled);
  }
 }
 //ищем среди ходов съедения наш ход
 for(n=0;n<move_eat;n++)
 {
  if (sMove_EatLevel[0][n].Coord256_1==coord256_1 && sMove_EatLevel[0][n].Coord256_2==coord256_2)//нашли наш ход
  {
   //смотрим, не дошла ли белая пешка до края доски
   if ((cell&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|WHITE) && y2==7 && NewFigureType!=sMove_EatLevel[0][n].NewFigureType) continue;
   //смотрим, не дошла ли чёрная пешка до края доски
   if ((cell&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|BLACK) && y2==0 && NewFigureType!=sMove_EatLevel[0][n].NewFigureType) continue;
   CELL eat_cell;
   ENGINE_BOOL enabled=Move_LegalMoveOn(sMove_EatLevel[0][n],old_figure_type,ismoved,eat_cell,coord256_passedpawn);
   if (enabled==ENGINE_TRUE)
   {
    FirstLevel++;
   }
   return(enabled);
  }
 }
 return(ENGINE_FALSE);//так ходить нельзя
}
//----------------------------------------------------------------------------------------------------
//сортировка ходов
//----------------------------------------------------------------------------------------------------
SMove* Move_SortMove(SMove *sMove_FirstPtr,SMove *sMove_FirstEatPtr,SMoveKit *sMoveKitPtr)
{
 //объединяем взятия с обычными ходами
 SMove *sMove_CurrentPtr;
 sMove_CurrentPtr=sMove_FirstEatPtr;
 while(sMove_CurrentPtr!=NULL)
 {
  if (sMove_CurrentPtr->sMove_NextPtr==NULL)
  {
   sMove_CurrentPtr->sMove_NextPtr=sMove_FirstPtr;
   sMove_FirstPtr=sMove_FirstEatPtr;
   break;
  }
  sMove_CurrentPtr=sMove_CurrentPtr->sMove_NextPtr;
 } 
 //ищем особые ходы в полученном списке
 SMove *sMove_KillerPtr=NULL;
 SMove *sMove_HashPtr=NULL;
 SMove *sMove_KillerPrevPtr=NULL;
 SMove *sMove_HashPrevPtr=NULL;
 SMove *sMove_PrevPtr=NULL;
 sMove_CurrentPtr=sMove_FirstPtr;
 while(sMove_CurrentPtr!=NULL)
 {
  if (sMove_CurrentPtr->Coord256_1==sMoveKitPtr->sMove_Killer.Coord256_1 && 
	  sMove_CurrentPtr->Coord256_2==sMoveKitPtr->sMove_Killer.Coord256_2)
  {
   sMove_KillerPtr=sMove_CurrentPtr;
   sMove_KillerPrevPtr=sMove_PrevPtr;
  }
  if (sMove_CurrentPtr->Coord256_1==sMoveKitPtr->sMove_Hash.Coord256_1 && 
	  sMove_CurrentPtr->Coord256_2==sMoveKitPtr->sMove_Hash.Coord256_2)
  {
   sMove_HashPtr=sMove_CurrentPtr;
   sMove_HashPrevPtr=sMove_PrevPtr;
  }
  sMove_PrevPtr=sMove_CurrentPtr;
  sMove_CurrentPtr=sMove_CurrentPtr->sMove_NextPtr;
 } 
 //сортируем ходы в следующем порядке:
 //1) Ход из хэш-таблицы
 //2) Лучшее взятие
 //3) Ход Killer
 //4) Все взятия
 //5) Остальные ходы
 
 //1) Ход из хэш-таблицы
 if (sMove_HashPtr!=NULL && sMove_HashPtr!=sMove_FirstPtr)//ход из хэш-таблицы был найден
 {  
  //был ход-предшественник
  if (sMove_HashPrevPtr!=NULL) sMove_HashPrevPtr->sMove_NextPtr=sMove_HashPtr->sMove_NextPtr;
  sMove_HashPtr->sMove_NextPtr=sMove_FirstPtr;
  sMove_FirstPtr=sMove_HashPtr;
 } 
 //2) Лучшее взятие и так на втором месте (если оно есть)
 sMove_CurrentPtr=sMove_FirstEatPtr;
 //3) Ход Killer
 if (sMove_KillerPtr!=NULL && sMove_KillerPtr!=sMove_HashPtr && sMove_KillerPtr!=sMove_CurrentPtr && sMove_CurrentPtr!=NULL)//ход killer найден и он не совпадает с лучшим взятием и ходом из хэш-таблицы
 {
  //если был предшествующий ход
  if (sMove_KillerPrevPtr!=NULL) sMove_KillerPrevPtr->sMove_NextPtr=sMove_KillerPtr->sMove_NextPtr;
  sMove_KillerPtr->sMove_NextPtr=sMove_CurrentPtr->sMove_NextPtr;
  sMove_CurrentPtr->sMove_NextPtr=sMove_KillerPtr;
 } 
 return(sMove_FirstPtr);
}

