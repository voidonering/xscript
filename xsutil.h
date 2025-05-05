/*
2009/03/09 TangJun
utilities for xs
*/

#ifndef _XSUTIL_H
#define _XSUTIL_H
#include "xsdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__WINS__)
#define PACKED
#else
#define PACKED __attribute__((packed, aligned(1)))
#endif
	
#pragma pack(1) 
typedef struct
{
	//文件数据长度
	xsuint length;
	//文件名长度
	xsbyte fnlen;
	//是否被压缩
	xsbyte isCompressed;
	//校验和
	xsushort scrc;
	//源文件长度
	xsuint srcLength;
	
}PACKED _xsfile;
#pragma pack()

int xs_atoi(const char * str);
char * xs_itoa(int value,char *str,int radix);
char * xs_gcvt(double number, int ndigit, char *buf) ;
void xs_sprintf(char *dstbuf, char *fmt,...);

//read the whole string from a text file
char *xs_readFile(char *filename, xsuint *fsize);
int xs_getFileSize(char *filename);

/*Usage
 * initPackage("c:\\test.pkg");
 * packFile("c:\\afolder\\afile1.bmp","c:\\test.pkg");
 * packFile("c:\\afolder\\afile2.jpg","c:\\test.pkg");
 * packFile("c:\\afolder\\afile3.c","c:\\test.pkg");
 * 
 * to unpack the files
 * the "path" parameter mast include "\\" 
 * xs_unPackFile("c:\\test.pkg","c:\\dstfolder\\");
 * */
void xs_initPackage(char *packageFile);
int xs_unPackFile(char *file, char *path);
int xs_packFile(char *srcfile, char *dstfile, xsbyte isCompress);
void xs_extractFileName(char *filename, char *dst);
void xs_extractFilePath(char *filename, char *dst);
char* xs_extractFileData(char *package, char *filename, xsuint *dstsize);
void xs_saveFileTo(char *file, char *data, int len);

#ifdef __cplusplus
}
#endif

#endif


