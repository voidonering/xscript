
#ifndef _XSQUEUELIB_H
#define _XSQUEUELIB_H
#include "xsapi.h"


#ifdef __cplusplus
extern "C" {
#endif

	void xs_q_deleteQueue(xsuint o);
	void xs_q_createQueue(_xsvm *vm);
	
	void xs_q_readByte(_xsvm *vm);
	void xs_q_readShort(_xsvm *vm);
	void xs_q_readInt(_xsvm *vm);
	void xs_q_readFloat(_xsvm *vm);
	void xs_q_readDouble(_xsvm *vm);
	void xs_q_readBytes(_xsvm *vm);
	
	void xs_q_writeByte(_xsvm *vm);
	void xs_q_writeShort(_xsvm *vm);
	void xs_q_writeInt(_xsvm *vm);
	void xs_q_writeFloat(_xsvm *vm);
	void xs_q_writeDouble(_xsvm *vm);
	void xs_q_writeBytes(_xsvm *vm);

	//只读取或修改,不从队列中移除
	void xs_q_getByteAt(_xsvm *);
	void xs_q_setByteAt(_xsvm *);
	void xs_q_toString(_xsvm *vm);
	void xs_q_getLength(_xsvm *vm);
	void usequeuelib(_xsvm *vm);


#ifdef __cplusplus
}
#endif

#endif
