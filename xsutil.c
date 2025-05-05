/*
2009/04/15 TangJun
utility
*/

#include "xsutil.h"
#include<stdarg.h>
#include<stdio.h>
#include<string.h>
#include "stdlib.h"
#include "xslzw.h"
#include "xslog.h"

void xs_extractFilePath(char *filename, char *dst)
{
	strcpy(dst,filename);
	dst+=strlen(filename);
	
	while(dst--)
	{
		if(*dst=='\\'||*dst=='/')
		{
			*dst=0;
			break;
		}
	}
}

void xs_extractFileName(char *filename, char *dst)
{
	int len=strlen(filename);
	filename+=len;
	while(filename--)
	{
		if(*filename=='\\'||*filename=='/')
		{
			strcpy(dst,filename+1);
			return;
		}
	}
	
	strcpy(dst,filename);
}

void xs_initPackage(char *packageFile)
{
	FILE *fh=fopen(packageFile,"w");
	fclose(fh);
}

int xs_packFile(char *srcfile, char *dstfile, xsbyte isCompress)
{
	FILE *src=fopen(srcfile,"rb");
	FILE *xsfh=fopen(dstfile,"ab");
	char *sbuf;
	char filename[256];
	_xsfile xsf;
	int fsize;
	xsuint fcompresssize;
	memset(filename,0,256);
	if(xsfh==NULL )
	{
		return -1;
	}
	if(src==NULL)
	{
		fclose(xsfh);
		return -2;
	}
	fseek(src,0,2);
	fsize=ftell(src);
	sbuf=(char*)malloc(fsize);
	fseek(src,0,0);
	fread(sbuf,1,fsize,src);
	fclose(src);	
	
	fcompresssize=fsize;
	if( isCompress )
	{
		if( fsize<(64*1024) )
		{
			char *sout=(char*)malloc(fsize);		
			xsushort olen;
			xs_compress( (xsbyte*)sbuf, fsize, (xsbyte*)sout, fsize, &olen);
			fcompresssize=olen;
			free(sbuf);
			sbuf=sout;
		}
		else
		{
			isCompress=0;
		}
	}
	fseek(xsfh, 0, 2);
	
	//write file header
	xs_extractFileName(srcfile,filename);
	xsf.length=fcompresssize;
	xsf.scrc='T';
	xsf.fnlen=strlen(filename);
	xsf.srcLength=fsize;
	xsf.isCompressed=isCompress;
	
	fwrite(&xsf,1,sizeof(_xsfile),xsfh);
	fwrite(filename,1,xsf.fnlen,xsfh);
	//write file content
	fwrite(sbuf,1,xsf.length, xsfh);
	free(sbuf);
	
	fclose(xsfh);
	
	return 0;
}

char* xs_extractFileData(char *package, char *filename, xsuint *dstsize)
{
	FILE *src=fopen(package,"rb");
	char *sbuf=NULL;
	if(src==NULL)
	{
		return NULL;
	}
	fseek(src,0,0);

	for(;;)
	{
		//get file header
		_xsfile xsf;
		char fn[256];
		if(fread(&xsf,1,sizeof(_xsfile),src)!=sizeof(_xsfile))
		{
			break;
		}
		//get dst file name
		memset(fn,0,256);
		if(fread(fn,1,xsf.fnlen,src)!=xsf.fnlen)
		{
			break;
		}
		if( xsf.scrc!='T' )
		{
			break;
		}
		if( strcmp( fn,filename)==0 )
		{
			sbuf=(char*)malloc(xsf.length+1);
			if( fread(sbuf,1,xsf.length,src)!=xsf.length)
			{
				xsfree(sbuf);
				sbuf=NULL;
			}
			else if( xsf.isCompressed )
			{
				char *sout=(char*)malloc( xsf.srcLength+1024);
				xsushort outlen;
				xs_decompress( (xsbyte*)sbuf, xsf.length, (xsbyte*)sout,xsf.srcLength+1024, &outlen );
				free(sbuf);
				sbuf=sout;
			}
			/*
			char info[256];
			sprintf(info,"filename=%s, data size=%d, img=%d,%p", filename, size, (xsuint)img,img);
			xs_log(info);
			 */
			*dstsize=xsf.srcLength;
			sbuf[xsf.srcLength]=0;
			fclose(src);
			return sbuf;
		}
		
		fseek(src, xsf.length, SEEK_CUR);
	}
	
	fclose(src);
	return NULL;	
}

