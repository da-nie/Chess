#ifndef LOADTGA_H
#define LOADTGA_H

#include <windows.h>
#include <stdio.h>

//��������� TGA-�����
#pragma pack(1)
struct STGAHeader
{
 char identsize;//������ ���� ID ��������� (0)
 char colorMapType;//���� �� �������:0-���,1-����
 char imageType;//��� ��������:0-���,1-��������� �����,2-RGB,3-������� ������, (3-� ��� - RLE- �����������)
 unsigned short	colorMapStart;//������ ����� ������
 unsigned short	colorMapLength;//���������� ������ � �����
 unsigned char 	colorMapBits;//����������� �������
 unsigned short	xstart;//��������� ���������� �����������
 unsigned short	ystart;                 
 unsigned short	width;//������ ����������� �� X
 unsigned short	height;//������ ����������� �� Y
 char bits;//��������� ��� �� ������� (8,16,24,32)
 char descriptor;//���������� ������������
};
#pragma pack(8)

unsigned char *LoadTGAFromFile(char *FileName,int &Width,int &Height);
unsigned char *LoadTGAFromResource(HMODULE hModule,int ID,int &Width,int &Height);
bool SaveTGA(char *FileName,int Width,int Height,unsigned char *Image);

#endif