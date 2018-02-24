#include "cdialog_main.h"

extern CELL Board[256];//шахматная доска
extern COORD Coord256to64[256];
extern COORD Coord64to256[64];
extern SMove sMove_PrincipalVariation[MAX_PLY+5];//главная линия ходов

//-Функции обработки сообщений класса----------------------------------------
BEGIN_MESSAGE_MAP(CDialog_Main,CDialog)
 ON_WM_DESTROY()
 ON_WM_PAINT() 
 ON_COMMAND(IDC_BUTTON_MOVE,OnCommand_Button_Move)
 ON_COMMAND(IDC_BUTTON_SAVE_STATE,OnCommand_Button_SaveState)
 ON_COMMAND(IDC_BUTTON_LOAD_STATE,OnCommand_Button_LoadState)
 ON_COMMAND(IDC_BUTTON_INIT_WHITE,OnCommand_Button_InitWhite)
 ON_COMMAND(IDC_BUTTON_INIT_BLACK,OnCommand_Button_InitBlack)
 ON_COMMAND(IDC_BUTTON_ROTATE_BOARD,OnCommand_Button_RotateBoard) 
 ON_WM_LBUTTONDOWN()
 ON_WM_RBUTTONDOWN() 
END_MESSAGE_MAP()
//-Конструктор класса--------------------------------------------------------
CDialog_Main::CDialog_Main(LPCTSTR lpszTemplateName, CWnd* pParentWnd):CDialog(lpszTemplateName,pParentWnd)
{
 int width;
 int height;
 for(long n=1;n<=26;n++)
 {
  char name[255];
  sprintf(name,"picture\\%i.tga",n);
  Figure[n]=LoadTGAFromFile(name,width,height);
  if (width!=36 || height!=36)
  {
   if (Figure[n]!=NULL) delete[](Figure[n]);
   Figure[n]=NULL;
  }
 }
 Horizontal=LoadTGAFromFile("picture\\horizontal.tga",width,height);
 Vertical=LoadTGAFromFile("picture\\vertical.tga",width,height);
 VerticalInvert=LoadTGAFromFile("picture\\vertical_invert.tga",width,height);
 HorizontalInvert=LoadTGAFromFile("picture\\horizontal_invert.tga",width,height);

 HumanColor=WHITE;
 ComputerColor=BLACK;
 char string[255];
 AddLog("--------------------------------------------------");
 AddLog("Новая игра за белых");
 SYSTEMTIME systemtime;
 GetLocalTime(&systemtime);
 sprintf(string,"%02i.%02i.%04i %02i:%02i:%02i",systemtime.wDay,systemtime.wMonth,systemtime.wYear,systemtime.wHour,systemtime.wMinute,systemtime.wSecond);
 AddLog(string); 
 AddLog("--------------------------------------------------");

 cDialog_SelectFigurePtr=new CDialog_SelectFigure((LPSTR)IDD_DIALOG_SELECT_FIGURE,this);
 InversionBoard=false;
 EngineRun();
}
//-Деструктор класса---------------------------------------------------------
CDialog_Main::~CDialog_Main()
{  
 EngineStop();
 for(long n=1;n<=26;n++)
 {
  if (Figure[n]!=NULL) delete[](Figure[n]);
 }
 if (Horizontal!=NULL) delete[](Horizontal);
 if (Vertical!=NULL) delete[](Vertical);
 if (VerticalInvert!=NULL) delete[](VerticalInvert);
 if (HorizontalInvert!=NULL) delete[](HorizontalInvert);

 delete(cDialog_SelectFigurePtr);
}
//-Замещённые функции предка-------------------------------------------------
afx_msg void CDialog_Main::OnOK(void)
{
}
afx_msg BOOL CDialog_Main::OnInitDialog(void)
{
 ((CEdit*)GetDlgItem(IDC_EDIT_MAIN_MOVE))->SetWindowText("");
 ((CEdit*)GetDlgItem(IDC_EDIT_MAIN_MOVE))->SetLimitText(5);
 Initialize();
 sMove.Coord256_PassedPawn=0;
 X1=-1;
 X2=-1;
 Y1=-1;
 Y2=-1;

 SelectX1=-1;
 SelectY1=-1;
 SelectX2=-1;
 SelectY2=-1;

 char string[255];
 AddText("Версия движка от 30.04.2015");
 AddText("Используемые эвристики и настройки:");
 AddText("  Альфа-бета с амортизацией отказов");
 AddText("  Late Move Reduction");
 AddText("  Продление взятий");
 AddText("  Продление шахов");
 AddText("  Эвристика убийцы");
 AddText("  Итеративное углубление");
 AddText("  Сортировка фигур MVV/LVA");
 //AddText("  + стоимость пешки за своего ферзя");
 AddText("  Таблица поле-фигура");
 AddText("  Функция оценки 2.1");
 AddText("  Нулевой ход с редукцией 4");
 AddText("  Futility pruning");
 AddText("  Razoring"); 
 AddText("  Хэш таблица");
 sprintf(string,"  Глубина анализа, полуходов %i",MAX_DEPTH);
 AddText(string);
 sprintf(string,"  Глубина продлений, полуходов %i",MAX_LEVEL);
 AddText(string);
 AddText("--------------------------------------------------");
 AddText("Рокировка задаётся ходом короля.");
 AddText("Взятие проходной пешки - ");
 AddText("указанием битого поля.");
 AddText("--------------------------------------------------");
 AddText("");
 AddText("");
 AddText("");

 return(CDialog::OnInitDialog());
}
//-Новые функции класса------------------------------------------------------
//-Функции обработки сообщений класса----------------------------------------
afx_msg void CDialog_Main::OnDestroy(void)
{
 CDialog::OnDestroy();
}
afx_msg void CDialog_Main::OnLButtonDown(UINT nFlags,CPoint point)
{
 CRect cRect;
 ((CStatic*)GetDlgItem(IDC_STATIC_MAIN_MAP))->GetWindowRect(&cRect);
 ClientToScreen(&point);

 float sizex=(cRect.right-cRect.left)/10.0f;
 float sizey=(cRect.bottom-cRect.top)/10.0f;

 point.x-=cRect.left;
 point.y-=cRect.top;

 SelectX1=(long)(point.x/sizex);
 SelectY1=(long)(point.y/sizey);
 SelectX1=SelectX1-1;
 SelectY1=9-SelectY1-1;
 SelectX2=-1;
 SelectY2=-1;

 if (InversionBoard==true) 
 {
  SelectY1=7-SelectY1;
  SelectX1=7-SelectX1;
 }
 InvalidateRect(NULL,FALSE);
}

