#pragma once

class CBmp2Jpeg
{
public:
	CBmp2Jpeg();
	~CBmp2Jpeg();

public:
	int Bmp2Jpeg(const char* bmp,const char* jpg);

private:
	int SaveJpeg(const char* filename, unsigned char* bit ,int width,int height, int depth);
	int ReadBmp(const char* filename,unsigned char **data,int &w,int &h,int &d);
	void Bgra2Rgb(const unsigned char *src,int w,int h,int d,unsigned char *dst);
	void InitFileHeader(void *pFile,void *fileHeader);
	void InitInfoHeader(void *pFile,void *infoHeader);
	void SaveBmp(void *fileHeader,void* intfoHeader);
	int m_quality;
};

