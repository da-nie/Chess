#include "cdialog_selectfigure.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//������� ��������� ��������� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CDialog_SelectFigure,CDialog)
 ON_WM_DESTROY()
 ON_WM_CLOSE()
 ON_WM_PAINT()
 ON_COMMAND(IDC_BUTTON_SELECT_FIGURE_QUEEN,OnCommand_Button_SelectQueen)
 ON_COMMAND(IDC_BUTTON_SELECT_FIGURE_ROOK,OnCommand_Button_SelectRook)
 ON_COMMAND(IDC_BUTTON_SELECT_FIGURE_BISHOP,OnCommand_Button_SelectBishop)
 ON_COMMAND(IDC_BUTTON_SELECT_FIGURE_KNIGHT,OnCommand_Button_SelectKnight)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
//����������� ������
//---------------------------------------------------------------------------
CDialog_SelectFigure::CDialog_SelectFigure(LPCTSTR lpszTemplateName, CWnd* pParentWnd):CDialog(lpszTemplateName,pParentWnd)
{ 
 Image=LoadTGAFromFile("picture\\change.tga",ImageWidth,ImageHeight);
}
//---------------------------------------------------------------------------
//���������� ������
//---------------------------------------------------------------------------
CDialog_SelectFigure::~CDialog_SelectFigure()
{
 if (Image!=NULL) delete[](Image);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//���������� ������� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//������ ������� Enter
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnOK(void)
{
}
//---------------------------------------------------------------------------
//������ ������� escape
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnCancel(void)
{
}
//---------------------------------------------------------------------------
//��������� ������� ������
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnClose(void)
{
}
//---------------------------------------------------------------------------
//������������� �������
//---------------------------------------------------------------------------
afx_msg BOOL CDialog_SelectFigure::OnInitDialog(void)
{
 return(CDialog::OnInitDialog());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//����� ������� ������ (�� ����������)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//������� ��������� ��������� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//��������� ����������� ����
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnDestroy(void)
{
 CDialog::OnDestroy();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//������� �����
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnCommand_Button_SelectQueen(void)
{
 EndDialog(FIGURE_TYPE_QUEEN);
}
//---------------------------------------------------------------------------
//������� �����
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnCommand_Button_SelectRook(void)
{
 EndDialog(FIGURE_TYPE_ROOK);
}
//---------------------------------------------------------------------------
//������� �����
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnCommand_Button_SelectBishop(void)
{
 EndDialog(FIGURE_TYPE_BISHOP);
}
//---------------------------------------------------------------------------
//������� ����
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnCommand_Button_SelectKnight(void)
{
 EndDialog(FIGURE_TYPE_KNIGHT);
}
//---------------------------------------------------------------------------
//���������
//---------------------------------------------------------------------------
afx_msg void CDialog_SelectFigure::OnPaint(void)
{
 CRect cRect;
 ((CStatic*)GetDlgItem(IDC_STATIC_SELECT_FIGURE_MAP))->GetClientRect(&cRect);
 CRect cRect_W;
 ((CStatic*)GetDlgItem(IDC_STATIC_SELECT_FIGURE_MAP))->GetWindowRect(&cRect_W);
 ScreenToClient(cRect_W);

 long width=cRect.right-cRect.left;
 long height=cRect.bottom-cRect.top;

 unsigned char *ImageBuffer=new unsigned char[ImageWidth*ImageHeight*3];
 if (Image!=NULL) DrawSprite(0,0,ImageWidth,ImageHeight,Image,ImageWidth,ImageHeight,ImageBuffer);
 
 CPaintDC dc(this);
 BITMAPINFOHEADER bmih;
 bmih.biSize=sizeof(BITMAPINFOHEADER);
 bmih.biWidth=ImageWidth;
 bmih.biHeight=ImageHeight;
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
 StretchDIBits(dc.m_hDC,cRect_W.left,cRect_W.top,cRect.right,cRect.bottom,0,0,ImageWidth,ImageHeight,ImageBuffer,&info,DIB_RGB_COLORS,SRCCOPY);
 
 delete(ImageBuffer);
 CDialog::OnPaint();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//����� ������� ������ (����������)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CDialog_SelectFigure::DrawSprite(long x,long y,long s_width,long s_height,unsigned char *s_ptr,long width,long height,unsigned char *i_ptr)
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
   i_ptr[offsetb]=b;
   i_ptr[offsetb+1]=g;
   i_ptr[offsetb+2]=r;
  }
 }
}