afx_msg void CDialog_Main::OnRButtonDown(UINT nFlags,CPoint point)
{
 CRect cRect;
 ((CStatic*)GetDlgItem(IDC_STATIC_MAIN_MAP))->GetWindowRect(&cRect);
 ClientToScreen(&point);

 float sizex=(cRect.right-cRect.left)/10.0f;
 float sizey=(cRect.bottom-cRect.top)/10.0f;

 point.x-=cRect.left;
 point.y-=cRect.top;

 SelectX2=(long)(point.x/sizex);
 SelectY2=(long)(point.y/sizey);
 SelectX2=SelectX2-1;
 SelectY2=9-SelectY2-1;

 if (InversionBoard==true) 
 {
  SelectY2=7-SelectY2;
  SelectX2=7-SelectX2;
 }

 if (SelectX1>=0 && SelectX1<=7 && SelectY1>=0 && SelectY1<=7) 
 {
  if (SelectX2>=0 && SelectX2<=7 && SelectY2>=0 && SelectY2<=7) 
  {
   InvalidateRect(NULL,FALSE);
   MSG msg;
   while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//обработка сообщений Windows
   {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
   }
   if (Move(SelectX1,SelectY1,SelectX2,SelectY2)==true)
   {
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//обработка сообщений Windows
	{
     TranslateMessage(&msg);
     DispatchMessage(&msg);
	}
	ComputerAnswer();
   }   
  }
 } 
}
afx_msg void CDialog_Main::OnPaint(void)
{
 long x;
 long y; 
 CRect cRect;
 ((CStatic*)GetDlgItem(IDC_STATIC_MAIN_MAP))->GetClientRect(&cRect);
 CRect cRect_W;
 ((CStatic*)GetDlgItem(IDC_STATIC_MAIN_MAP))->GetWindowRect(&cRect_W);
 ScreenToClient(cRect_W);

 long width=cRect.right-cRect.left;
 long height=cRect.bottom-cRect.top;

 unsigned char *ImageBuffer=new unsigned char[(10*36)*(10*36)*3+1];
 memset(ImageBuffer,192,(10*36)*(10*36)*3+1);
 //рисуем шахматную доску
 for(x=0;x<8;x++)
 {
  for(y=0;y<8;y++)
  {
   long sprite=25;
   if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=25;
                                        else sprite=26;
   long type=Board[Coord64to256[(y<<3)|x]]&MASK_TYPE;
   long color=Board[Coord64to256[(y<<3)|x]]&MASK_COLOR;
   if (color==BLACK)
   {
    if (type==FIGURE_TYPE_PAWN)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=13;
                                          else sprite=19;
	}
    if (type==FIGURE_TYPE_KNIGHT)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=14;
                                          else sprite=20;
	}
    if (type==FIGURE_TYPE_BISHOP)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=15;
                                          else sprite=21;
	}
    if (type==FIGURE_TYPE_ROOK)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=16;
                                          else sprite=22;
	}
    if (type==FIGURE_TYPE_QUEEN)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=17;
                                          else sprite=23;
	}
    if (type==FIGURE_TYPE_KING)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=18;
                                          else sprite=24;
	}
   }
   if (color==WHITE)
   {
    if (type==FIGURE_TYPE_PAWN)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=1;
                                          else sprite=7;
	}
    if (type==FIGURE_TYPE_KNIGHT)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=2;
                                          else sprite=8;
	}
    if (type==FIGURE_TYPE_BISHOP)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=3;
                                          else sprite=9;
	}
    if (type==FIGURE_TYPE_ROOK)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=4;
                                          else sprite=10;
	}
    if (type==FIGURE_TYPE_QUEEN)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=5;
                                          else sprite=11;
	}
    if (type==FIGURE_TYPE_KING)
	{
     if (x%2==0 && y%2!=0 || x%2!=0 && y%2==0) sprite=6;
                                          else sprite=12;
	}
   }
   long select=0;
   if (x==X1 && y==Y1) select=1;
   if (x==X2 && y==Y2) select=1;

   if (x==SelectX1 && y==SelectY1) select=1;
   if (x==SelectX2 && y==SelectY2) select=1;
   long yb=y;
   long xb=x;
   if (InversionBoard==true) 
   {
	yb=7-y;
	xb=7-x;
   }
   if (Figure[sprite]!=NULL) DrawSprite((xb+1)*36,(yb+1)*36,36,36,Figure[sprite],10*36,10*36,ImageBuffer,select);
  }
 }
 if (InversionBoard==false)
 {
  if (Horizontal!=NULL) DrawSprite(1*36,0*36,36*8,36,Horizontal,10*36,10*36,ImageBuffer,false);
  if (Horizontal!=NULL) DrawSprite(1*36,9*36,36*8,36,Horizontal,10*36,10*36,ImageBuffer,false);
  if (Vertical!=NULL) DrawSprite(0*36,1*36,36,36*8,Vertical,10*36,10*36,ImageBuffer,false);
  if (Vertical!=NULL) DrawSprite(9*36,1*36,36,36*8,Vertical,10*36,10*36,ImageBuffer,false);
 }
 else
 {
  if (HorizontalInvert!=NULL) DrawSprite(1*36,0*36,36*8,36,HorizontalInvert,10*36,10*36,ImageBuffer,false);
  if (HorizontalInvert!=NULL) DrawSprite(1*36,9*36,36*8,36,HorizontalInvert,10*36,10*36,ImageBuffer,false);
  if (VerticalInvert!=NULL) DrawSprite(0*36,1*36,36,36*8,VerticalInvert,10*36,10*36,ImageBuffer,false);
  if (VerticalInvert!=NULL) DrawSprite(9*36,1*36,36,36*8,VerticalInvert,10*36,10*36,ImageBuffer,false);
 }
 
 //создаём изображение с использованием билинейной интерполяции

 unsigned char *BitmapImage=new unsigned char[width*height*3];
 unsigned char *b_ptr=BitmapImage;
 long original_width=10*36;
 long original_height=10*36;
 for(y=0;y<height;y++)
 {
  float tmp=static_cast<float>(y)/static_cast<float>(height-1)*static_cast<float>(original_height-1);
  long ys=static_cast<long>(floor(tmp));
  if (ys<0) ys=0;
  if (ys>=original_height-1) ys=original_height-2;
  float u=tmp-ys;
  for(x=0;x<width;x++,b_ptr+=3)
  {
   float tmp=static_cast<float>(x)/static_cast<float>(width-1)*static_cast<float>(original_width-1);
   long xs=static_cast<long>(floor(tmp));
   if (xs<0) xs=0;
   if (xs>=original_width-1) xs=original_width-2;
   float v=tmp-xs;
   //коэффициенты
   float d1=(1-v)*(1-u);
   float d2=v*(1-u);
   float d3=v*u;
   float d4=(1-v)*u;
   //окрестные пиксели
   for(long n=0;n<3;n++)
   {
    unsigned long p1=ImageBuffer[3*(xs+ys*original_width)+n];
    unsigned long p2=ImageBuffer[3*(xs+1+ys*original_width)+n];
    unsigned long p3=ImageBuffer[3*(xs+1+(ys+1)*original_width)+n];
    unsigned long p4=ImageBuffer[3*(xs+(ys+1)*original_width)+n];
    //вычисляем новое значение пикселя
    unsigned long p=static_cast<short>(p1*d1+p2*d2+p3*d3+p4*d4);
	if (p<0) p=0;
	if (p>255) p=255;
	*(b_ptr+n)=p;
   }
  }
 }


 CPaintDC dc(this);
 BITMAPINFOHEADER bmih;
 bmih.biSize=sizeof(BITMAPINFOHEADER);
 bmih.biWidth=width;
 bmih.biHeight=height;
 bmih.biPlanes=1;
 bmih.biBitCount=24;
 bmih.biCompression=BI_RGB;
 bmih.biSizeImage=0;
 bmih.biXPelsPerMeter=300;
 bmih.biYPelsPerMeter=300;
 bmih.biClrUsed=0;
 bmih.biClrImportant=0;
 RGBQUAD rgbq;
 BITMAPINFO info;
 rgbq.rgbBlue=1;
 rgbq.rgbGreen=0;
 rgbq.rgbRed=0;
 rgbq.rgbReserved=0;
 info.bmiHeader=bmih;
 info.bmiColors[0]=rgbq;
 StretchDIBits(dc.m_hDC,cRect_W.left,cRect_W.top,cRect.right,cRect.bottom,0,0,width,height,BitmapImage,&info,DIB_RGB_COLORS,SRCCOPY);
 
 delete[](ImageBuffer);
 delete[](BitmapImage);
 CDialog::OnPaint();
}
afx_msg void CDialog_Main::OnCommand_Button_SaveState(void)
{
 SaveState();
}
afx_msg void CDialog_Main::OnCommand_Button_LoadState(void)
{
 LoadState();
 InvalidateRect(NULL,FALSE);
}
afx_msg void CDialog_Main::OnCommand_Button_InitBlack(void)
{
 char string[255];
 AddLog("--------------------------------------------------");
 AddLog("Новая игра за чёрных");
 SYSTEMTIME systemtime;
 GetLocalTime(&systemtime);
 sprintf(string,"%02i.%02i.%04i %02i:%02i:%02i",systemtime.wDay,systemtime.wMonth,systemtime.wYear,systemtime.wHour,systemtime.wMinute,systemtime.wSecond);
 AddLog(string); 
 AddLog("--------------------------------------------------");

 HumanColor=BLACK;
 ComputerColor=WHITE;
 Initialize();
 sMove.Coord256_PassedPawn=0;
 X1=-1;
 X2=-1;
 Y1=-1;
 Y2=-1;

 SelectX1=-1;
 SelectY1=-1;
 SelectX2=-1;
 SelectY2=-1;

 InvalidateRect(NULL,FALSE);
 MSG msg;
 while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//обработка сообщений Windows
 {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
 }
 //вызываем расчёт хода компьютером 
 ComputerAnswer();
 InvalidateRect(NULL,FALSE);
}
afx_msg void CDialog_Main::OnCommand_Button_InitWhite(void)
{
 char string[255];
 AddLog("--------------------------------------------------");
 AddLog("Новая игра за белых");
 SYSTEMTIME systemtime;
 GetLocalTime(&systemtime);
 sprintf(string,"%02i.%02i.%04i %02i:%02i:%02i",systemtime.wDay,systemtime.wMonth,systemtime.wYear,systemtime.wHour,systemtime.wMinute,systemtime.wSecond);
 AddLog(string); 
 AddLog("--------------------------------------------------");

 HumanColor=WHITE;
 ComputerColor=BLACK;
 Initialize();
 sMove.Coord256_PassedPawn=0; 
 X1=-1;
 X2=-1;
 Y1=-1;
 Y2=-1;

 SelectX1=-1;
 SelectY1=-1;
 SelectX2=-1;
 SelectY2=-1;

 InvalidateRect(NULL,FALSE);
}
afx_msg void CDialog_Main::OnCommand_Button_RotateBoard(void)
{
 if (InversionBoard==true) InversionBoard=false;
                      else InversionBoard=true;
 InvalidateRect(NULL,FALSE);
}

