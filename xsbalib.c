
#include "xsbalib.h"

/*Binary Array Library
 * 
 *Handle binary data 
 * 
*/

void xs_deleteByteArray( xsuint o)
{
	_xsByteArray *b=(_xsByteArray*)o;
	free(b->stream);
	free(b);
}

void xs_createByteArray(_xsvm *vm)
{
	_xsByteArray *b=(_xsByteArray*)calloc(sizeof(_xsByteArray),1);
	b->length=32;
	b->stream=(unsigned char*)calloc(32,1);
	b->currentPos=b->stream;
	
	xs_pushObject(vm, (xsuint)b, xs_deleteByteArray);
}

void xs_readByte(_xsvm *vm)
{
	//read 1 byte from stream
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	if( (b->currentPos-b->stream)<1 )
	{
		xs_pushNumber(vm, 0 );
		return;
		//exception
	}
	b->currentPos-=1;
	xs_pushNumber(vm, *(unsigned char*)b->currentPos );
}
void xs_readShort(_xsvm *vm)
{
	//read 2 byte from stream
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	if( (b->currentPos-b->stream)<2 )
	{
		xs_pushNumber(vm, 0 );
		return;
		//exception
	}
	b->currentPos-=2;
	xs_pushNumber(vm, *(xsushort*)b->currentPos );
}
void xs_readInt(_xsvm *vm)
{
	//read 4 byte from stream
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	if( (b->currentPos-b->stream)<4 )
	{
		xs_pushNumber(vm, 0 );
		return;
		//exception
	}
	b->currentPos-=4;
	xs_pushNumber(vm, *(int*)b->currentPos );
}

void xs_readFloat(_xsvm *vm)
{
	//read 4 byte from stream
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	if( (b->currentPos-b->stream)<4 )
	{
		xs_pushNumber(vm, 0 );
		return;
		//exception
	}
	b->currentPos-=4;
	xs_pushNumber(vm, *(float*)b->currentPos );
}

void xs_readDouble(_xsvm *vm)
{
	//read 8 byte from stream
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	if( (b->currentPos-b->stream)<8 )
	{
		xs_pushNumber(vm, 0 );
		return;
		//exception
	}
	b->currentPos-=8;
	xs_pushNumber(vm, *(double*)b->currentPos );
}

void xs_readBytes(_xsvm *vm)
{
	xsuint count=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	_xsByteArray *ob=(_xsByteArray*)calloc(sizeof(_xsByteArray),1);
	ob->length=count+1;
	ob->stream=(unsigned char*)calloc(ob->length, 1);
	b->currentPos-=count;
	memcpy(ob->stream, b->currentPos, count);
	ob->currentPos=ob->stream+count;
	
	xs_pushObject(vm, (xsuint)ob, xs_deleteByteArray);
}

//xs_resizeByteArray
void xs_resizeByteArray(_xsByteArray *b, int newCount)
{
	int realCount=b->currentPos-b->stream;
	unsigned char *buf;
	newCount=(newCount*110)/100+8;
	buf=(unsigned char*)malloc(newCount);
	memcpy(buf,b->stream, realCount );
	free(b->stream);
	b->stream=buf;
	b->currentPos=buf+realCount;	
}

void xs_writeByte(_xsvm *vm)
{
	unsigned char c=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=b->currentPos-b->stream+sizeof(char);
	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}
	
	*b->currentPos=c;
	b->currentPos+=sizeof(char);
}

void xs_writeShort(_xsvm *vm)
{
	unsigned short s=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=b->currentPos-b->stream+sizeof(xsushort);

	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}

	*((xsushort*)b->currentPos)=s;	
	b->currentPos+=sizeof(xsushort);
}

void xs_writeInt(_xsvm *vm)
{
	unsigned int i=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=b->currentPos-b->stream+sizeof(int);

	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}

	*((int*)b->currentPos)=i;
	b->currentPos+=sizeof(int);
}

void xs_writeFloat(_xsvm *vm)
{
	float f=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=b->currentPos-b->stream+sizeof(float);
	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}
	
	*((float*)b->currentPos)=f;
	b->currentPos+=sizeof(float);
}

void xs_writeDouble(_xsvm *vm)
{
	double d=xs_popNumber(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=b->currentPos-b->stream+sizeof(double);
	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}
	
	*((double*)b->currentPos)=d;
	b->currentPos+=sizeof(double);
}

//writeBytes(b,b2)
void xs_writeBytes(_xsvm *vm)
{
	_xsByteArray *b2=(_xsByteArray*)xs_popObject(vm);
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	int newCount=(b->currentPos-b->stream)+(b2->currentPos-b2->stream);
	if( b->length<=newCount)
	{
		xs_resizeByteArray(b,newCount);
	}
	
	memcpy(b->currentPos,b2->stream,(b2->currentPos-b2->stream));
	b->currentPos+=(b2->currentPos-b2->stream);
}

/*
//只读取或修改,不从队列中移除
void xs_getByteAt(_xsvm *)
{
	//_xsByteArray *b
	//int position
	
}

void xs_setByteAt(_xsvm *)
{
	
}
*/

void xs_byteArrayToString(_xsvm *vm)
{
	_xsByteArray *b=(_xsByteArray*)xs_popObject(vm);
	//b->stream[b->currentPos-b->stream]=0;
	//b->currentPos[0]=0;
	xs_pushString(vm, (char*)b->stream);
}


void xs_getLength(_xsvm *vm)
{
	_xsByteArray *b=(_xsByteArray*)calloc(sizeof(_xsByteArray),1);
	xs_pushNumber(vm, b->currentPos-b->stream);	
}

void usebalib(_xsvm *vm)
{	
	xs_register(vm, "ba_readByte", 1, xs_readByte);
	xs_register(vm, "ba_readShort", 1, xs_readShort);
	xs_register(vm, "ba_readInt", 1, xs_readInt);
	xs_register(vm, "ba_readFloat", 1, xs_readFloat);
	xs_register(vm, "ba_readDouble", 1, xs_readDouble);
	xs_register(vm, "ba_readBytes", 2, xs_readBytes);
	
	xs_register(vm, "ba_writeByte", 2, xs_writeByte);
	xs_register(vm, "ba_writeShort", 2, xs_writeShort);
	xs_register(vm, "ba_writeInt", 2, xs_writeInt);
	xs_register(vm, "ba_writeFloat", 2, xs_writeFloat);
	xs_register(vm, "ba_writeDouble", 2, xs_writeDouble);
	xs_register(vm, "ba_writeBytes", 2, xs_writeBytes);

	xs_register(vm, "ba_getLength", 1, xs_getLength);
	xs_register(vm, "ba_byteArrayToString",1, xs_byteArrayToString);
	
	xs_register(vm, "ba_createByteArray", 0, xs_createByteArray);
}


