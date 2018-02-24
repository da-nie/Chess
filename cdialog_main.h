#ifndef CDIALOG_MAIN_H
#define CDIALOG_MAIN_H

#include "stdafx.h"

#include "engine.h"
#include "cdialog_selectfigure.h"
#include "tga.h"

class CDialog_Main:public CDialog
{
 protected:
  //-Переменные класса-------------------------------------------------------
  unsigned char *Figure[27];
  unsigned char *Horizontal;
  unsigned char *Vertical;
  unsigned char *VerticalInvert;
  unsigned char *HorizontalInvert;

  long X1;
  long Y1;
  long X2;
  long Y2;

  long SelectX1;
  long SelectY1;
  long SelectX2;
  long SelectY2;

  SMove sMove;

  long HumanColor;
  long ComputerColor;

  bool InversionBoard;//инвертирована ли доска

  CDialog_SelectFigure *cDialog_SelectFigurePtr;

  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CDialog_Main(LPCTSTR lpszTemplateName, CWnd* pParentWnd);
  //-Деструктор класса-------------------------------------------------------
  ~CDialog_Main();
  //-Переменные класса-------------------------------------------------------
  //-Замещённые функции предка-----------------------------------------------
  afx_msg void OnOK(void);
  afx_msg BOOL OnInitDialog(void);
  //-Новые функции класса----------------------------------------------------
  //-Функции обработки сообщений класса--------------------------------------
  DECLARE_MESSAGE_MAP()
  afx_msg void OnDestroy(void);
  afx_msg void OnCommand_Button_Move(void);
  afx_msg void OnCommand_Button_SaveState(void);
  afx_msg void OnCommand_Button_LoadState(void);
  afx_msg void OnCommand_Button_InitWhite(void);
  afx_msg void OnCommand_Button_InitBlack(void);
  afx_msg void OnCommand_Button_RotateBoard(void);
  afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags,CPoint point);
  afx_msg void OnPaint(void);
  //-Новые функции класса----------------------------------------------------
  int LoadBitmap(char *FileName,unsigned char *Bitmap);
  void AddMoveToList(char *text,long x1,long y1,long x2,long y2);
  void AddMoveToPrincipalList(long x1,long y1,long x2,long y2);
  void ComputerAnswer(void);
  void AddText(char *string);
  void DrawSprite(long x,long y,long s_width,long s_height,unsigned char *s_ptr,long width,long height,unsigned char *i_ptr,long select);
  bool Move(long x1,long y1,long x2,long y2);
  void AddLog(char *string);
  //-Прочее------------------------------------------------------------------

};

#endif