afx_msg void CDialog_Main::OnCommand_Button_Move(void)
{ 
 unsigned char string[255];
 ((CEdit*)GetDlgItem(IDC_EDIT_MAIN_MOVE))->GetWindowText((char*)string,255);
 if (string[2]!='-') return;
 long x1=-1;
 long y1=-1;
 if (string[0]>='a' && string[0]<='h') x1=string[0]-'a';
 if (string[0]>='A' && string[0]<='H') x1=string[0]-'A';
 if (string[1]>='1' && string[1]<='8') y1=string[1]-'1';
 long x2=-1;
 long y2=-1;
 if (string[3]>='a' && string[3]<='h') x2=string[3]-'a';
 if (string[3]>='A' && string[3]<='H') x2=string[3]-'A';
 if (string[4]>='1' && string[4]<='8') y2=string[4]-'1';
 if (x1<0 || y1<0 || x2<0 || y2<0) return;
 bool res=Move(x1,y1,x2,y2);
 MSG msg;
 while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))//обработка сообщений Windows
 {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
 }
 if (res==true) ComputerAnswer();
}
//-Новые функции класса------------------------------------------------------
void CDialog_Main::AddMoveToList(char *text,long x1,long y1,long x2,long y2)
{
 char fname[255];
 char string[255];
 sprintf(fname," ");
 CELL type=Board[Coord64to256[x2|(y2<<3)]]&MASK_TYPE;
 if (type==FIGURE_TYPE_PAWN) sprintf(fname,"Пешка ");
 if (type==FIGURE_TYPE_KING) sprintf(fname,"Король ");
 if (type==FIGURE_TYPE_QUEEN) sprintf(fname,"Ферзь ");
 if (type==FIGURE_TYPE_ROOK) sprintf(fname,"Ладья ");
 if (type==FIGURE_TYPE_KNIGHT) sprintf(fname,"Конь ");   
 if (type==FIGURE_TYPE_BISHOP) sprintf(fname,"Слон ");
 sprintf(string,"%s %s%c%c-%c%c",text,fname,'A'+x1,'1'+y1,'A'+x2,'1'+y2);
 AddLog(string);
 AddText(string);
}
void CDialog_Main::AddMoveToPrincipalList(long x1,long y1,long x2,long y2)
{
 char string[255];
 sprintf(string,"%c%c-%c%c",'A'+x1,'1'+y1,'A'+x2,'1'+y2);
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_PRINCIPAL_MOVE))->AddString(string);
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_PRINCIPAL_MOVE))->SetTopIndex(0);
}

