//
// bitmap.cpp
//
// handle MS bitmap I/O. For portability, we don't use the data structure 
// defined in Windows.h. However, there is some strange thing, the size of our 
// structure is different from what it should be though we define it in the 
// same way as MS did. So, there is a hack, we use the hardcoded constant, 14, 
// instead of the sizeof to calculate the size of the structure.  You are not 
// supposed to worry about this part. However, I will appreciate if you find 
// out the reason and let me know. Thanks.
//

#include "bitmap.h"
 
BMP_BITMAPFILEHEADER bmfh; 
BMP_BITMAPINFOHEADER bmih;

DIB_COLOR_MAPPING bmp_color_mapping{ 2, 1, 0 };
DIB_COLOR_MAPPING cinepak_color_mapping{ 0, 2, 1 };

// Bitmap data returned is (R,G,B) tuples in row-major order.
unsigned char* readBMP(char*	fname, 
					   int&		width,
					   int&		height)
{ 
	FILE* file; 
	BMP_DWORD pos; 
 
	if ( (file=fopen( fname, "rb" )) == NULL )  
		return NULL; 
	 
//	I am doing fread( &bmfh, sizeof(BMP_BITMAPFILEHEADER), 1, file ) in a safe way. :}
	fread( &(bmfh.bfType), 2, 1, file); 
	fread( &(bmfh.bfSize), 4, 1, file); 
	fread( &(bmfh.bfReserved1), 2, 1, file); 
	fread( &(bmfh.bfReserved2), 2, 1, file); 
	fread( &(bmfh.bfOffBits), 4, 1, file); 

	pos = bmfh.bfOffBits; 
 
	fread( &bmih, sizeof(BMP_BITMAPINFOHEADER), 1, file ); 
 
	// error checking
	if ( bmfh.bfType!= 0x4d42 ) {	// "BM" actually
		return NULL;
	}
	if ( bmih.biBitCount != 24 )  
		return NULL; 
/*
 	if ( bmih.biCompression != BMP_BI_RGB ) {
		return NULL;
	}
*/
	fseek( file, pos, SEEK_SET ); 
 
	width = bmih.biWidth; 
	height = bmih.biHeight;

	const auto padding = calculatePadding(width);
	const auto bytes = height * padding.padWidth;
 
	unsigned char *data = new unsigned char [bytes]; 

	int result = fread( data, bytes, 1, file ); 
	
	if (!result) {
		delete [] data;
		return NULL;
	}

	fclose( file );
	
	mapColor(data, width, height, padding.pad, bmp_color_mapping);
			  
	return data; 
}

unsigned char* mapColor(unsigned char* data, const long int width, const long int height, const long int pad, const DIB_COLOR_MAPPING map)
{
	auto in = data;
	auto out = data;

	for (auto j = 0; j < height; ++j)
	{
		for (auto i = 0; i < width; ++i)
		{
			unsigned char colors[3] = { in[0], in[1], in[2] };
			out[0] = colors[map.c0];
			out[1] = colors[map.c1];
			out[2] = colors[map.c2];

			in += 3;
			out += 3;
		}
		in += pad;
	}

	return data;
}

unsigned char* reverseMapColor(unsigned char* data, const long width, const long height, const DIB_COLOR_MAPPING map)
{
	auto in = data;
	auto out = data;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			unsigned char colors[3] = { in[0], in[1], in[2] };
			out[0] = colors[2];
			out[1] = colors[1];
			out[2] = colors[0];

			in += 3;
			out += 3;
		}
	}

	return data;
}

unsigned char* repackBmp(unsigned char* data, const long width, const long height, const long pad)
{
	const auto padding = calculatePadding(width);
	const auto tLength = height * padding.padWidth;

	auto *t = new unsigned char[tLength];
	memcpy(t, data, tLength);

	auto in = t;
	auto out = data;
	for (auto j = 0; j < height; ++j)
	{
		for (auto i = 0; i < width; ++i)
		{
			out[0] = in[0];
			out[1] = in[1];
			out[2] = in[2];

			in += 3;
			out += 3;
		}
		for(auto i = 0; i < pad; i++)
		{
			out[i] = 0;
		}
		out += pad;
	}

	delete[] t;

	return data;
}

PADDING calculatePadding(long width)
{
	int padWidth = width * 3;
	int pad = 0;
	if (padWidth % 4 != 0)
	{
		pad = 4 - (padWidth % 4);
		padWidth += pad;
	}

	return {pad, padWidth};
}

void writeBMP(char*				iname,
			  int				width, 
			  int				height, 
			  unsigned char*	data) 
{ 
	int bytes, pad;
	bytes = width * 3;
	pad = (bytes%4) ? 4-(bytes%4) : 0;
	bytes += pad;
	bytes *= height;

	bmfh.bfType = 0x4d42;    // "BM"
	bmfh.bfSize = sizeof(BMP_BITMAPFILEHEADER) + sizeof(BMP_BITMAPINFOHEADER) + bytes;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = /*hack sizeof(BMP_BITMAPFILEHEADER)=14, sizeof doesn't work?*/ 
					 14 + sizeof(BMP_BITMAPINFOHEADER);

	bmih.biSize = sizeof(BMP_BITMAPINFOHEADER);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BMP_BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = (int)(100 / 2.54 * 72);
	bmih.biYPelsPerMeter = (int)(100 / 2.54 * 72);
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	FILE *outFile=fopen(iname, "wb"); 

	//	fwrite(&bmfh, sizeof(BMP_BITMAPFILEHEADER), 1, outFile);
	fwrite( &(bmfh.bfType), 2, 1, outFile); 
	fwrite( &(bmfh.bfSize), 4, 1, outFile); 
	fwrite( &(bmfh.bfReserved1), 2, 1, outFile); 
	fwrite( &(bmfh.bfReserved2), 2, 1, outFile); 
	fwrite( &(bmfh.bfOffBits), 4, 1, outFile); 

	fwrite(&bmih, sizeof(BMP_BITMAPINFOHEADER), 1, outFile); 

	bytes /= height;

	reverseMapColor(data, width, height, bmp_color_mapping);
	for (int j = 0; j < height; ++j)
	{
		fwrite(data + j * 3 * width, bytes, 1, outFile);
	}


	fclose(outFile);
} 