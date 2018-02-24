#include "engine_search.h"
 
extern SMove sMove_Level[MAX_PLY+5][200];//ходы фигурой
extern SMove sMove_EatLevel[MAX_PLY+5][200];//ходы фигурой со взятием
extern CELL Board[256];//шахматная доска с полем посередине (16x16)
extern COORD FigureWhiteCoord256[16];//позиции белых фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
extern COORD FigureBlackCoord256[16];//позиции чёрных фигур на доске (для быстрого доступа к фигурам. 0- фигуры нет)
extern COORD *KingWhitePointer;//указатель на короля в массиве позиций белых
extern COORD *KingBlackPointer;//указатель на короля в массиве позиций чёрных
extern struct SEvaluate sEvaluate;
extern SHTable *sHTable;
extern unsigned __int64 HKey;//хэш позиции
extern unsigned __int64 ZobristKeyMove;//ключ смены хода
extern unsigned __int64 ZobristKeyNullMove;//ключ нулевого хода
extern unsigned __int64 HKeyLevel[MAX_MOVES];//ключи позиции
extern unsigned long FirstLevel;//верхний уровень

ENGINE_BOOL StopSearch=ENGINE_FALSE;//останов поиска
SMove sMove_Killer[MAX_PLY+5];//ход Killer
SMove sMove_PrincipalVariation[MAX_PLY+5];//главная линия ходов
long Positions=0;//количество проанализированных позиций
long MaxLevel;//максимальный уровень анализа

//----------------------------------------------------------------------------------------------------
//таблицы перекодировки координат
//----------------------------------------------------------------------------------------------------
extern COORD Coord256to64[256];
extern COORD Coord64to256[64];
 
