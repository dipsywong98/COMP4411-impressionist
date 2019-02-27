//
// bitmap.h
//
// header file for MS bitmap format
//
//

#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <string.h>

#define BMP_BI_RGB        0L

typedef unsigned short	BMP_WORD; 
typedef unsigned int	BMP_DWORD; 
typedef int				BMP_LONG; 
 
typedef struct { 
	BMP_WORD	bfType; 
	BMP_DWORD	bfSize; 
	BMP_WORD	bfReserved1; 
	BMP_WORD	bfReserved2; 
	BMP_DWORD	bfOffBits; 
} BMP_BITMAPFILEHEADER; 
 
typedef struct { 
	BMP_DWORD	biSize; 
	BMP_LONG	biWidth; 
	BMP_LONG	biHeight; 
	BMP_WORD	biPlanes; 
	BMP_WORD	biBitCount; 
	BMP_DWORD	biCompression; 
	BMP_DWORD	biSizeImage; 
	BMP_LONG	biXPelsPerMeter; 
	BMP_LONG	biYPelsPerMeter; 
	BMP_DWORD	biClrUsed; 
	BMP_DWORD	biClrImportant; 
} BMP_BITMAPINFOHEADER;

typedef struct
{
	int pad;
	int padWidth;
} PADDING;

typedef struct
{
	unsigned int c0;
	unsigned int c1;
	unsigned int c2;
} DIB_COLOR_MAPPING;

// global I/O routines
extern unsigned char* readBMP(const char* fname, int& width, int& height );
extern unsigned char* mapColor(unsigned char* data, long int width, long int height, long int pad, DIB_COLOR_MAPPING map);
extern unsigned char* reverseMapColor(unsigned char* data, long int width, long int height, DIB_COLOR_MAPPING map);
extern unsigned char* repackBmp(unsigned char* data, long int width, long int height, long int pad);
extern PADDING calculatePadding(long width);
extern void writeBMP(const char* iname, int width, int height, unsigned char* data );

extern DIB_COLOR_MAPPING bmp_color_mapping;
extern DIB_COLOR_MAPPING cinepak_color_mapping;

#endif