void CDialog_Main::ComputerAnswer(void)
{
 char string[255];
 //вызываем рассчёт хода компьютером
 DWORD begin=GetTickCount();
 long positions;
 long levels;
 long score;
 SMove sMove_Computer=Search(ComputerColor,sMove.Coord256_PassedPawn,positions,levels,score,MAX_DEPTH);//поиск лучшего хода
 sMove_Computer.Coord256_PassedPawn=sMove.Coord256_PassedPawn;
 sMove=sMove_Computer;
 COORD coord64_1=Coord256to64[sMove.Coord256_1];
 COORD coord64_2=Coord256to64[sMove.Coord256_2];
 long x1=coord64_1&7;
 long x2=coord64_2&7;
 long y1=coord64_1>>3;
 long y2=coord64_2>>3;
 DWORD end=GetTickCount();
 float delta=(float)(end-begin);
 float dtime=delta/1000.0f;
 sprintf(string,"Анализировалось: %i позиций",positions);
 AddText(string);
 sprintf(string,"Глубина продлений,полуходов: %i",levels);
 AddText(string);
 sprintf(string,"Потрачено: %.2f секунд",dtime);
 AddText(string);
 float speed=((float)positions)/dtime;
 sprintf(string,"Скорость: %.0f поз./сек.",speed);
 AddText(string);  
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_PRINCIPAL_MOVE))->ResetContent();
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_PRINCIPAL_MOVE))->AddString("PVS");
 for(long n=0;n<MAX_DEPTH;n++)
 {
  COORD coord64_1p=Coord256to64[sMove_PrincipalVariation[n].Coord256_1];
  COORD coord64_2p=Coord256to64[sMove_PrincipalVariation[n].Coord256_2];
  long xp1=coord64_1p&7;
  long xp2=coord64_2p&7;
  long yp1=coord64_1p>>3;
  long yp2=coord64_2p>>3;
  if (xp1==xp2 && yp1==yp2) break;
  AddMoveToPrincipalList(xp1,yp1,xp2,yp2);
 }

 X1=x1;
 Y1=y1;
 X2=x2;
 Y2=y2;
 SelectX1=-1;
 SelectY1=-1;
 SelectX2=-1;
 SelectY2=-1;
 if (Move_MoveOn(ComputerColor,(COORD)x1,(COORD)y1,(COORD)x2,(COORD)y2,sMove.Coord256_PassedPawn,sMove.NewFigureType)==ENGINE_FALSE)
 {
  if (Move_CheckKing(ComputerColor)==ENGINE_FALSE)
  {
   AddText("ПАТ.");
   AddLog("ПАТ.");
   MessageBox("Достигнут ПАТ!","Сообщение",MB_OK);
  }
 } 
 else AddMoveToList("C:",x1,y1,x2,y2);
 float scoref=-static_cast<float>(score)/static_cast<float>(WEIGHT_PAWN);
 sprintf(string,"Оценка ситуации:%.2f",scoref);
 AddText(string);
 if (ComputerColor==WHITE) score=-score;
 if (score>WEIGHT_INFINITY-MAX_LEVEL) 
 {
  long depth=WEIGHT_INFINITY-score-1;
  if (depth>0) 
  {
   sprintf(string,"МАТ белым через %i полухода!",depth);
   AddText(string);
   AddLog(string);
  }
  else
  {
   AddText("МАТ белым!");
   AddLog("МАТ белым!");
   MessageBox("Поставлен МАТ белым!","Сообщение",MB_OK);
  }
 }
 if (score<-WEIGHT_INFINITY+MAX_LEVEL) 
 {
  long depth=WEIGHT_INFINITY+score-1;
  if (depth>0)
  {
   sprintf(string,"МАТ чёрным через %i полухода!",depth);
   AddText(string);
   AddLog(string);
  }
  else
  {
   AddText("МАТ чёрным!");
   AddLog("МАТ чёрным!");
   MessageBox("Поставлен МАТ чёрным!","Сообщение",MB_OK);
  }
 }
 AddText("--------------------------------------------------");
 InvalidateRect(NULL,FALSE); 
}