//#define XS_TEST_COMPRESS
//path must include "\\"
int xs_unPackFile(char *file, char *path)
{
	FILE *src=fopen(file,"rb");
	FILE *dst;
	char dstfile[256];
	char fn[256];
	char *sbuf;
#ifdef XS_TEST_COMPRESS
	int dlen=0;
	char tb[256];
#endif
	if(src==NULL)
	{
#ifdef XS_TEST_COMPRESS
		sprintf(tb, "failed to open file: %s", file);
		xs_log(tb);
#endif
		return -1;
	}
	fseek(src,0,0);

	for(;;)
	{
		//get file header
		_xsfile xsf;
		if(fread(&xsf,1,sizeof(_xsfile),src)!=sizeof(_xsfile))
		{
#ifdef XS_TEST_COMPRESS
			xs_log("failed to read file header ");
#endif
			break;
		}
		//get dst file name
		memset(fn,0,256);
		if(fread(fn,1,xsf.fnlen,src)!=xsf.fnlen)
		{
#ifdef XS_TEST_COMPRESS
			xs_log( "The two file length do not match ");
#endif
			break;
		}
		if( xsf.scrc!='T' )
		{
#ifdef XS_TEST_COMPRESS
			xs_log( "SCRC error ");
#endif
			break;
		}
		strcpy(dstfile,path);
		strcat(dstfile,fn);
		
		//open destination file
		dst=fopen(dstfile,"wb");
		if(dst==NULL)
		{
#ifdef XS_TEST_COMPRESS
			sprintf(tb, "failed to create file: %s", dstfile);
			xs_log(tb);
#endif
			break;
		}
				
#ifdef XS_TEST_COMPRESS
		sprintf(tb, "reading file data: %d :%s", xsf.length, dstfile);
		xs_log(tb);
#endif

		sbuf=(char*)malloc(xsf.length);
		if( fread(sbuf,1,xsf.length,src)!=xsf.length)
		{
			free(sbuf);
			fclose(dst);
			break;
		}

		//write file content
		if( xsf.isCompressed )
		{
			char *sout=(char*)malloc( xsf.srcLength+1024);
			xsushort outlen;
#ifdef XS_TEST_COMPRESS
			sprintf(tb, "decompress file, srcLength: %d", xsf.srcLength);
			xs_log(tb);
#endif

			xs_decompress( (xsbyte*)sbuf, xsf.length, (xsbyte*)sout,xsf.srcLength+1024, &outlen );
#ifdef XS_TEST_COMPRESS
			sprintf(tb, "decompressed file size: %d", outlen);
			xs_log(tb);
#endif
			free(sbuf);
			sbuf=sout;
		}
		
#ifdef XS_TEST_COMPRESS
			sprintf(tb, "writting file data: %d", xsf.srcLength);
			xs_log(tb);
#endif
		fwrite(sbuf,1,xsf.srcLength,dst);
		free(sbuf);
		fclose(dst);			
	}
	
	fclose(src);
	
	return 0;
}

int xs_getFileSize(char *filename)
{
	FILE *src=fopen(filename,"rb");
	int fsize;
	if(src==NULL)
	{
		return 0;
	}
	fseek(src,0,2);
	fsize=ftell(src);
	fclose(src);
	return fsize;
}

