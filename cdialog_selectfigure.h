#ifndef CDIALOG_SELECT_FIGURE_H
#define CDIALOG_SELECT_FIGURE_H

#include "stdafx.h"
#include "engine.h"
#include "tga.h"

class CDialog_SelectFigure:public CDialog
{
 protected:
  //-���������� ������-------------------------------------------------------
  unsigned char *Image;
  int ImageHeight;
  int ImageWidth;
  //-������------------------------------------------------------------------
 public:
  //-����������� ������------------------------------------------------------
  CDialog_SelectFigure(LPCTSTR lpszTemplateName, CWnd* pParentWnd);
  //-���������� ������-------------------------------------------------------
  ~CDialog_SelectFigure();
  //-���������� ������� ������-----------------------------------------------
  afx_msg void OnOK(void);
  afx_msg void OnCancel(void);
  afx_msg void OnClose(void);
  afx_msg BOOL OnInitDialog(void);
  afx_msg void OnPaint(void);
  //-����� ������� ������----------------------------------------------------  
 public:
  //-������� ��������� ��������� ������--------------------------------------
  DECLARE_MESSAGE_MAP()
  afx_msg void OnDestroy(void);
  //-����� ������� ������----------------------------------------------------
  afx_msg void OnCommand_Button_SelectQueen(void);//������� �����
  afx_msg void OnCommand_Button_SelectRook(void);//������� �����
  afx_msg void OnCommand_Button_SelectBishop(void);//������� �����
  afx_msg void OnCommand_Button_SelectKnight(void);//������� ����
  //-������------------------------------------------------------------------
 protected:
  //-����� ������� ������----------------------------------------------------  
  void DrawSprite(long x,long y,long s_width,long s_height,unsigned char *s_ptr,long width,long height,unsigned char *i_ptr);
};

#endif