//-Новые функции класса------------------------------------------------------
bool CDialog_Main::Move(long x1,long y1,long x2,long y2)
{
 //сделать ход
 COORD coord64=x1|(y1<<3);
 CELL cell=Board[Coord64to256[coord64]];
 FIGURE_TYPE type=cell&MASK_TYPE;
 if (Move_MoveOn(HumanColor,(COORD)x1,(COORD)y1,(COORD)x2,(COORD)y2,sMove.Coord256_PassedPawn,FIGURE_TYPE_QUEEN)==ENGINE_TRUE)
 {
  COORD coord64_2=x2|(y2<<3);
  COORD coord256_2=Coord64to256[coord64_2];
  CELL b=Board[coord256_2];
  if ((HumanColor==WHITE && type==FIGURE_TYPE_PAWN && y2==7) || (HumanColor==BLACK && type==FIGURE_TYPE_PAWN && y2==0))
  {
   CELL new_type=(FIGURE_TYPE)cDialog_SelectFigurePtr->DoModal();
   b=(b&(CELL_FULL_VALUE^MASK_TYPE))|new_type;
   Board[coord256_2]=b;
  }
  long EvaluateValue=Score_Evaluate(WHITE);  
  AddMoveToList("H:",x1,y1,x2,y2);
  ((CEdit*)GetDlgItem(IDC_EDIT_MAIN_MOVE))->SetWindowText("");  
  X1=-1;
  Y1=-1;
  X2=-1;
  Y2=-1;
  InvalidateRect(NULL,FALSE);
  return(true);
 }
 else AddMoveToList("Ошибочный ход:",x1,y1,x2,y2);  
 InvalidateRect(NULL,FALSE); 
 return(false);
}