char *xs_readFile(char *filename, xsuint *fsize)
{
	FILE *src=fopen(filename,"rb");
	char *sbuf;
	if(src==NULL)
	{
		return NULL;
	}
	fseek(src,0,2);
	*fsize=ftell(src);
	sbuf=(char*)malloc(*fsize+1);
	fseek(src,0,0);
	fread(sbuf,1,*fsize,src);
	fclose(src);
	sbuf[*fsize]=0;
	
	return sbuf;
}

void xs_saveFileTo(char *filename, char *data, int size)
{
	FILE *src=fopen(filename,"rb");
	fwrite(data,1,size,src);
	fclose(src);	
}

int xs_atoi(const char * str)
{ 
	 int ret = 0;
	 int flag = 1;
	 unsigned int ch;
	 if(*str == '-') { str++;flag = -1; }
	 if(*str == '*') { str++;flag = 1;  }
	   
	 for( ; *str; str++)
	 {
		 ch = *str - '0';
		 ret = ret * 10 + ch;
	 }
	 return ret * flag;
} 

char* xs_itoa(int value,char *str,int radix)
{ 
/* 转换整数为字符串，value为要转换的整数，
 *  str转换后的字符串，radix为进制，返回为转换后的字符串
 */ 
	 int i,j,sign;
	 char ps[256];
	 memset(ps,0,256);
	 
	 if((radix>36) || (radix<2)) return 0;
	 
	 sign = 0;
	 if(value<0)
	 {
		  sign=-1;
		  value=-value;
	 }
	 
	 i=0;
	 
	 do
	 {
		  if((value % radix) > 9)
		   ps[i] = value % radix + '0' + 39;
		  else
		   ps[i] = value % radix + '0' ;
		  i++;
	 }while((value /= radix) > 0);
	 
	 if(sign<0)  ps[i]='-';
	 else i--;
	 
	 for(j=i;j>=0;j--)
	  str[i-j]=ps[j];
	 return str;
}

char* xs_gcvt(double number, int ndigit, char *buf) 
{ /* 转换小数为字符串，number为要转换的小数，ndigit为
 *  小数的精度位数，buf为转换后的字符串，返回为转换后的字符串
 */
	 double float_part;
	 long   int_part; 
	 char stra[30], strb[30];
	 int i=0;
	 int_part=(long)number;
	 float_part=number-int_part;
	 if(float_part<0) float_part=-float_part;
	 memset(stra,0,30);
	 memset(strb,0,30);
	 
	 xs_itoa(int_part,stra,10);//处理整数部分
	 
	 if(ndigit>0)    //需要显示小数部分 
	 { 
		  strb[i++]='.'; 
		  while(ndigit-- > 0) 
		  { 
		   float_part=float_part*10; 
		   strb[i++]= ((int)float_part)%10 + '0';
		  }    
	 }
	 strcat(stra,strb);
	 strcpy(buf,stra);  
	 return   buf;
}  

void xs_sprintf(char *dstbuf, char *fmt,...)
{
	 va_list ap;
	 double dval;
	 int ival;
	 char *p, *sval;
	 char *bp;
	
	 bp= dstbuf;
	 *bp= 0;
	 va_start (ap, fmt);
	 for (p= fmt; *p; p++)
	 {
		  if (*p != '%')
		  {
		   *bp++ = *p;
		   continue;
		  }
		  switch (*++p) 
		  {
			  case 'd':
			   ival= va_arg(ap, int);   
			   xs_itoa (ival, bp, 10);
			   break;
			  case 'o':
			   ival= va_arg(ap, int);   
			   *bp++= '0';
			   xs_itoa (ival, bp, 8);
			   break;
			  case 'x':
			   ival= va_arg(ap, int);
			   *bp++= '0';
			   *bp++= 'x';
			   xs_itoa (ival, bp, 16);
			   break;
			  case 'f':
			   dval= va_arg(ap, double);
			   xs_gcvt(dval,6,bp);
			   break;
			  case 's':
			   for (sval = va_arg(ap, char *) ; *sval ; sval++ )
			    *bp++= *sval;
			   break;
		  }
	 }
	 *bp= 0; 
	 va_end (ap);

}