//----------------------------------------------------------------------------------------------------
//инициализация
//----------------------------------------------------------------------------------------------------
void Search_Init(void)
{
}
//----------------------------------------------------------------------------------------------------
//деинициализация
//----------------------------------------------------------------------------------------------------
void Search_Release(void)
{
}
//----------------------------------------------------------------------------------------------------
//определение изменения безопасности короля
//----------------------------------------------------------------------------------------------------
ENGINE_BOOL Search_IsChangedKingSafety(SMove *sMove_CurrentPtr,FIGURE_COLOR color)
{
 CELL cell=Board[sMove_CurrentPtr->Coord256_2];
 FIGURE_TYPE type=(cell&MASK_TYPE);
 if (type==FIGURE_TYPE_QUEEN || type==FIGURE_TYPE_ROOK || type==FIGURE_TYPE_BISHOP || type==FIGURE_TYPE_KNIGHT)
 {
  //считаем, как изменилось расстояние от короля до фигуры
  COORD coord64_f1=Coord256to64[sMove_CurrentPtr->Coord256_1];
  COORD coord64_f2=Coord256to64[sMove_CurrentPtr->Coord256_2];
  COORD coord64_king;
  if (color==WHITE) coord64_king=Coord256to64[*KingWhitePointer];
               else coord64_king=Coord256to64[*KingBlackPointer];
  long fy1=coord64_f1>>3;
  long fx1=coord64_f1&7;
  long fy2=coord64_f2>>3;
  long fx2=coord64_f2&7;
  long ky=coord64_king>>3;
  long kx=coord64_king&7;
   
  long dx1=kx-fx1;
  long dy1=ky-fy1;
  long dx2=kx-fx2;
  long dy2=ky-fy2;
  dx1*=dx1;
  dy1*=dy1;
  dx2*=dx2;
  dy2*=dy2;
  //продлеваем, если фигура приближается в районе короля (3 клетки от короля)
  if ((dx1+dy1)>(dx2+dy2) && ((dx2+dy2)<=9)) return(ENGINE_TRUE);
 }
 return(ENGINE_FALSE);
}
//----------------------------------------------------------------------------------------------------
//поиск хода в списке ходов
//----------------------------------------------------------------------------------------------------
SMove* Search_FindMove(SMove *sMove_Ptr,SMove sMove)
{
 if (sMove.Coord256_1==0 || sMove.Coord256_2==0) return(NULL);
 while(sMove_Ptr!=NULL)
 {
  if (sMove_Ptr->Coord256_1==sMove.Coord256_1 && sMove_Ptr->Coord256_2==sMove.Coord256_2 && sMove_Ptr->NewFigureType==sMove.NewFigureType && sMove_Ptr->IsEat==sMove.IsEat) return(sMove_Ptr); 
  sMove_Ptr=sMove_Ptr->sMove_NextPtr;
 }
 return(NULL);
}
//----------------------------------------------------------------------------------------------------
//функция выбора лучшего хода
//----------------------------------------------------------------------------------------------------
long Search_AlphaBeta(long alpha,long beta,long depth,long level,FIGURE_COLOR color,COORD coord256_passedpawn,SMove sMove_Last,ENGINE_BOOL PVNode,ENGINE_BOOL NullMove)
{ 
 if (StopSearch==ENGINE_TRUE) return(alpha);
 Positions++;
 if (level>=MaxLevel) MaxLevel=level; 
 if (Move_CheckEatKing(color)==ENGINE_TRUE) return(-WEIGHT_INFINITY+level);//случайно съели короля
 FIGURE_COLOR next_color=color^(WHITE|BLACK);
 if (Move_CheckEatKing(next_color)==ENGINE_TRUE) return(WEIGHT_INFINITY-level);//случайно съели короля
 //модифицируем ключ позиции с учётом цвета активных фигур
 unsigned __int64 HKey_Local=HKey;
 if (color==BLACK) HKey_Local^=ZobristKeyMove;
 if (level>=MAX_PLY) return(alpha);
 
 long old_alpha=alpha;
 CELL ismoved;
 CELL eat_cell;
 FIGURE_TYPE old_figure_type;
  
 if (level==0) PVNode=ENGINE_TRUE;
 //набор ходов
 SMoveKit sMoveKit;
 sMoveKit.sMove_Hash.Coord256_1=0;
 sMoveKit.sMove_Hash.Coord256_2=0;
 sMoveKit.sMove_Killer=sMove_Killer[level];
 sMoveKit.sMove_PrincipalVariation=sMove_PrincipalVariation[level];
 sMoveKit.sMove_Last=sMove_Last; 
 //----------------------------------------------------------------------------------------------------
 //проверяем повтор позиции
 //----------------------------------------------------------------------------------------------------
 long l=level+FirstLevel;
 HKeyLevel[l]=HKey_Local;//запоминаем ключ позиции
 if (l>=4 && level>0)
 {  
  for(long p=4;(l-p)>=0;p+=2)
  {    
   if (HKeyLevel[l]==HKeyLevel[l-p]) return(0);//ничья
  }  
 }
 //----------------------------------------------------------------------------------------------------
 //1) пробуем хэш-таблицу для получения отсечки
 //----------------------------------------------------------------------------------------------------
 if (PVNode==ENGINE_FALSE)//мы не в основном варианте
 {
  SHTable *sHTablePtr=&sHTable[HKey&(MAX_HASH_TABLE_SIZE-1)];//номер в таблице по остатку от деления
  if (sHTablePtr->HashType!=HASH_TABLE_TYPE_EMPTY && sHTablePtr->HKey==HKey_Local)//нашли такую позицию
  {
   if (sHTablePtr->HashType!=HASH_TABLE_TYPE_OLD)
   {
    //используем оценку из хэш таблицы
    if (sHTablePtr->Level>=level)
    {
     long score=sHTablePtr->Score;
     //корректировка оценки
     if (score>WEIGHT_INFINITY-MAX_LEVEL)  score-=level;
     if (score<-WEIGHT_INFINITY+MAX_LEVEL)  score+=level;
     if (sHTablePtr->HashType==HASH_TABLE_TYPE_BETA)//отсечка по beta
     {
      if (score>=beta)//проверим, что условие отсечки всё ещё выполняется
      {
       return(score);
      }
     }
     if (sHTablePtr->HashType==HASH_TABLE_TYPE_EXACT)//оценка точная
     {
      if (score>alpha && score<beta) return(score);      
     }
     if (sHTablePtr->HashType==HASH_TABLE_TYPE_ALPHA)//отсечки при записи в хэш-таблицу не было
     { 
      if (score<alpha) return(score);      
     }
    }    
   }
   if (sHTablePtr->Level>=level)
   {
    //запишем ход из хэша
    if (sHTablePtr->HashType!=HASH_TABLE_TYPE_ALPHA) sMoveKit.sMove_Hash=sHTablePtr->sMove;   
   }
  }
 }
 ENGINE_BOOL check_king=Move_CheckKing(color);
 if (level<MAX_LEVEL)
 {
  //ENGINE_BOOL state=Search_IsChangedKingSafety(&sMove_Last,color);
  if (check_king==ENGINE_TRUE) depth++;//продлеваем анализ шахов
 }
 //----------------------------------------------------------------------------------------------------
 //2) на максимальной глубине запускаем статический поиск
 //----------------------------------------------------------------------------------------------------
 if (depth<=0 || level>=MAX_LEVEL)//уровень возврата оценки позиции
 {   
  //вызываем статический поиск взятий
  long score=Search_Quies(alpha,beta,depth,level,color,coord256_passedpawn,sMove_Last,PVNode,NullMove);
  return(score);
 }  
 //----------------------------------------------------------------------------------------------------
 //отсечки
 //----------------------------------------------------------------------------------------------------
 if (PVNode==ENGINE_FALSE)//мы не в основном варианте
 {  
  if (check_king==ENGINE_FALSE)//король не под шахом
  {
   long evaluate=Score_Evaluate(color);//статическая оценка позиции   
   if (depth<FUTILITY_DEPTH && depth>=0)//достигнута глубина включения отсечек Razoring и Futility pruning
   {       
    //----------------------------------------------------------------------------------------------------
    //3) выполняем Razoring
    //----------------------------------------------------------------------------------------------------
    static long razoring_margin_table[5]={2*WEIGHT_PAWN,2*WEIGHT_PAWN,3*WEIGHT_PAWN,4*WEIGHT_PAWN,5*WEIGHT_PAWN};
    long margin=razoring_margin_table[depth];
    long beta_sub_margin=beta-margin;
    if (evaluate<beta_sub_margin)
    {
     evaluate=Search_Quies(beta_sub_margin-1,beta_sub_margin,depth,level,color,coord256_passedpawn,sMove_Last,PVNode,NullMove);//запускаем статический поиск
     if (evaluate<beta_sub_margin) return(evaluate);
    }
    //----------------------------------------------------------------------------------------------------
    //4) выполняем Futility pruning    
    //----------------------------------------------------------------------------------------------------
    if (NullMove==ENGINE_FALSE)//мы не в ветке нулевого хода
    {
     static long futility_margin_table[5]={2*WEIGHT_PAWN,2*WEIGHT_PAWN,3*WEIGHT_PAWN,4*WEIGHT_PAWN,5*WEIGHT_PAWN};
     margin=futility_margin_table[depth];
     long beta_add_margin=beta+margin;
     if (evaluate>=beta_add_margin) return(evaluate);
    }
   }     
   //----------------------------------------------------------------------------------------------------
   //5) пробуем нулевой ход
   //----------------------------------------------------------------------------------------------------
   if (NullMove==ENGINE_FALSE && depth>NULL_MOVE_DEPTH && evaluate>=beta && sMove_Last.IsEat==ENGINE_FALSE)
   {
    SMove sMove;
    sMove.Coord256_1=0;
    sMove.Coord256_2=0;
    sMove.IsEat=ENGINE_FALSE;
    HKey^=ZobristKeyNullMove;//вписываем ключ нулевого хода
    evaluate=-Search_AlphaBeta(-alpha,-(alpha-1),depth-NULL_MOVE_DEPTH-1,level+1,next_color,0,sMove,ENGINE_FALSE,ENGINE_TRUE);
    HKey^=ZobristKeyNullMove;//снимаем ключ нулевого хода
    if (evaluate>=beta) 
    {
     if (depth<VERIFICATOIN_SEARCH_REDUCTION+1) return(evaluate);
     //обход цугцванга
     evaluate=-Search_AlphaBeta(-(beta-1),-beta,depth-VERIFICATOIN_SEARCH_REDUCTION-1,level+1,next_color,0,sMove,ENGINE_FALSE,ENGINE_TRUE);
     if (evaluate>=beta) return(evaluate);
    }        
   }
  }
 }
 long move=0;
 long move_eat=0;
 SMove *sMove_FirstPtr=NULL;
 SMove *sMove_FirstEatPtr=NULL;
 //создаём все возможные ходы фигурами
 if (color==WHITE)
 {
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureWhiteCoord256[n];
   if (coord256==0) continue;//фигуры нет
   Move_CreateMove(coord256,sMove_Level[level],move,sMove_EatLevel[level],move_eat,coord256_passedpawn,sMove_FirstPtr,sMove_FirstEatPtr,&sMoveKit);
  }
 }
 else
 {
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureBlackCoord256[n];
   if (coord256==0) continue;//фигуры нет   
   Move_CreateMove(coord256,sMove_Level[level],move,sMove_EatLevel[level],move_eat,coord256_passedpawn,sMove_FirstPtr,sMove_FirstEatPtr,&sMoveKit);
  }
 }

 long ret=-WEIGHT_INFINITY+level;
 //сортируем ходы
 SMove *sMove_CurrentPtr;
 sMove_FirstPtr=Move_SortMove(sMove_FirstPtr,sMove_FirstEatPtr,&sMoveKit);
 //----------------------------------------------------------------------------------------------------
 //6) Выполняем ходы
 //----------------------------------------------------------------------------------------------------
 long move_counter=0; 
 static const long FutilityMoveCount[9]={19,19,19,19,19,35,67,131,259};
 sMove_CurrentPtr=sMove_FirstPtr;
 while(sMove_CurrentPtr!=NULL)
 {
  if (alpha>=beta) return(alpha);
  if (check_king==ENGINE_FALSE && sMove_CurrentPtr->IsEat==ENGINE_FALSE && sMove_CurrentPtr->MoveType==MOVE_TYPE_SIMPLY && PVNode==ENGINE_FALSE && sMove_Last.IsEat==ENGINE_FALSE)//король не под шахом и ход простой
  {
   if (move_counter>3)//уже проверяли ходы и не раз
   {
    if (depth<=8 && depth>=0)
    {
     if (move_counter>=FutilityMoveCount[depth])//переходим к следующему ходу
     {
      sMove_CurrentPtr=sMove_CurrentPtr->sMove_NextPtr;
      continue;
     }
    }
   }
  }
  if (Move_LegalMoveOn(*sMove_CurrentPtr,old_figure_type,ismoved,eat_cell,coord256_passedpawn)==ENGINE_TRUE)//ход допустим
  {
   long score=0;
   ENGINE_BOOL PVSearch=ENGINE_TRUE;
   ENGINE_BOOL LMRSearch=ENGINE_FALSE;
   ENGINE_BOOL NULLWindowSearch=ENGINE_FALSE;

   if (PVNode==ENGINE_TRUE && move_counter==0)//главный узел и первый ход
   {
    PVSearch=ENGINE_TRUE;
    LMRSearch=ENGINE_FALSE;
    NULLWindowSearch=ENGINE_FALSE;
   }
   else
   {
    PVSearch=ENGINE_FALSE;
    NULLWindowSearch=ENGINE_TRUE;
   }
   if (PVSearch==ENGINE_FALSE)//полный поиск не требуется
   {
    //проверяем, можем ли мы использовать поиск LMR
    LMRSearch=ENGINE_FALSE;
    if ((sMove_CurrentPtr->Coord256_1!=sMoveKit.sMove_Killer.Coord256_1 || sMove_CurrentPtr->Coord256_2!=sMoveKit.sMove_Killer.Coord256_2) &&
        (sMove_CurrentPtr->Coord256_1!=sMoveKit.sMove_Hash.Coord256_1 || sMove_CurrentPtr->Coord256_2!=sMoveKit.sMove_Hash.Coord256_2) &&
        (sMove_CurrentPtr->Coord256_1!=sMoveKit.sMove_PrincipalVariation.Coord256_1 || sMove_CurrentPtr->Coord256_2!=sMoveKit.sMove_PrincipalVariation.Coord256_2) &&
        (move_counter!=0) && check_king==ENGINE_FALSE && sMove_CurrentPtr->MoveType==MOVE_TYPE_SIMPLY && depth>=2 && sMove_CurrentPtr->IsEat==ENGINE_FALSE) LMRSearch=ENGINE_TRUE;
   }    
   //поиск LMR
   if (LMRSearch==ENGINE_TRUE)
   {
    score=-Search_AlphaBeta(-(alpha+1),-alpha,depth-2,level+1,next_color,coord256_passedpawn,*sMove_CurrentPtr,ENGINE_FALSE,ENGINE_FALSE);//сокращаем глубину на 2
    if (score>alpha) NULLWindowSearch=ENGINE_TRUE;
               else  NULLWindowSearch=ENGINE_FALSE;
   }
   if (NULLWindowSearch==ENGINE_TRUE) 
   {     
    score=-Search_AlphaBeta(-(alpha+1),-alpha,depth-1,level+1,next_color,coord256_passedpawn,*sMove_CurrentPtr,ENGINE_FALSE,ENGINE_FALSE);//сокращаем глубину на 1
    if (score>alpha && PVNode==ENGINE_TRUE) PVSearch=ENGINE_TRUE;
                                       else PVSearch=ENGINE_FALSE;
   }
   if (PVSearch==ENGINE_TRUE)
   {
    score=-Search_AlphaBeta(-beta,-alpha,depth-1,level+1,next_color,coord256_passedpawn,*sMove_CurrentPtr,ENGINE_TRUE,ENGINE_FALSE);//в главной линии делаем поиск с полным окном
   }    
   Move_LegalMoveOff(*sMove_CurrentPtr,old_figure_type,ismoved,eat_cell);
   move_counter++;
   if (score>ret)
   {
    if (score>alpha)
    { 
     alpha=score;
     //добавляем в эвристику истории этот ход, как хороший
	 if (sMove_CurrentPtr->IsEat==ENGINE_FALSE && sMove_CurrentPtr->MoveType==MOVE_TYPE_SIMPLY)
	 {
      FIGURE_TYPE ft=Board[sMove_CurrentPtr->Coord256_1]&MASK_TYPE;
      History_AddGoodMove(color,ft,sMove_CurrentPtr->Coord256_2,depth);
	 }
     if (score>=beta)
     {
      if (sMove_CurrentPtr->IsEat==ENGINE_FALSE && sMove_CurrentPtr->MoveType==MOVE_TYPE_SIMPLY)//запоминаем ход Killer
      {
       sMove_Killer[level]=*sMove_CurrentPtr;
      }
      //запоминаем ход в хэше
      if (alpha<beta) Hash_RecordHash(level,alpha,HASH_TABLE_TYPE_EXACT,HKey_Local,sMove_CurrentPtr);
                 else Hash_RecordHash(level,alpha,HASH_TABLE_TYPE_BETA,HKey_Local,sMove_CurrentPtr);
      return(alpha);
     }     
     if (PVNode==ENGINE_TRUE)//пишем основной вариант
     {
      sMove_PrincipalVariation[level]=*sMove_CurrentPtr;
      sMove_PrincipalVariation[level].Coord256_PassedPawn=coord256_passedpawn;
     }
    } 
    ret=alpha;
   }     
  }
  sMove_CurrentPtr=sMove_CurrentPtr->sMove_NextPtr;
 } 
 if (move_counter==0)//походить не удалось
 {  
  //а не шах ли королю?
  if (check_king==ENGINE_TRUE) alpha=-WEIGHT_INFINITY+level;//это мат
                          else alpha=0;//это пат
 }
 if (alpha>ret) ret=alpha;
 if (ret==old_alpha) Hash_RecordHash(level,ret,HASH_TABLE_TYPE_ALPHA,HKey_Local,NULL);
 return(ret);
}
//----------------------------------------------------------------------------------------------------
//функция обработки взятий
//----------------------------------------------------------------------------------------------------
long Search_Quies(long alpha,long beta,long depth,long level,FIGURE_COLOR color,COORD coord256_passedpawn,SMove sMove_Last,ENGINE_BOOL PVNode,ENGINE_BOOL NullMove)
{
 //отсечка взятий ладьей
 #define QUEEN_MARGIN         (WEIGHT_QUEEN+2*WEIGHT_PAWN)
 //отсечка взятий ладьей
 #define ROOK_MARGIN          (WEIGHT_ROOK+2*WEIGHT_PAWN)
 //отсечка взятий  слонов и коней (если слон дороже, то нужно использовать WEIGHT_BISHOP)
 #define BISHOP_KNIGHT_MARGIN (WEIGHT_KNIGHT+2*WEIGHT_PAWN)
 //отсечка взятий  пешек
 #define PAWN_MARGIN          (WEIGHT_PAWN+2*WEIGHT_PAWN)

 if (StopSearch==ENGINE_TRUE) return(alpha);
 Positions++;
 if (level>MaxLevel) MaxLevel=level;
 if (Move_CheckEatKing(color)==ENGINE_TRUE) return(-WEIGHT_INFINITY+level);//случайно съели короля 
 FIGURE_COLOR next_color=color^(WHITE|BLACK);
 if (Move_CheckEatKing(next_color)==ENGINE_TRUE) return(WEIGHT_INFINITY-level);//случайно съели короля 
 if (level<MAX_LEVEL)//в случае шаха вызываем полный поиск
 {
  ENGINE_BOOL check_king=Move_CheckKing(color);
  if (check_king==ENGINE_TRUE) return(Search_AlphaBeta(alpha,beta,1,level,color,coord256_passedpawn,sMove_Last,PVNode,NullMove));
 } 
 long evaluate=Score_Evaluate(color);
 if (evaluate>alpha) 
 {
  if (evaluate>=beta) return(evaluate);
  alpha=evaluate;
 }
 if (level>=MAX_PLY) return(alpha);
 
 //делаем Delta pruning
 long evaluate_max=evaluate;  
 if (evaluate<(alpha-PAWN_MARGIN))//исключаем взятия пешек
 {
  evaluate_max=evaluate+PAWN_MARGIN;
  if (evaluate<(alpha-BISHOP_KNIGHT_MARGIN))//исключаем взятие слонов и коней
  {
   evaluate_max=evaluate+BISHOP_KNIGHT_MARGIN;
   if (evaluate<(alpha-ROOK_MARGIN))//исключаем взятие ладей
   {
    evaluate_max=evaluate+ROOK_MARGIN;
    if (evaluate<(alpha-QUEEN_MARGIN))//исключаем взятие ферзей
    {
     evaluate_max=evaluate+QUEEN_MARGIN;
    }
   }
  }
 }
 SMoveKit sMoveKit;
 sMoveKit.sMove_Hash.Coord256_1=0;
 sMoveKit.sMove_Hash.Coord256_2=0;
 sMoveKit.sMove_Killer=sMove_Killer[level];
 sMoveKit.sMove_Last=sMove_Last;
 sMoveKit.sMove_PrincipalVariation=sMove_PrincipalVariation[level];
  
 long move_eat=0;
 SMove *sMove_FirstEatPtr=NULL;
 //создаём все возможные ходы фигурами
 if (color==WHITE)
 {
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureWhiteCoord256[n];
   if (coord256==0) continue;//фигуры нет   
   Move_CreateEatMove(coord256,sMove_EatLevel[level],move_eat,coord256_passedpawn,sMove_FirstEatPtr,&sMoveKit);
  }
 }
 else
 {
  for(long n=0;n<16;n++)
  {
   COORD coord256=FigureBlackCoord256[n];
   if (coord256==0) continue;//фигуры нет   
   Move_CreateEatMove(coord256,sMove_EatLevel[level],move_eat,coord256_passedpawn,sMove_FirstEatPtr,&sMoveKit);
  }
 }
 CELL ismoved;
 CELL eat_cell;
 FIGURE_TYPE old_figure_type;
  
 //выполняем ходы взятия
 SMove *sMove_CurrentPtr;
 sMove_CurrentPtr=sMove_FirstEatPtr;
 while(sMove_CurrentPtr!=NULL)
 {
  if (alpha>=beta) return(alpha);  
  if (Move_LegalMoveOn(*sMove_CurrentPtr,old_figure_type,ismoved,eat_cell,coord256_passedpawn)==ENGINE_TRUE)//ход допустим
  {
   long score=-Search_Quies(-beta,-alpha,depth-1,level+1,next_color,coord256_passedpawn,*sMove_CurrentPtr,PVNode,NullMove);
   Move_LegalMoveOff(*sMove_CurrentPtr,old_figure_type,ismoved,eat_cell);
   if (score>evaluate_max)
   {
    if (score>alpha)
    {
     if (score>=beta) return(score);    
     alpha=score; 
     if (alpha<beta && PVNode==ENGINE_TRUE)
     {
      sMove_PrincipalVariation[level]=*sMove_CurrentPtr;
      sMove_PrincipalVariation[level].Coord256_PassedPawn=coord256_passedpawn;
     }
    }
    evaluate_max=score;
   }  
  }
  sMove_CurrentPtr=sMove_CurrentPtr->sMove_NextPtr;
 }
 return(alpha);
}
//----------------------------------------------------------------------------------------------------
//поиск лучшего хода
//----------------------------------------------------------------------------------------------------
SMove Search_Search(FIGURE_COLOR color,COORD coord256_passedpawn,long &positions,long &levels,long &score_value,long max_depth)
{ 
 long n;
 //инициализируем таблицу стоимости фигур
 //long weight_hight[]={0,WEIGHT_KING,WEIGHT_QUEEN+WEIGHT_PAWN,WEIGHT_ROOK+(WEIGHT_PAWN>>3),WEIGHT_BISHOP+(WEIGHT_PAWN>>4),WEIGHT_KNIGHT+(WEIGHT_PAWN>>4),WEIGHT_PAWN+(WEIGHT_PAWN>>5)};
 //long weight_hight[7]={0,WEIGHT_KING,WEIGHT_QUEEN+WEIGHT_PAWN,WEIGHT_ROOK,WEIGHT_BISHOP,WEIGHT_KNIGHT,WEIGHT_PAWN};
 long weight_hight[7]={0,WEIGHT_KING,WEIGHT_QUEEN,WEIGHT_ROOK,WEIGHT_BISHOP,WEIGHT_KNIGHT,WEIGHT_PAWN};
 long weight_normal[7]={0,WEIGHT_KING,WEIGHT_QUEEN,WEIGHT_ROOK,WEIGHT_BISHOP,WEIGHT_KNIGHT,WEIGHT_PAWN};
 for(n=1;n<7;n++)
 {
  sEvaluate.B_WeightFromType[n]=weight_normal[n];
  sEvaluate.W_WeightFromType[n]=weight_normal[n];
  if (color==BLACK) sEvaluate.B_WeightFromType[n]=weight_hight[n];
  if (color==WHITE) sEvaluate.W_WeightFromType[n]=weight_hight[n];
 }

 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
/*
  //для теста надо отключить в оценке рокировку у королей и оценку позиции пешек

  for(long s=0;s<10000;s++)
  {
   //очищаем доску
   for(n=0;n<256;n++) Board[n]=BYTE8(1,0,0,0,0,0,0,0);
   //заполняем центр доски
   for(n=0;n<64;n++) Board[Coord64to256[n]]=CELL_EMPTY;
   //задаём все фиугры
   for(n=0;n<16;n++)
   {
    FigureWhiteCoord256[n]=0;
    FigureBlackCoord256[n]=0;
   }

   long x[32];
   long y[32];
   for(n=0;n<32;n++)
   {
    while(1)
	{
     long x_f=rand()%8;
	 long y_f=rand()%8;	 
	 bool ok=true;
	 for(long m=0;m<n;m++)
	 { 
      if (x[m]==x_f && y[m]==y_f)
	  {
       ok=false;
	   break;
	  }
	 }
     if (ok==true)
	 {
      x[n]=x_f;
	  y[n]=y_f;
	  break;
	 }
	}
   }
   //за белых

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
   //белые
   Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'A'+x[0],'1'+y[0],WHITE);
   Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'A'+x[1],'1'+y[1],WHITE);
   Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[2],'1'+y[2],WHITE);
   Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[3],'1'+y[3],WHITE);
   Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[4],'1'+y[4],WHITE);
   Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[5],'1'+y[5],WHITE);
   Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[6],'1'+y[6],WHITE);
   Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[7],'1'+y[7],WHITE);
   for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+x[8+n],'1'+y[8+n],WHITE);
     
   //чёрные
   Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'A'+x[16],'1'+y[16],BLACK);
   Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'A'+x[17],'1'+y[17],BLACK);
   Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[18],'1'+y[18],BLACK);
   Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[19],'1'+y[19],BLACK);
   Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[20],'1'+y[20],BLACK);
   Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[21],'1'+y[21],BLACK);
   Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[22],'1'+y[22],BLACK);
   Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[23],'1'+y[23],BLACK);
   for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+x[24+n],'1'+y[24+n],BLACK);

   {
 FILE *file=fopen("state.txt","wb");
 fwrite(Board,sizeof(CELL),256,file);
 fwrite(FigureWhiteCoord256,sizeof(COORD),16,file);
 fwrite(FigureBlackCoord256,sizeof(COORD),16,file); 
 fclose(file);
   }

   long w_score=Score_Evaluate(WHITE);

   //за чёрных
   for(n=0;n<16;n++)
   {
    long copy=y[n];
	y[n]=7-y[n+16];
	y[n+16]=7-copy;

	copy=x[n];
	x[n]=7-x[n+16];
	x[n+16]=7-copy;
   }
  

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
   //белые
   Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'A'+x[0],'1'+y[0],WHITE);
   Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'A'+x[1],'1'+y[1],WHITE);
   Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[2],'1'+y[2],WHITE);
   Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[3],'1'+y[3],WHITE);
   Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[4],'1'+y[4],WHITE);
   Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[5],'1'+y[5],WHITE);
   Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[6],'1'+y[6],WHITE);
   Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[7],'1'+y[7],WHITE);
   for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+x[8+n],'1'+y[8+n],WHITE);
   
  
   //чёрные
   Move_SetFigureState(0,FIGURE_TYPE_KING,ENGINE_FALSE,'A'+x[16],'1'+y[16],BLACK);
   Move_SetFigureState(1,FIGURE_TYPE_QUEEN,ENGINE_FALSE,'A'+x[17],'1'+y[17],BLACK);
   Move_SetFigureState(2,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[18],'1'+y[18],BLACK);
   Move_SetFigureState(3,FIGURE_TYPE_ROOK,ENGINE_FALSE,'A'+x[19],'1'+y[19],BLACK);
   Move_SetFigureState(4,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[20],'1'+y[20],BLACK);
   Move_SetFigureState(5,FIGURE_TYPE_BISHOP,ENGINE_FALSE,'A'+x[21],'1'+y[21],BLACK);
   Move_SetFigureState(6,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[22],'1'+y[22],BLACK);
   Move_SetFigureState(7,FIGURE_TYPE_KNIGHT,ENGINE_FALSE,'A'+x[23],'1'+y[23],BLACK);
   for(n=0;n<8;n++) Move_SetFigureState(8+n,FIGURE_TYPE_PAWN,ENGINE_FALSE,'A'+x[24+n],'1'+y[24+n],BLACK);

   {
 FILE *file=fopen("state1.txt","wb");
 fwrite(Board,sizeof(CELL),256,file);
 fwrite(FigureWhiteCoord256,sizeof(COORD),16,file);
 fwrite(FigureBlackCoord256,sizeof(COORD),16,file); 
 fclose(file);
   }


   long b_score=Score_Evaluate(BLACK);

   if (b_score!=w_score)
   {
	printf("%i Error! WS:%i BS:%i\r\n",s,w_score,b_score);
	break;
   }

  }
 printf("End.\r\n");
 SMove sMove_Best1;
 sMove_Best1.Coord256_1=0;
 sMove_Best1.Coord256_2=0;
 return(sMove_Best1);
*/
  


 /*
 //тестируем оценочную функцию на симметрию
 for(long k=0;k<16;k++)
 {
  COORD coord256_w_1=FigureWhiteCoord256[k];
  COORD coord64_w_1=Coord256to64[coord256_w_1];
  long b_x1=coord64_w_1&7;
  long b_y1=coord64_w_1>>3;

  long w_x1=b_x1;
  long w_y1=7-b_y1;
  COORD coord64_b_1=w_x1|(w_y1<<3);
  COORD coord256_b_1=Coord64to256[coord64_b_1];

  for(long x=0;x<8;x++)
  {
   for(long y=0;y<8;y++)
   {
    long w_x2=x;
    long w_y2=y;

    long b_x2=x;
    long b_y2=7-y;

    long score_w=0;
    long score_b=0;
    COORD coord64_w_2=(w_y2<<3)|w_x2;
    COORD coord256_w_2=Coord64to256[coord64_w_2];

    COORD coord64_b_2=(b_y2<<3)|b_x2;
    COORD coord256_b_2=Coord64to256[coord64_b_2];

    if (Board[coord256_w_2]==CELL_EMPTY && Board[coord256_b_2]==CELL_EMPTY)
    {
     long index=0;
     index=(Board[coord256_w_1]&MASK_INDEX)>>INDEX_SHIFT;
     Board[coord256_w_2]=Board[coord256_w_1];
     Board[coord256_w_1]=CELL_EMPTY;
     FigureWhiteCoord256[index]=coord256_w_2;
     score_w=Score_Evaluate(WHITE);
     Board[coord256_w_1]=Board[coord256_w_2];
     Board[coord256_w_2]=CELL_EMPTY;
     FigureWhiteCoord256[index]=coord256_w_1;
         
     index=(Board[coord256_b_1]&MASK_INDEX)>>INDEX_SHIFT;
     Board[coord256_b_2]=Board[coord256_b_1];
     Board[coord256_b_1]=CELL_EMPTY;
     FigureBlackCoord256[index]=coord256_b_2;
     score_b=Score_Evaluate(WHITE);
     Board[coord256_b_1]=Board[coord256_b_2];
     Board[coord256_b_2]=CELL_EMPTY;
     FigureBlackCoord256[index]=coord256_b_1;
     
     score_b=-score_b;

     if (score_b!=score_w)
     {
      printf("Error! W:%i B:%i ",score_w,score_b);
      printf("%c%c-%c%c  ",'A'+w_x1,'1'+w_y1,'A'+w_x2,'1'+w_y2);
      printf("%c%c-%c%c\r\n",'A'+b_x1,'1'+b_y1,'A'+b_x2,'1'+b_y2);
      printf("----------\r\n");
     }
    }
   }
  }
 }
 printf("End.\r\n");
 SMove sMove_Best1;
 sMove_Best1.Coord256_1=0;
 sMove_Best1.Coord256_2=0;
 return(sMove_Best1);
 */
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------------

 SMove sMove_Last;
 sMove_Last.Coord256_1=0;
 sMove_Last.Coord256_2=0;
 Positions=0;
 MaxLevel=0;
 for(n=0;n<MAX_PLY+5;n++) 
 {
  sMove_Killer[n].Coord256_1=0;
  sMove_Killer[n].Coord256_2=0;
  sMove_PrincipalVariation[n].Coord256_1=0;
  sMove_PrincipalVariation[n].Coord256_2=0;
 }
 for(n=0;n<MAX_HASH_TABLE_SIZE;n++) sHTable[n].HashType=HASH_TABLE_TYPE_EMPTY;
 HKey=Hash_GetHKey();//узнаём хэш-ключ позиции
 long depth=1;
 StopSearch=ENGINE_FALSE;
 SMove sMove_Best;
 History_Reset();
 while(depth<=MAX_LEVEL)
 {
  sMove_Last.Coord256_1=0;
  sMove_Last.Coord256_2=0;
  sMove_Last.IsEat=ENGINE_FALSE;
  HKey=Hash_GetHKey();//узнаём хэш-ключ позиции
  for(n=0;n<MAX_HASH_TABLE_SIZE;n++)
  {
   if (sHTable[n].HashType==HASH_TABLE_TYPE_BETA || sHTable[n].HashType==HASH_TABLE_TYPE_EXACT) sHTable[n].HashType=HASH_TABLE_TYPE_OLD;
   if (sHTable[n].HashType==HASH_TABLE_TYPE_ALPHA) sHTable[n].HashType=HASH_TABLE_TYPE_EMPTY;
  }  
  score_value=Search_AlphaBeta(-WEIGHT_INFINITY,WEIGHT_INFINITY,depth,0,color,coord256_passedpawn,sMove_Last,ENGINE_TRUE,ENGINE_FALSE); 
  if (StopSearch==ENGINE_TRUE) break;
  sMove_Best=sMove_PrincipalVariation[0];
  long score=static_cast<long>(100.0f*(static_cast<float>(score_value)/static_cast<float>(WEIGHT_PAWN)));
  printf("info score cp %i ",score);
  printf("depth %i ",depth);
  printf("seldepth %i ",MaxLevel);  
  printf("nodes %i ",Positions);
  printf("pv ");
  for(long p=0;p<depth;p++)
  {
   COORD coord64_1=Coord256to64[sMove_PrincipalVariation[p].Coord256_1];
   COORD coord64_2=Coord256to64[sMove_PrincipalVariation[p].Coord256_2];
   long x1=coord64_1&7;
   long y1=coord64_1>>3;
   long x2=coord64_2&7;
   long y2=coord64_2>>3;
   printf("%c%c%c%c ",'a'+x1,'1'+y1,'a'+x2,'1'+y2);
  }
  printf("\n");
  if (score_value>=WEIGHT_INFINITY-MAX_PLY || score_value<=-WEIGHT_INFINITY+MAX_PLY) break;  
  if (max_depth>0 && depth>=max_depth) break;
  depth++;
 }
 if (HKey!=Hash_GetHKey()) printf("HKey error!\n");
 positions=Positions;
 levels=MaxLevel;
 return(sMove_Best);
}
 
