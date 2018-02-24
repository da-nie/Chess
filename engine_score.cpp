#include "engine_score.h"
#include "engine_move.h"
 
extern CELL Board[256];//��������� ����� � ����� ���������� (16x16)
 
extern COORD FigureWhiteCoord256[16];//������� ����� ����� �� ����� (��� �������� ������� � �������. 0- ������ ���)
extern COORD FigureBlackCoord256[16];//������� ������ ����� �� ����� (��� �������� ������� � �������. 0- ������ ���)
extern COORD *KingWhitePointer;//��������� �� ������ � ������� ������� �����
extern COORD *KingBlackPointer;//��������� �� ������ � ������� ������� ������
 
//��������� ��� ������ �������
SEvaluate sEvaluate;
 
//----------------------------------------------------------------------------------------------------
//���� �����
//----------------------------------------------------------------------------------------------------
extern long KingMove[9];//���� ������
extern long QueenMove[9];//���� �����
extern long RookMove[5];//���� �����
extern long BishopMove[5];//���� �����
extern long KnightMove[9];//���� ����
//----------------------------------------------------------------------------------------------------
//������� ������������� ���������
//----------------------------------------------------------------------------------------------------
extern COORD Coord256to64[256];
extern COORD Coord64to256[64];
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//������ ����� �� ������� ����� (�.�. Y ���������� � �������� ������� ��� ������� �������)
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
 
//������ ������� ����� � ������ ���� � ������ � ������
const long PawnBeginAndKingInCenterScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,181,181,0,0,0,
 -543,-181,0,181,181,0,-181,-543,
 -543,-181,0,181,181,0,-181,-543,
 -543,-181,0,255,255,0,-181,-543,
 -543,-181,0,181,181,0,-181,-543,
 -543,-181,0,181,181,0,-181,-543,
 -543,-181,0,181,181,0,-181,-543
}; 
//������ ������� ����� � ������ ���� � ������ ������
const long PawnBeginAndKingInRightScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,181,181,0,0,0,
 0,0,0,181,181,0,-181,-543,
 0,0,0,181,181,0,-181,-543,
 0,0,0,255,255,0,-181,-543,
 0,0,0,181,181,0,-181,-543,
 0,0,0,181,181,0,-181,-543,
 0,0,0,181,181,0,-181,-543
};
//������ ������� ����� � ������ ���� � ������ �����
const long PawnBeginAndKingInLeftScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
 -543,-181,0,181,181,0,0,0,
};
 
//������ ������� ����� � ����� ����
const long PawnEndScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,181,181,0,0,0,
 0,0,0,181,181,0,0,0,
 0,0,0,181,181,0,0,0,
 0,0,0,255,255,0,0,0,
 0,0,0,255,255,0,0,0,
 0,0,0,255,255,0,0,0,
 0,0,0,255,255,0,0,0
};
//������ ��������� ������
const long PawnPerspectiveEndScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,               
 0,0,0,0,0,0,0,0,     
 181,181,181,181,181,181,181,181,
 255,255,255,255,255,255,255,255,
 1600,1600,1600,1600,1600,1600,1600,1600,
 3200,3200,3200,3200,3200,3200,3200,3200,
 0,0,0,0,0,0,0,0  
};


//������ ������� ���� � ������ ����
const long KnightBeginScore[64]=    
{
 -3492,-2798,-2104,-1757,-1757,-2104,-2798,-3492,
 -2440,-1746,-1052, -705, -705,-1052,-1746,-2440,
 -1388,-694,0,347,347,0,-694,-1388,
 -683,11,705,1052,1052,705,11,-683,
 -325,369,1063,1410,1410,1063,369,-325,
 -314,380,1074,1421,1421,1074,380,-314,
 -1366,-672,22,369,369,22,-672,-1366,
 -5618,-1724,-1030,-683,-683,-1030,-1724,-5618
};
//������ ������� ���� � ����� ����
const long KnightEndScore[64]=    
{
 -448,-336,-224,-168,-168,-224,-336,-448,
 -336,-224,-112, -56, -56,-122,-224,-336,
 -224,-112,   0,  56,  56,   0,-112,-224,
 -168, -56,  56, 112, 112,  56, -56,-168,
 -168, -56,  56, 112, 112,  56, -56,-168,
 -224,-112,   0,  56,  56,   0,-112,-224,
 -336,-224,-112, -56, -56,-122,-224,-336,
 -448,-336,-224,-168,-168,-224,-336,-448
};
//������ ������� ����� � ������ ����
const long BishopBeginScore[64]=
{
 -755,-839,-692,-545,-545,-692,-839,-755,
 -588,  84,-147,   0,   0,-147,  84,-588,
 -441,-147, 378, 147, 147, 378,-147,-441,
 -294,   0, 147, 672, 672, 147,   0,-294,
 -294,   0, 147, 672, 672, 147,   0,-294,
 -441,-147, 378, 147, 147, 378,-147,-441,
 -588,  84,-147,   0,   0,-147,  84,-588,
 -504,-588,-441,-294,-294,-441,-588,-504
};
//������ ������� ����� � ����� ����
const long BishopEndScore[64]=
{
 -294,-196,-147,-98,-98,-147,-196,-294,
 -196,  98,-49,   0,  0,-49,  -98,-196,
 -147, -49,  0,  49, 49,  0,  -49,-147,
 - 98,   0, 49,  98, 98, 49,    0, -98,
 - 98,   0, 49,  98, 98, 49,    0, -98,
 -147, -49,  0,  49, 49,  0,  -49,-147,
 -196,  98,-49,   0,  0,-49,  -98,-196,
 -294,-196,-147,-98,-98,-147,-196,-294
};
//������ ������� ����� � ������ ����
const long RookBeginScore[64]=
{
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208,
 -208,-104,0,104,104,0,-104,-208
};
//������ ������� ����� � ����� ����
const long RookEndScore[64]=
{
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0
};
//������ ������� ����� � ������ ����
const long QueenBeginScore[64]=
{
 -789,-593,-495,-397,-397,-495,-593,-789,
 -392,-196, -98,   0,   0, -98,-196,-392,
 -294, -98,   0,  98,  98,   0, -98,-294,
 -196,   0,  98, 196, 196,  98,   0,-196,
 -196,   0,  98, 196, 196,  98,   0,-196,
 -294, -98,   0,  98,  98,   0, -98,-294,
 -392,-196, -98,   0,   0, -98,-196,-392,
 -588,-392,-294,-196,-196,-294,-392,-588
};
//������ ������� ����� � ����� ����
const long QueenEndScore[64]=
{
 -648,-432,-324,-216,-216,-324,-432,-648,
 -432,-216,-108,   0,   0,-108,-216,-432,
 -324,-108,   0, 108, 108,   0,-108,-324,
 -216,   0, 108, 216, 216, 108,   0,-216,
 -216,   0, 108, 216, 216, 108,   0,-216,
 -324,-108,   0, 108, 108,   0,-108,-324,
 -432,-216,-108,   0,   0,-108,-216,-432,
 -648,-432,-324,-216,-216,-324,-432,-648
};
//������ ������� ������ � ������ ����
const long KingBeginScore[64]=
{
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407,
 1407,1876,938,0,0,938,1876,1407
};
//������ ������� ������ � ����� ����
const long KingEndScore[64]=
{
 -2406,-1604,-1203,-802,-802,-1203,-1604,-2406,
 -1604,-802,-401,0,0,-401,-802,-1604,
 -1203,-401,0,401,401,0,-401,-1203,
 -802,0,401,802, 802,401,0,-802,
 -802,0,401,802, 802,401,0,-802,
 -1203,-401,0,401,401,0,-401,-1203,
 -1604,-802,-401,0,0,-401,-802,-1604,
 -2406,-1604,-1203,-802,-802,-1203,-1604,-2406
};
 