//-Прочее--------------------------------------------------------------------
void CDialog_Main::AddText(char *string)
{
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_MOVE))->AddString(string);
 long up=((CListBox*)GetDlgItem(IDC_LIST_MAIN_MOVE))->GetCount();
 ((CListBox*)GetDlgItem(IDC_LIST_MAIN_MOVE))->SetTopIndex(up-1);	 
}

void CDialog_Main::DrawSprite(long x,long y,long s_width,long s_height,unsigned char *s_ptr,long width,long height,unsigned char *i_ptr,long select)
{
 for(long xp=0;xp<s_width;xp++)
 {
  for(long yp=0;yp<s_height;yp++)
  {
   long offsetb=(x+xp+(y+yp)*width)*3;
   long offsetp=(xp+(s_height-yp-1)*s_width)*3;   
   unsigned char b=s_ptr[offsetp];
   unsigned char g=s_ptr[offsetp+1];
   unsigned char r=s_ptr[offsetp+2];
   if (select==1)
   {
    if (xp<3 || xp>s_width-3 || yp<3 || yp>=s_height-3)
	{
     r=64;
	 g=0;
	 b=255;
	}
   }
   if (select==2)
   {
    if (xp<3 || xp>s_width-3 || yp<3 || yp>=s_height-3)
	{
     r=127;
	 g=127;
	 b=255;
	}
   }
   i_ptr[offsetb]=b;
   i_ptr[offsetb+1]=g;
   i_ptr[offsetb+2]=r;
  }
 }
}
void CDialog_Main::AddLog(char *string)
{
 FILE *file=fopen("game_log.txt","ab");
 if (file==NULL) return;
 fprintf(file,"%s\r\n",string);
 fclose(file);
}
