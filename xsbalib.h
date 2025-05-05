
#ifndef _XSBALIB_H
#define _XSBALIB_H
#include "xsapi.h"


#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct
{
	unsigned int	length;
	unsigned char*	stream;
	unsigned char*	currentPos;
		
}_xsByteArray;
void xs_deleteByteArray( xsuint o);
void xs_createByteArray(_xsvm *vm);
void usebalib();

void xs_readByte(_xsvm *vm);
void xs_readShort(_xsvm *vm);
void xs_readInt(_xsvm *vm);
void xs_readFloat(_xsvm *vm);
void xs_readDouble(_xsvm *vm);
void xs_readBytes(_xsvm *vm);
void xs_resizeByteArray(_xsByteArray *b, int newCount);
void xs_writeByte(_xsvm *vm);
void xs_writeShort(_xsvm *vm);
void xs_writeInt(_xsvm *vm);
void xs_writeFloat(_xsvm *vm);
void xs_writeDouble(_xsvm *vm);
void xs_writeBytes(_xsvm *vm);
void xs_byteArrayToString(_xsvm *vm);
void xs_getLength(_xsvm *vm);



#ifdef __cplusplus
}
#endif

#endif