//----------------------------------------------------------------------------------------------------
//�������������
//----------------------------------------------------------------------------------------------------
void Score_Init(void)
{
}
//----------------------------------------------------------------------------------------------------
//���������������
//----------------------------------------------------------------------------------------------------
void Score_Release(void)
{
}
//----------------------------------------------------------------------------------------------------
//������� ���� ���� ������ � ���������� ���������� �����
//----------------------------------------------------------------------------------------------------
inline void Score_CreateFigureSpace(COORD coord256)
{
 long n;
 CELL cell=Board[coord256]; 
 FIGURE_COLOR color=cell&MASK_COLOR;
 FIGURE_COLOR opponent_color=color^(WHITE|BLACK);
 FIGURE_TYPE type=cell&MASK_TYPE;
 long index=(cell&MASK_INDEX)>>INDEX_SHIFT;
 long *move_ptr=&sEvaluate.B_MoveFromType[type];
 long *eat_ptr=&sEvaluate.B_EatFromType[type];
 CELL_EAT *board_ptr=sEvaluate.B_EatBoard;
 long *move_amount_ptr=&sEvaluate.B_MoveAmountFromIndex[index];
 COORD *coord256_move_ptr=sEvaluate.B_Coord256MoveFromIndex[index];
 if (color==WHITE)
 {  
  move_ptr=&sEvaluate.W_MoveFromType[type];
  eat_ptr=&sEvaluate.W_EatFromType[type];
  board_ptr=sEvaluate.W_EatBoard;
  move_amount_ptr=&sEvaluate.W_MoveAmountFromIndex[index];
  coord256_move_ptr=sEvaluate.W_Coord256MoveFromIndex[index];
 }
 //--------------------------------------------------
 //������
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KING)
 {
  n=0;
  while(KingMove[n]!=0)
  {
   COORD c256=coord256+KingMove[n];
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)
   {
    (*eat_ptr)++;
    board_ptr[c256]|=CELL_EAT_MASK_KING;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
   }
   if (cell==0) 
   {
    (*move_ptr)++;
    board_ptr[c256]|=CELL_EAT_MASK_KING;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //�����
 //--------------------------------------------------
 if (type==FIGURE_TYPE_QUEEN)
 {
  n=0;
  while(QueenMove[n]!=0)
  {
   COORD c256=coord256+QueenMove[n];    
   while(Board[c256]==0)//���� ����� ������
   {
    board_ptr[c256]|=CELL_EAT_MASK_QUEEN;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    (*move_ptr)++;
    c256+=QueenMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)
   {
    (*eat_ptr)++;
    board_ptr[c256]|=CELL_EAT_MASK_QUEEN;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //�����
 //--------------------------------------------------
 if (type==FIGURE_TYPE_ROOK)
 {
  n=0;
  while(RookMove[n]!=0)
  {
   COORD c256=coord256+RookMove[n];    
   while(Board[c256]==0)//���� ����� ������
   {
    (*move_ptr)++;
    board_ptr[c256]|=CELL_EAT_MASK_ROOK;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    c256+=RookMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color) 
   {
    board_ptr[c256]|=CELL_EAT_MASK_ROOK;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    (*eat_ptr)++;
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //����
 //--------------------------------------------------
 if (type==FIGURE_TYPE_BISHOP)
 {
  n=0;
  while(BishopMove[n]!=0)
  {
   COORD c256=coord256+BishopMove[n];    
   while(Board[c256]==0)//���� ����� ������
   {
    (*move_ptr)++;
    board_ptr[c256]|=CELL_EAT_MASK_BISHOP;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    c256+=BishopMove[n];
   }
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color)
   {
    board_ptr[c256]|=CELL_EAT_MASK_BISHOP;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    (*eat_ptr)++;
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //����
 //--------------------------------------------------
 if (type==FIGURE_TYPE_KNIGHT)
 {
  n=0;
  while(KnightMove[n]!=0)
  {
   COORD c256=coord256+KnightMove[n];    
   cell=Board[c256];
   if ((cell&MASK_COLOR)==opponent_color) 
   {
    board_ptr[c256]|=CELL_EAT_MASK_KNIGHT;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    (*eat_ptr)++;
   }
   if (cell==0) 
   {
    board_ptr[c256]|=CELL_EAT_MASK_KNIGHT;
    coord256_move_ptr[*move_amount_ptr]=c256;
    (*move_amount_ptr)++;
    (*move_ptr)++;
   }
   n++;
  }
  return;
 }
 //--------------------------------------------------
 //�����
 //--------------------------------------------------
 if (type==FIGURE_TYPE_PAWN)
 {  
  if (color==WHITE)//���� ��� ����� �����
  {
   //��������, ����� �� ����
   cell=Board[coord256-1+16];
   if ((cell&MASK_COLOR)==opponent_color) (*eat_ptr)++;
   if ((cell&MASK_COLOR)!=color) board_ptr[coord256-1+16]|=CELL_EAT_MASK_PAWN;
   cell=Board[coord256+1+16];
   if ((cell&MASK_COLOR)==opponent_color) (*eat_ptr)++;
   if ((cell&MASK_COLOR)!=color) board_ptr[coord256+1+16]|=CELL_EAT_MASK_PAWN;
  }
  else//���� ��� ������ �����
  {
   //��������, ����� �� ����
   cell=Board[coord256-1-16];
   if ((cell&MASK_COLOR)==opponent_color) (*eat_ptr)++;
   if ((cell&MASK_COLOR)!=color) board_ptr[coord256-1-16]|=CELL_EAT_MASK_PAWN;
   cell=Board[coord256+1-16];
   if ((cell&MASK_COLOR)==opponent_color) (*eat_ptr)++;
   if ((cell&MASK_COLOR)!=color) board_ptr[coord256+1-16]|=CELL_EAT_MASK_PAWN;
  }
  return;
 }
 return;
}
 
//----------------------------------------------------------------------------------------------------
//������� ��������� � ���������� �������� �����, ����������� ���� ����
//----------------------------------------------------------------------------------------------------
float Score_GetMaterialScoreAndCreateSpace(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0;

 long b_score=0;
 long w_score=0;

 COORD coord256;
 CELL cell;
 long white_rook_amount=0;//���������� ����� �����
 long white_bishop_amount=0;//���������� ����� ������
 long white_knight_amount=0;//���������� ����� �����
 long white_queen_amount=0;//���������� ����� ������
 long white_pawn_amount=0;//���������� ����� �����

 long black_rook_amount=0;//���������� ������ �����
 long black_bishop_amount=0;//���������� ������ ������
 long black_knight_amount=0;//���������� ������ �����
 long black_queen_amount=0;//���������� ������ ������
 long black_pawn_amount=0;//���������� ����� �����

 for(long n=0;n<16;n++)
 {  
  //�����
  coord256=FigureWhiteCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   w_score+=sEvaluate.W_WeightFromType[type];
   //������� ������
   if (type==FIGURE_TYPE_ROOK) white_rook_amount++;
   if (type==FIGURE_TYPE_BISHOP) white_bishop_amount++;
   if (type==FIGURE_TYPE_KNIGHT) white_knight_amount++;
   if (type==FIGURE_TYPE_QUEEN) white_queen_amount++;
   if (type==FIGURE_TYPE_PAWN) white_pawn_amount++;
   //���������� ����� ������������ �������
   Score_CreateFigureSpace(coord256);
  }
  //������
  coord256=FigureBlackCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   b_score+=sEvaluate.B_WeightFromType[type];
   //������� ������
   if (type==FIGURE_TYPE_ROOK) black_rook_amount++;
   if (type==FIGURE_TYPE_BISHOP) black_bishop_amount++;
   if (type==FIGURE_TYPE_KNIGHT) black_knight_amount++;
   if (type==FIGURE_TYPE_QUEEN) black_queen_amount++;
   if (type==FIGURE_TYPE_PAWN) black_pawn_amount++;
   //���������� ����� ������������ �������
   Score_CreateFigureSpace(coord256);
  }
 }
 //���������� ���� ���� 
 long current_material=b_score+w_score-sEvaluate.W_WeightFromType[FIGURE_TYPE_KING]-sEvaluate.B_WeightFromType[FIGURE_TYPE_KING];//������� �������� ��� ������� 
 long begin_material=(sEvaluate.W_WeightFromType[FIGURE_TYPE_PAWN]+sEvaluate.B_WeightFromType[FIGURE_TYPE_PAWN])*8;//�����
 begin_material+=(sEvaluate.W_WeightFromType[FIGURE_TYPE_KNIGHT]+sEvaluate.B_WeightFromType[FIGURE_TYPE_KNIGHT])*2;//����
 begin_material+=(sEvaluate.W_WeightFromType[FIGURE_TYPE_BISHOP]+sEvaluate.B_WeightFromType[FIGURE_TYPE_BISHOP])*2;//�����
 begin_material+=(sEvaluate.W_WeightFromType[FIGURE_TYPE_ROOK]+sEvaluate.B_WeightFromType[FIGURE_TYPE_ROOK])*2;//�����
 begin_material+=(sEvaluate.W_WeightFromType[FIGURE_TYPE_QUEEN]+sEvaluate.B_WeightFromType[FIGURE_TYPE_QUEEN])*1;//�����
 float phase=1;//���� ����
 if (current_material<=begin_material) phase=static_cast<float>(current_material)/static_cast<float>(begin_material);
 if (phase<0) phase=0;//�� ������, ���� ������ ����� � ��� ������� ��-���� ����� ������� (���� ���� �� ������)

 b_begin_score=b_score;
 w_begin_score=w_score;
 b_end_score=b_score;
 w_end_score=w_score;

 //���� ����� �� ��������  
 #define PAWN_BEGIN_SCORE                  2176
 //���� ����� � ��������
 #define PAWN_END_SCORE                    3400
 //����� �� ���� (��� ������) ������
 #define BISHOP_PAIR_BEGIN_SCORE           (32*952)
 #define BISHOP_PAIR_END_SCORE             (32*1496)
 //����� ����� � ������
 #define BISHOP_AND_PAWN_MAT_SCORE         (32*96)
 //����� �� ���� ������, ������� ��� � ���������
 #define BISHOP_PAIR_VS_NO_LIGHT_SCORE     (32*160)
 //����� �� ���� (��� ������) �����
 #define ROOK_PAIR_BEGIN_SCORE             (32*448)
 #define ROOK_PAIR_END_SCORE               (32*896)
 //����� �� ����� � �����
 #define QUEEN_AND_ROOK_BEGIN_SCORE        (32*224)
 #define QUEEN_AND_ROOK_END_SCORE          (32*448)
 //����� �� ���� � �����
 #define KNIGHT_AND_PAWN_MAT_SCORE         (32*160)
 //����� �� ����� � �����
 #define ROOK_AND_PAWN_MAT_SCORE           (32*160)


 //��������� ����������� ������ �� ��������� �����
 long w_special_begin_score=0;
 long b_special_begin_score=0;
 long w_special_end_score=0;
 long b_special_end_score=0;
 //�����
 if (white_bishop_amount>1)//����� �� ���� (��� ������) ������
 {
  w_special_begin_score+=BISHOP_PAIR_BEGIN_SCORE;
  w_special_end_score+=BISHOP_PAIR_END_SCORE;
  b_special_end_score-=BISHOP_AND_PAWN_MAT_SCORE*(white_pawn_amount-5);
  if (black_bishop_amount==0)
  {
   w_special_begin_score+=BISHOP_PAIR_VS_NO_LIGHT_SCORE;
   w_special_end_score+=BISHOP_PAIR_VS_NO_LIGHT_SCORE;
  }
 }
 if (white_rook_amount>1)//����� �� ���� (��� ������) �����
 {
  w_special_begin_score+=ROOK_PAIR_BEGIN_SCORE;
  w_special_end_score+=ROOK_PAIR_END_SCORE;
 }
 if (white_rook_amount+white_queen_amount>1)//����� �� ����� � �����
 {
  w_special_begin_score+=QUEEN_AND_ROOK_BEGIN_SCORE;
  w_special_end_score+=QUEEN_AND_ROOK_END_SCORE;
 }
 //����� �� ����� � �����
 w_special_begin_score-=white_rook_amount*(white_pawn_amount-5)*ROOK_AND_PAWN_MAT_SCORE;  
 //������ �� ���� � �����
 w_special_end_score-=white_knight_amount*(white_pawn_amount-5)*KNIGHT_AND_PAWN_MAT_SCORE;

 //������
 if (black_bishop_amount>1)//����� �� ���� (��� ������) ������
 {
  b_special_begin_score+=BISHOP_PAIR_BEGIN_SCORE;
  b_special_end_score+=BISHOP_PAIR_END_SCORE;
  b_special_end_score-=BISHOP_AND_PAWN_MAT_SCORE*(black_pawn_amount-5);
  if (black_bishop_amount==0)
  {
   b_special_begin_score+=BISHOP_PAIR_VS_NO_LIGHT_SCORE;
   b_special_end_score+=BISHOP_PAIR_VS_NO_LIGHT_SCORE;
  }
 }
 if (black_rook_amount>1)//����� �� ���� (��� ������) �����
 {
  b_special_begin_score+=ROOK_PAIR_BEGIN_SCORE;
  b_special_end_score+=ROOK_PAIR_END_SCORE;
 }
 if (black_rook_amount+black_queen_amount>1)//����� �� ����� � �����
 {
  b_special_begin_score+=QUEEN_AND_ROOK_BEGIN_SCORE;
  b_special_end_score+=QUEEN_AND_ROOK_END_SCORE;
 }
 //����� �� ����� � �����
 b_special_begin_score-=black_rook_amount*(black_pawn_amount-5)*ROOK_AND_PAWN_MAT_SCORE;  
 //������ �� ���� � �����
 b_special_end_score-=black_knight_amount*(black_pawn_amount-5)*KNIGHT_AND_PAWN_MAT_SCORE; 
 
 
 w_special_begin_score/=PAWN_BEGIN_SCORE;
 b_special_begin_score/=PAWN_BEGIN_SCORE;
 w_special_end_score/=PAWN_END_SCORE;
 b_special_end_score/=PAWN_END_SCORE;
 
 w_begin_score+=w_special_begin_score;
 b_begin_score+=b_special_begin_score;
 w_end_score+=w_special_end_score;
 b_end_score+=b_special_end_score;

 return(phase);
}
//----------------------------------------------------------------------------------------------------
//������� ������ �� �������
//----------------------------------------------------------------------------------------------------
void Score_GetPositionScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0;

 COORD coord256;
 CELL cell;
 //���������� ��������� �������, �� ����� ������� ������� ��� �����
 COORD coord64_w_king=Coord256to64[*KingWhitePointer];
 COORD coord64_b_king=Coord256to64[*KingBlackPointer];
 long x_w_king=coord64_w_king&7;
 long x_b_king=coord64_b_king&7;
 long y_w_king=coord64_w_king>>3;
 long y_b_king=coord64_b_king>>3;
 for(long n=0;n<16;n++)
 {  
  //�����
  coord256=FigureWhiteCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   COORD coord64=Coord256to64[coord256];
   if (type==FIGURE_TYPE_KING)
   {
    w_begin_score+=KingBeginScore[coord64]; 
    w_end_score+=KingEndScore[coord64]; 
   }
   if (type==FIGURE_TYPE_PAWN)
   {
    w_end_score+=PawnEndScore[coord64];
    ENGINE_BOOL center=ENGINE_TRUE;
    if (x_w_king<3)
    {
     w_begin_score+=PawnBeginAndKingInLeftScore[coord64];
     center=ENGINE_FALSE;
    }
    if (x_w_king>4)
    {
     w_begin_score+=PawnBeginAndKingInRightScore[coord64];
     center=ENGINE_FALSE;
    }
    if (center==ENGINE_TRUE) w_begin_score+=PawnBeginAndKingInCenterScore[coord64];
   }
   if (type==FIGURE_TYPE_BISHOP)
   {
    w_begin_score+=BishopBeginScore[coord64];
    w_end_score+=BishopEndScore[coord64];
   }
   if (type==FIGURE_TYPE_KNIGHT)
   {
    w_begin_score+=KnightBeginScore[coord64];
    w_end_score+=KnightEndScore[coord64];
   }
   if (type==FIGURE_TYPE_QUEEN)
   {
    w_begin_score+=QueenBeginScore[coord64];
    w_end_score+=QueenEndScore[coord64];
   }
   if (type==FIGURE_TYPE_ROOK) 
   {
    w_begin_score+=RookBeginScore[coord64];    
    w_end_score+=RookEndScore[coord64];    
   }
  }
  //������
  coord256=FigureBlackCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   COORD coord64=Coord256to64[coord256];
   coord64^=0x38;
   if (type==FIGURE_TYPE_KING)
   {
    b_begin_score+=KingBeginScore[coord64];
    b_end_score+=KingEndScore[coord64];
   }   
   if (type==FIGURE_TYPE_PAWN) 
   {
    b_end_score+=PawnEndScore[coord64];
    ENGINE_BOOL center=ENGINE_TRUE;
    if (x_b_king<3)
    {
     b_begin_score+=PawnBeginAndKingInLeftScore[coord64];
     center=ENGINE_FALSE;
    }
    if (x_b_king>4) 
    {
     b_begin_score+=PawnBeginAndKingInRightScore[coord64];
     center=ENGINE_FALSE;
    }
    if (center==ENGINE_TRUE) b_begin_score+=PawnBeginAndKingInCenterScore[coord64];
   }
   if (type==FIGURE_TYPE_BISHOP) 
   {
    b_begin_score+=BishopBeginScore[coord64];
    b_end_score+=BishopEndScore[coord64];
   }
   if (type==FIGURE_TYPE_KNIGHT)
   {
    b_begin_score+=KnightBeginScore[coord64];
    b_end_score+=KnightEndScore[coord64];
   }
   if (type==FIGURE_TYPE_QUEEN) 
   {
    b_begin_score+=QueenBeginScore[coord64];
    b_end_score+=QueenEndScore[coord64];
   }
   if (type==FIGURE_TYPE_ROOK)
   {
    b_begin_score+=RookBeginScore[coord64];
    b_end_score+=RookEndScore[coord64];
   }
  }
 }
}
//----------------------------------------------------------------------------------------------------
//������ �� ������ ������������
//----------------------------------------------------------------------------------------------------
void Score_GetSpaceScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_QUEEN_OPENING  54
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_ROOK_OPENING   84
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_BISHOP_OPENING 149
 //����������� ������� ������������ �����
 #define CONTROL_SQUARE_KNIGHT_OPENING 14
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_QUEEN_ENDGAME  37
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_ROOK_ENDGAME   79
 //����������� ������� ������������ ������
 #define CONTROL_SQUARE_BISHOP_ENDGAME 116
 //����������� ������� ������������ �����
 #define CONTROL_SQUARE_KNIGHT_ENDGAME 121

 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0;

 //�������!!!
 //����� ��� ������ ����� ���� ��� ����, �� ����� ����� ������, ���� ������ �����������!
 //����� ������� ������ �� ��������� ��� �� ����� ������.

 static long space_begin_score[FIGURE_TYPE_PAWN+1]={0,0,CONTROL_SQUARE_QUEEN_OPENING,CONTROL_SQUARE_ROOK_OPENING,CONTROL_SQUARE_BISHOP_OPENING,CONTROL_SQUARE_KNIGHT_OPENING,0};
 static long space_end_score[FIGURE_TYPE_PAWN+1]={0,0,CONTROL_SQUARE_QUEEN_ENDGAME,CONTROL_SQUARE_ROOK_ENDGAME,CONTROL_SQUARE_BISHOP_ENDGAME,CONTROL_SQUARE_KNIGHT_ENDGAME,0};
 for(long n=FIGURE_TYPE_QUEEN;n<FIGURE_TYPE_PAWN;n++)
 {
  w_begin_score+=sEvaluate.W_MoveFromType[n]*space_begin_score[n];
  b_begin_score+=sEvaluate.B_MoveFromType[n]*space_begin_score[n];

  w_end_score+=sEvaluate.W_MoveFromType[n]*space_end_score[n];
  b_end_score+=sEvaluate.B_MoveFromType[n]*space_end_score[n];
 }
}
//----------------------------------------------------------------------------------------------------
//������ �� ��������� ������
//----------------------------------------------------------------------------------------------------
void Score_GetKingScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
  
 //����� �� ���� �������� ���������
 #define PENALTY_CASTLING_O_O 960
 //����� �� ���� ������� ���������
 #define PENALTY_CASTLING_O_O_O 640
 //����������� ����� �� ������ ������
 #define ATAK_KING_QUEEN 532
 //����������� ����� �� ������ ������
 #define ATAK_KING_ROOK 665
 //����������� ����� �� ������ ������
 #define ATAK_KING_BISHOP 418
 //����������� ����� �� ������ �����
 #define ATAK_KING_KNIGHT 941
 //����������� ����� �� ������ ������
 #define ATAK_KING_PAWN   0
 //����������� �� ���
 #define CHECK_SCORE      252
  
 COORD coord256_w_king=*KingWhitePointer;
 COORD coord256_b_king=*KingBlackPointer;
 CELL cell_w_king=Board[coord256_w_king];
 CELL cell_b_king=Board[coord256_b_king];

 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0; 
 //--------------------------------------------------
 //����� ������
 //--------------------------------------------------
 if ((cell_w_king&MASK_IS_MOVED)==0)//������ �� �����
 {
  //������� ����� ��� ������� ���������
  CELL cell_rook_left=Board[coord256_w_king-4];
  if ((cell_rook_left&(MASK_COLOR|MASK_TYPE))==(MASK_WHITE|FIGURE_TYPE_ROOK))//����� ����� �� �����
  {   
   if ((cell_rook_left&MASK_IS_MOVED)!=0) w_begin_score-=PENALTY_CASTLING_O_O_O;//����� �� ���� ������� ��������� 
  }
  else w_begin_score-=PENALTY_CASTLING_O_O_O;//����� �� ���� ������� ���������
  //������� ����� ��� �������� ���������
  CELL cell_rook_right=Board[coord256_w_king+3];
  if ((cell_rook_right&(MASK_COLOR|MASK_TYPE))==(MASK_WHITE|FIGURE_TYPE_ROOK))//������ ����� �� �����
  {   
   if ((cell_rook_right&MASK_IS_MOVED)!=0) w_begin_score-=PENALTY_CASTLING_O_O;//����� �� ���� �������� ��������� 
  }
  else w_begin_score-=PENALTY_CASTLING_O_O;//����� �� ���� �������� ���������
 }
 else//������ �����
 {
  if ((cell_w_king&MASK_CASTLING)==0)//��������� �� ����
  {
   w_begin_score-=PENALTY_CASTLING_O_O;
   w_begin_score-=PENALTY_CASTLING_O_O_O;
  }
 }

 //--------------------------------------------------
 //������ ������
 //--------------------------------------------------

 if ((cell_b_king&MASK_IS_MOVED)==0)//������ �� �����
 {
  //������� ����� ��� ������� ���������
  CELL cell_rook_left=Board[coord256_b_king-4];
  if ((cell_rook_left&(MASK_COLOR|MASK_TYPE))==(MASK_BLACK|FIGURE_TYPE_ROOK))//����� ����� �� �����
  {   
   if ((cell_rook_left&MASK_IS_MOVED)!=0) b_begin_score-=PENALTY_CASTLING_O_O_O;//����� �� ���� ������� ��������� 
  }
  else b_begin_score-=PENALTY_CASTLING_O_O_O;//����� �� ���� ������� ���������
  //������� ����� ��� �������� ���������
  CELL cell_rook_right=Board[coord256_b_king+3];
  if ((cell_rook_right&(MASK_COLOR|MASK_TYPE))==(MASK_BLACK|FIGURE_TYPE_ROOK))//������ ����� �� �����
  {   
   if ((cell_rook_right&MASK_IS_MOVED)!=0) b_begin_score-=PENALTY_CASTLING_O_O;//����� �� ���� �������� ��������� 
  }
  else b_begin_score-=PENALTY_CASTLING_O_O;//����� �� ���� �������� ���������
 }
 else//������ �����
 {
  if ((cell_b_king&MASK_CASTLING)==0)//��������� �� ����
  {
   b_begin_score-=PENALTY_CASTLING_O_O;
   b_begin_score-=PENALTY_CASTLING_O_O_O;
  }
 }

 //������� ����� �� ������
 //������������� ���� ��� ������ � ��������� ������ ����� �� ������ ������
 COORD coord256;
 CELL cell;
 long n;
 for(n=0;n<16;n++)
 {   
  //������
  coord256=FigureBlackCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   CELL_EAT cell_eat_mask=CELL_EAT_NONE;
   if (type==FIGURE_TYPE_QUEEN) cell_eat_mask=CELL_EAT_MASK_QUEEN;
   if (type==FIGURE_TYPE_ROOK) cell_eat_mask=CELL_EAT_MASK_ROOK;
   if (type==FIGURE_TYPE_BISHOP) cell_eat_mask=CELL_EAT_MASK_BISHOP;
   if (type==FIGURE_TYPE_KNIGHT) cell_eat_mask=CELL_EAT_MASK_KNIGHT;
   if (type==FIGURE_TYPE_PAWN) cell_eat_mask=CELL_EAT_MASK_PAWN;
   long cells=0;
   if ((sEvaluate.B_EatBoard[coord256_w_king]&cell_eat_mask)!=CELL_EAT_NONE)//���
   {
    b_begin_score+=CHECK_SCORE;
    b_end_score+=CHECK_SCORE;
   }
   if (((sEvaluate.B_EatBoard[coord256_w_king-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king-16]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king-16]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king+16]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king+16]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king-16-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king-16-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king-16+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king-16+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king+16-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king+16-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.B_EatBoard[coord256_w_king+16+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_w_king+16+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (cells>0)
   {
    if (type==FIGURE_TYPE_QUEEN)
    {
     b_begin_score+=ATAK_KING_QUEEN;
     b_end_score+=ATAK_KING_QUEEN;
    } 
    if (type==FIGURE_TYPE_ROOK)
    {
     b_begin_score+=ATAK_KING_ROOK;
     b_end_score+=ATAK_KING_ROOK;
    }
    if (type==FIGURE_TYPE_BISHOP)
    {
     b_begin_score+=ATAK_KING_BISHOP;
     b_end_score+=ATAK_KING_BISHOP;
    }
    if (type==FIGURE_TYPE_KNIGHT)
    {
     b_begin_score+=ATAK_KING_KNIGHT;
     b_end_score+=ATAK_KING_KNIGHT;
    }
    if (type==FIGURE_TYPE_PAWN)
    {
     b_begin_score+=ATAK_KING_PAWN;
     b_end_score+=ATAK_KING_PAWN;
    }
   }
  }
 }
 //������������� ���� ��� ����� � ��������� ������ ����� �� ������� ������
 for(n=0;n<16;n++)
 {   
  //�����
  coord256=FigureWhiteCoord256[n];
  if (coord256!=0)
  {    
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   CELL_EAT cell_eat_mask=CELL_EAT_NONE;
   if (type==FIGURE_TYPE_QUEEN) cell_eat_mask=CELL_EAT_MASK_QUEEN;
   if (type==FIGURE_TYPE_ROOK) cell_eat_mask=CELL_EAT_MASK_ROOK;
   if (type==FIGURE_TYPE_BISHOP) cell_eat_mask=CELL_EAT_MASK_BISHOP;
   if (type==FIGURE_TYPE_KNIGHT) cell_eat_mask=CELL_EAT_MASK_KNIGHT;
   if (type==FIGURE_TYPE_PAWN) cell_eat_mask=CELL_EAT_MASK_PAWN;
   long cells=0;
   if ((sEvaluate.W_EatBoard[coord256_b_king]&cell_eat_mask)!=CELL_EAT_NONE)//���
   {
    w_begin_score+=CHECK_SCORE;
    w_end_score+=CHECK_SCORE;
   }
   if (((sEvaluate.W_EatBoard[coord256_b_king-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king-16]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king-16]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king+16]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king+16]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king-16-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king-16-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king-16+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king-16+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king+16-1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king+16-1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (((sEvaluate.W_EatBoard[coord256_b_king+16+1]&cell_eat_mask)!=CELL_EAT_NONE) && Board[coord256_b_king+16+1]==CELL_EMPTY) cells++;//��� ������ ������� ������
   if (cells>0)
   {
    if (type==FIGURE_TYPE_QUEEN)
    {
     w_begin_score+=ATAK_KING_QUEEN;
     w_end_score+=ATAK_KING_QUEEN;
    } 
    if (type==FIGURE_TYPE_ROOK)
    {
     w_begin_score+=ATAK_KING_ROOK;
     w_end_score+=ATAK_KING_ROOK;
    }
    if (type==FIGURE_TYPE_BISHOP)
    {
     w_begin_score+=ATAK_KING_BISHOP;
     w_end_score+=ATAK_KING_BISHOP;
    }
    if (type==FIGURE_TYPE_KNIGHT)
    {
     w_begin_score+=ATAK_KING_KNIGHT;
     w_end_score+=ATAK_KING_KNIGHT;
    }
    if (type==FIGURE_TYPE_PAWN)
    {
     w_begin_score+=ATAK_KING_PAWN;
     w_end_score+=ATAK_KING_PAWN;
    }
   }
  }
 }
}
//----------------------------------------------------------------------------------------------------
//������ �� ��������� �����
//----------------------------------------------------------------------------------------------------
void Score_GetPawnScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
 //����� �� ��������� �����
 #define PENALTY_PAWN 158
 //��������� ����� ����� �� ������ ��������� (�������� 3)
 #define PERSPECTIVE_PAWN 320
 
 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0;

 COORD coord256;
 CELL cell;
 ENGINE_BOOL w_double_pawn=ENGINE_FALSE;
 ENGINE_BOOL b_double_pawn=ENGINE_FALSE;
 for(long n=0;n<16;n++)
 {  
  //�����
  coord256=FigureWhiteCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   if (type==FIGURE_TYPE_PAWN)//��� �����
   {
    //������� �����
    if ((Board[coord256-1]&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|WHITE)) w_double_pawn=ENGINE_TRUE;
    //������� ������
    if ((Board[coord256+1]&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|WHITE)) w_double_pawn=ENGINE_TRUE;
    //�������, ��������� ��� ����� ��� ���
    COORD coord64=Coord256to64[coord256];
    long y=coord64>>3;
    long x=coord64&7;
    ENGINE_BOOL line_empty[3]={ENGINE_TRUE,ENGINE_TRUE,ENGINE_TRUE};
    coord256+=16;
    y++;
    for(;y<8;y++,coord256+=16)
    {
     if ((Board[coord256-1]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|BLACK)) line_empty[0]=ENGINE_FALSE;
     if ((Board[coord256+0]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|BLACK))//����� ����� �� ���������
     {     
      line_empty[1]=ENGINE_FALSE;
      break;
     }
     if ((Board[coord256+1]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|BLACK)) line_empty[2]=ENGINE_FALSE;     
     if (line_empty[0]==ENGINE_FALSE && line_empty[1]==ENGINE_FALSE && line_empty[2]==ENGINE_FALSE) break;
    }	
    if (line_empty[1]==ENGINE_TRUE)//������ ��������
    {     
     w_end_score+=PERSPECTIVE_PAWN;
	 w_begin_score+=PERSPECTIVE_PAWN;
     if (line_empty[0]==ENGINE_TRUE)
     {
      w_end_score+=PERSPECTIVE_PAWN;
	  w_begin_score+=PERSPECTIVE_PAWN;
     }
     if (line_empty[2]==ENGINE_TRUE)
     {      
      w_end_score+=PERSPECTIVE_PAWN;
	  w_begin_score+=PERSPECTIVE_PAWN;
     }     
    }
    //������ �� ����������� ������ ��������� ������
    if (x>=1 && x<=6)//����� � ������
    {
     if (line_empty[0]==ENGINE_TRUE && line_empty[1]==ENGINE_TRUE && line_empty[2]==ENGINE_TRUE)
     {
      w_end_score+=PawnPerspectiveEndScore[coord64];
	  w_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }    
    if (x==0)//����� �����
    {
     if (line_empty[1]==ENGINE_TRUE && line_empty[2]==ENGINE_TRUE)
     {
      w_end_score+=PawnPerspectiveEndScore[coord64];
	  w_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }    
    if (x==7)//����� ������
    {
     if (line_empty[0]==ENGINE_TRUE && line_empty[1]==ENGINE_TRUE) 
     {
      w_end_score+=PawnPerspectiveEndScore[coord64];
	  w_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }
   }   
  }
  //������
  coord256=FigureBlackCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   if (type==FIGURE_TYPE_PAWN)//��� �����
   {
    //������� �����
    if ((Board[coord256-1]&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|BLACK)) b_double_pawn=ENGINE_TRUE;
    //������� ������
    if ((Board[coord256+1]&(MASK_TYPE|MASK_COLOR))==(FIGURE_TYPE_PAWN|BLACK)) b_double_pawn=ENGINE_TRUE;
    //�������, ��������� ��� ����� ��� ���
    COORD coord64=Coord256to64[coord256];
    long y=coord64>>3;
    long x=coord64&7;
    coord64^=0x38;
    ENGINE_BOOL line_empty[3]={ENGINE_TRUE,ENGINE_TRUE,ENGINE_TRUE};
    coord256-=16;
    y--;
    for(;y>=0;y--,coord256-=16)
    {
     if ((Board[coord256-1]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|WHITE)) line_empty[0]=ENGINE_FALSE;
     if ((Board[coord256+0]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|WHITE))//����� ����� �� ���������
     {     
      line_empty[1]=ENGINE_FALSE;
      break;
     }
     if ((Board[coord256+1]&(MASK_COLOR|MASK_TYPE))==(FIGURE_TYPE_PAWN|WHITE)) line_empty[2]=ENGINE_FALSE;     
     if (line_empty[0]==ENGINE_FALSE && line_empty[1]==ENGINE_FALSE && line_empty[2]==ENGINE_FALSE) break;
    }	
    if (line_empty[1]==ENGINE_TRUE)//������ ��������
    {
     b_end_score+=PERSPECTIVE_PAWN;
	 b_begin_score+=PERSPECTIVE_PAWN;
     if (line_empty[0]==ENGINE_TRUE) 
     {
      b_end_score+=PERSPECTIVE_PAWN;
	  b_begin_score+=PERSPECTIVE_PAWN;
     }
     if (line_empty[2]==ENGINE_TRUE) 
     {
      b_end_score+=PERSPECTIVE_PAWN;
	  b_begin_score+=PERSPECTIVE_PAWN;
     }
    }
    //������ �� ����������� ������ ��������� ������
    if (x>=1 && x<=6)//����� � ������
    {
     if (line_empty[0]==ENGINE_TRUE && line_empty[1]==ENGINE_TRUE && line_empty[2]==ENGINE_TRUE) 
     {
      b_end_score+=PawnPerspectiveEndScore[coord64];
	  b_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }    
    if (x==0)//����� �����
    {
     if (line_empty[1]==ENGINE_TRUE && line_empty[2]==ENGINE_TRUE) 
     {      
      b_end_score+=PawnPerspectiveEndScore[coord64];
	  b_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }    
    if (x==7)//����� ������
    {
     if (line_empty[0]==ENGINE_TRUE && line_empty[1]==ENGINE_TRUE) 
     {
      b_end_score+=PawnPerspectiveEndScore[coord64];
	  b_begin_score+=PawnPerspectiveEndScore[coord64];
     }
    }
   }
  }
 }
 if (w_double_pawn==ENGINE_TRUE)
 {
  w_begin_score-=PENALTY_PAWN;
  w_end_score-=PENALTY_PAWN;
 }
 if (b_double_pawn==ENGINE_TRUE)
 {
  b_begin_score-=PENALTY_PAWN;
  b_end_score-=PENALTY_PAWN;
 }
}
//----------------------------------------------------------------------------------------------------
//������ �� �������
//----------------------------------------------------------------------------------------------------
void Score_GetTrapDoorScore(long &w_begin_score,long &b_begin_score,long &w_end_score,long &b_end_score)
{
 //�� ������� ����������� ���� � �����
 //��� ���� ����������� ������ ���� ����� ������� ����������
 //����� �� ���������� �����
 #define PENALTY_BISHOP_TRAP_DOOR (73*32)
 //����� �� ��������� �����
 #define PENALTY_BISHOP_LOCKED    (73*32)
 //����� �� �������� �����
 #define PENALTY_ROOK_LOCKED      (73*32)

 b_begin_score=0;
 w_begin_score=0;
 b_end_score=0;
 w_end_score=0;

 COORD coord256;
 CELL cell;
 for(long n=0;n<16;n++)
 {  
  //�����
  coord256=FigureWhiteCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   if (type==FIGURE_TYPE_BISHOP || type==FIGURE_TYPE_ROOK)
   {
    //�������, ������ �� ����, ���� ����� ������
    long index=(cell&MASK_INDEX)>>INDEX_SHIFT;
    long amount=sEvaluate.W_MoveAmountFromIndex[index];
    long free_cell=0;//��������� ����
    for(long m=0;m<amount;m++)
    {
     COORD coord256_move=sEvaluate.W_Coord256MoveFromIndex[index][m];//����������, ���� ����� �������� ������
     if ((sEvaluate.B_EatBoard[coord256_move]&CELL_EAT_MASK_PAWN)==CELL_EAT_NONE)//���� �� ��������� �������
     {
      free_cell++;
      break;
     }
    }
    if (free_cell==0)//������ �������
    {
     if (amount>0)
	 {
      if (type==FIGURE_TYPE_BISHOP) w_begin_score-=PENALTY_BISHOP_LOCKED;
      if (type==FIGURE_TYPE_ROOK) w_begin_score-=PENALTY_ROOK_LOCKED;
	 }
     if ((sEvaluate.B_EatBoard[coord256]&CELL_EAT_MASK_PAWN)!=CELL_EAT_NONE && type==FIGURE_TYPE_BISHOP) //������ ������������� ����� ���� �������
     {
      w_begin_score-=PENALTY_BISHOP_TRAP_DOOR;
      w_end_score-=PENALTY_BISHOP_TRAP_DOOR;
     }
    }
   }
  }
  //������
  coord256=FigureBlackCoord256[n];
  if (coord256!=0)
  {
   cell=Board[coord256];
   FIGURE_TYPE type=cell&MASK_TYPE;
   if (type==FIGURE_TYPE_BISHOP || type==FIGURE_TYPE_ROOK)
   {
    //�������, ������ �� ����, ���� ����� ������
	long index=(cell&MASK_INDEX)>>INDEX_SHIFT;
    long amount=sEvaluate.B_MoveAmountFromIndex[index];
    long free_cell=0;//��������� ����
    for(long m=0;m<amount;m++)
    {
     COORD coord256_move=sEvaluate.B_Coord256MoveFromIndex[index][m];//����������, ���� ����� �������� ������
     if ((sEvaluate.W_EatBoard[coord256_move]&CELL_EAT_MASK_PAWN)==CELL_EAT_NONE)//���� �� ��������� �������
     {
      free_cell++;
      break;
     }
    }
    if (free_cell==0)//������ �������
    {
     if (amount>0)
	 {
      if (type==FIGURE_TYPE_BISHOP) b_begin_score-=PENALTY_BISHOP_LOCKED;
      if (type==FIGURE_TYPE_ROOK) b_begin_score-=PENALTY_ROOK_LOCKED;
	 }
     if ((sEvaluate.W_EatBoard[coord256]&CELL_EAT_MASK_PAWN)!=CELL_EAT_NONE && type==FIGURE_TYPE_BISHOP) //������ ������������� ����� ���� �������
     {
      b_begin_score-=PENALTY_BISHOP_TRAP_DOOR;
      b_end_score-=PENALTY_BISHOP_TRAP_DOOR;
     }
    }    
   }
  }
 }
}
//----------------------------------------------------------------------------------------------------
//������ �������
//----------------------------------------------------------------------------------------------------
long Score_Evaluate(FIGURE_COLOR color)
{
 //�������������� ��� ��������� � �������� ������
 long n;     
 //������� ������� ������
 for(n=FIGURE_TYPE_KING;n<=FIGURE_TYPE_PAWN;n++)
 {
  sEvaluate.B_EatFromType[n]=0;
  sEvaluate.B_MoveFromType[n]=0;
  sEvaluate.W_EatFromType[n]=0;
  sEvaluate.W_MoveFromType[n]=0;
 }
 for(n=0;n<16;n++)
 {
  sEvaluate.B_MoveAmountFromIndex[n]=0;
  sEvaluate.W_MoveAmountFromIndex[n]=0;
 }
 //�������� ����� ����
 for(n=0;n<256;n++)
 {
  sEvaluate.B_EatBoard[n]=CELL_EAT_NONE;
  sEvaluate.W_EatBoard[n]=CELL_EAT_NONE;
 }
 //������� ��������,��������� ��������� ���������� ����� � ���������� ���� ����
 long b_material_begin_score=0;
 long w_material_begin_score=0;
 long b_material_end_score=0;
 long w_material_end_score=0;
 float phase=Score_GetMaterialScoreAndCreateSpace(w_material_begin_score,b_material_begin_score,w_material_end_score,b_material_end_score);
 //������� ����������� �����
 long b_position_begin_score=0;
 long w_position_begin_score=0;
 long b_position_end_score=0;
 long w_position_end_score=0;
 Score_GetPositionScore(w_position_begin_score,b_position_begin_score,w_position_end_score,b_position_end_score);
 //������� ������ �� ������ ������������
 long b_space_begin_score=0;
 long w_space_begin_score=0;
 long b_space_end_score=0;
 long w_space_end_score=0;
 Score_GetSpaceScore(w_space_begin_score,b_space_begin_score,w_space_end_score,b_space_end_score);
 //������� ������ �� �������
 long b_trap_door_begin_score=0;
 long w_trap_door_begin_score=0;
 long b_trap_door_end_score=0;
 long w_trap_door_end_score=0;
 Score_GetTrapDoorScore(w_trap_door_begin_score,b_trap_door_begin_score,w_trap_door_end_score,b_trap_door_end_score);
 //������� ������ �� ��������� �����
 long w_pawn_begin_score=0;
 long b_pawn_begin_score=0;
 long w_pawn_end_score=0;
 long b_pawn_end_score=0;
 Score_GetPawnScore(w_pawn_begin_score,b_pawn_begin_score,w_pawn_end_score,b_pawn_end_score);
 //������� ����� �� ��������� ������
 long w_king_begin_score=0; 
 long b_king_begin_score=0; 
 long w_king_end_score=0; 
 long b_king_end_score=0; 
 Score_GetKingScore(w_king_begin_score,b_king_begin_score,w_king_end_score,b_king_end_score);
 //������� �������� ������
 long begin_score=0;
 long end_score=0;
 begin_score+=(w_position_begin_score-b_position_begin_score);
 begin_score+=(w_space_begin_score-b_space_begin_score);
 begin_score+=(w_king_begin_score-b_king_begin_score);
 begin_score+=(w_pawn_begin_score-b_pawn_begin_score);
 begin_score+=(w_trap_door_begin_score-b_trap_door_begin_score);

 end_score+=(w_position_end_score-b_position_end_score);
 end_score+=(w_space_end_score-b_space_end_score);
 end_score+=(w_king_end_score-b_king_end_score);
 end_score+=(w_pawn_end_score-b_pawn_end_score);
 end_score+=(w_trap_door_end_score-b_trap_door_end_score);

 begin_score/=32;
 end_score/=32;

 begin_score+=w_material_begin_score-b_material_begin_score;
 end_score+=w_material_end_score-b_material_end_score;

 long score=static_cast<long>(static_cast<float>(begin_score)*phase+static_cast<float>(end_score)*(1.0-phase));
 if (color==BLACK) return(-score);
 return(score);
}
 
