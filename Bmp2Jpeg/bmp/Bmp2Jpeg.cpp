#include "stdafx.h"
#include "Bmp2Jpeg.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include "jconfig.h"
#include "jmorecfg.h"
#include "jpeglib.h"

#pragma comment(lib,"libjpegd.lib")
using namespace std;

#pragma  pack(2)

struct bmp_fileheader
{
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
};

struct bmp_infoheader 
{
	unsigned long biSize;
	unsigned long biWidth;
	unsigned long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	unsigned long biXpelsPerMeter;
	unsigned long biYpelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
};

struct RGBPallete
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char alpha;

};

CBmp2Jpeg::CBmp2Jpeg()
{
}


CBmp2Jpeg::~CBmp2Jpeg()
{
}


int CBmp2Jpeg::Bmp2Jpeg(const char* bmp, const char* jpg)
{
	unsigned char *brga = NULL;
	int width = 0, height = 0, depth = 0;
	if (ReadBmp(bmp, &brga, width, height, depth) < 0)
	{
		return -1;
	}
	unsigned char *rgb = new unsigned char[width*abs(height)*depth/8];

	Bgra2Rgb(brga, width, height, depth/8, rgb);

	SaveJpeg(jpg, rgb, width, height, 3);

	return 0;
}

int CBmp2Jpeg::ReadBmp(const char* filename, unsigned char **data, int &w, int &h, int &d)
{
	fstream filein;
	filein.open(filename, std::ios::binary | std::ios::in);
	if (!filein.is_open())
	{
		char clog[256] = { 0 };
		sprintf_s(clog, sizeof(clog), "???",filename);
		OutputDebugStringA(clog);
		return -1;
	}

	int width = 0;
	int height = 0;
	int bitCount = 0;

	bmp_fileheader fileHeader;
	bmp_infoheader infoHeader;

	InitFileHeader(&filein,&fileHeader);

	if (fileHeader.bfType != 0x4d42)
	{
		filein.close();
		return -1;
	}

	InitInfoHeader(&filein, &infoHeader);
 
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	bitCount = infoHeader.biBitCount;

	int bitPerLine = ((width*bitCount + 31) >> 5 )<< 2;
	int imgSize = abs(height*bitPerLine);
	int imgReal = fileHeader.bfSize - fileHeader.bfOffBits;

	if (imgReal != imgSize)
	{
		char clog[256] = { 0 };
		sprintf_s(clog, sizeof(clog), "123");
		OutputDebugStringA(clog);
		filein.close();
		return -1;
	}
	if (bitCount == 8)
	{
	}
	else if (bitCount == 24 || bitCount == 32)
	{
		*data = new unsigned char[imgSize];
		filein.read((char*)(*data), imgSize);
		filein.close();
	}
	w = width;
	h = height;
	d = bitCount;
	return 0;


}

void CBmp2Jpeg::InitFileHeader(void *pFile, void *fileHeader)
{
	bmp_fileheader *pFileHeader = (bmp_fileheader*)fileHeader;
	fstream *pfilein = (fstream*)pFile;
	pFileHeader->bfType = 0;
	pFileHeader->bfSize = 0;
	pFileHeader->bfReserved1 = 0;
	pFileHeader->bfReserved2 = 0;
	pFileHeader->bfOffBits = 0;
	pfilein->read((char*)fileHeader, sizeof(bmp_fileheader));

}

void CBmp2Jpeg::InitInfoHeader(void *pFile, void *infoHeader)
{
	bmp_infoheader *pInfoHeader = (bmp_infoheader *)infoHeader;
	fstream *pFilein = (fstream*)pFile;
	char px[1024] = { 0 };
	memcpy_s(pInfoHeader, sizeof(bmp_infoheader), px, sizeof(bmp_infoheader));
	pFilein->read((char*)infoHeader, sizeof(bmp_infoheader));
}

void CBmp2Jpeg::Bgra2Rgb(const unsigned char *src, int w, int h, int d, unsigned char *dst)
{
	unsigned char* pTempDst = dst;
	for (int i = 0; i < abs(h);i++)
	{
		const unsigned char* pTempSrc = nullptr;
		if (h>0)
		{
			pTempSrc = src+w*i*d;
		}
		else
		{
			pTempSrc = src + abs(i + h + 1)*d;
		}
		for (int j = 0; j < w;j++)
		{
			*(pTempDst) = *(pTempSrc + 2);
			*(pTempDst + 1) = *(pTempSrc + 1);
			*(pTempDst + 2) = *(pTempSrc);
			pTempDst += 3;
			pTempSrc += d;
		}
	}


}


int CBmp2Jpeg::SaveJpeg(const char* filename, unsigned char* bit, int width, int height, int depth)
{
	FILE *outfile;
	fopen_s(&outfile,filename,"wb");
	if (outfile == NULL)
	{
		return -1;
	}
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;      /* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 3;         /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;         /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE /* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer[1];        /* pointer to JSAMPLE row[s] */
	int     row_stride;             /* physical row width in image buffer */
	row_stride = width * depth; /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		//这里我做过修改，由于jpg文件的图像是倒的，所以改了一下读的顺序
		//row_pointer[0] = & bit[cinfo.next_scanline * row_stride];
		row_pointer[0] = &bit[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	return 0;
}


