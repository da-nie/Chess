#ifndef CDIALOG_SELECT_FIGURE_H
#define CDIALOG_SELECT_FIGURE_H

#include "stdafx.h"
#include "engine.h"
#include "tga.h"

class CDialog_SelectFigure:public CDialog
{
 protected:
  //-Переменные класса-------------------------------------------------------
  unsigned char *Image;
  int ImageHeight;
  int ImageWidth;
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CDialog_SelectFigure(LPCTSTR lpszTemplateName, CWnd* pParentWnd);
  //-Деструктор класса-------------------------------------------------------
  ~CDialog_SelectFigure();
  //-Замещённые функции предка-----------------------------------------------
  afx_msg void OnOK(void);
  afx_msg void OnCancel(void);
  afx_msg void OnClose(void);
  afx_msg BOOL OnInitDialog(void);
  afx_msg void OnPaint(void);
  //-Новые функции класса----------------------------------------------------  
 public:
  //-Функции обработки сообщений класса--------------------------------------
  DECLARE_MESSAGE_MAP()
  afx_msg void OnDestroy(void);
  //-Новые функции класса----------------------------------------------------
  afx_msg void OnCommand_Button_SelectQueen(void);//выбрать ферзя
  afx_msg void OnCommand_Button_SelectRook(void);//выбрать ладью
  afx_msg void OnCommand_Button_SelectBishop(void);//выбрать слона
  afx_msg void OnCommand_Button_SelectKnight(void);//выбрать коня
  //-Прочее------------------------------------------------------------------
 protected:
  //-Новые функции класса----------------------------------------------------  
  void DrawSprite(long x,long y,long s_width,long s_height,unsigned char *s_ptr,long width,long height,unsigned char *i_ptr);
};

#endif