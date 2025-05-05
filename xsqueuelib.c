
#include "xsqueuelib.h"
#include "xsqueue.h"
#include "xsdef.h"


void xs_q_deleteQueue(xsuint o)
{
	Queue q=(Queue)o;
	queueFree(q);
}

void xs_q_createQueue(_xsvm *vm)
{
	Queue q=queueCreate(1024);
	
	xs_pushObject(vm, (xsuint)q, xs_q_deleteQueue);
}

void xs_q_createQueueByQueue(_xsvm *vm)
{
	Queue q2=(Queue)(xsuint)xs_popNumber(vm);
	Queue q=queueCreate(1024);
	char *buf=queueFront(q2, q2->Size);
	queuePush(q, buf, q2->Size);
	xs_pushObject(vm, (xsuint)q, xs_q_deleteQueue);
	free(buf);
}

void xs_q_readByte(_xsvm *vm)
{
	//read 1 byte from stream
	Queue q=(Queue)xs_popObject(vm);
	char *b=queuePop(q,1);
	xs_pushNumber(vm, *b );
	if(b)
	{
		free(b);
	}		
}	

void xs_q_readShort(_xsvm *vm)
{
	//read 2 byte from stream
	Queue q=(Queue)xs_popObject(vm);
	char *b=queuePop(q,2);
	short *v=(short*)b;
	xs_pushNumber(vm, *v );
	if(b)
	{
		free(b);
	}		
}
void xs_q_readInt(_xsvm *vm)
{
	//read 4 byte from stream
	Queue q=(Queue)xs_popObject(vm);
	char *b=queuePop(q,4);
	int *v=(int*)b;
	xs_pushNumber(vm, *v );
	if(b)
	{
		free(b);
	}		
}

void xs_q_readFloat(_xsvm *vm)
{
	//read 4 byte from stream
	Queue q=(Queue)xs_popObject(vm);
	char *b=queuePop(q,4);
	float *v=(float*)b;
	xs_pushNumber(vm, *v );
	if(b)
	{
		free(b);
	}		
}

void xs_q_readDouble(_xsvm *vm)
{
	//read 8 byte from stream
	Queue q=(Queue)xs_popObject(vm);
	char *b=queuePop(q,8);
	double *v=(double*)b;
	xs_pushNumber(vm, *v );
	if(b)
	{
		free(b);
	}		
}

void xs_q_readBytes(_xsvm *vm)
{
	xsuint count=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	Queue q2=queueCreate(count+32);
	
	char *b=queuePop(q,count);	
	if(b)
	{
		queuePush(q2, b, count );
		free(b);
	}
	xs_pushObject(vm, (xsuint)q2, xs_q_deleteQueue);
}

void xs_q_writeByte(_xsvm *vm)
{
	char c=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	queuePush(q, &c, 1);
}

void xs_q_writeShort(_xsvm *vm)
{
	short c=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	queuePush(q, (char*)&c, 2);

}

void xs_q_writeInt(_xsvm *vm)
{
	int c=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	queuePush(q, (char*)&c, 4);

}

void xs_q_writeFloat(_xsvm *vm)
{
	float c=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	queuePush(q, (char*)&c, 4);

}

void xs_q_writeDouble(_xsvm *vm)
{
	double c=xs_popNumber(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	queuePush(q, (char*)&c, 8);

}

//writeBytes(b,b2)
void xs_q_writeBytes(_xsvm *vm)
{
	Queue q2=(Queue)xs_popObject(vm);
	Queue q=(Queue)xs_popObject(vm);
	
	char *b=queueFront(q2, q2->Size);
	if(b)
	{
		queuePush(q, b, q2->Size);
		free(b);
	}
}
/*
//只读取或修改,不从队列中移除
void xs_q_getByteAt(_xsvm *)
{
	//_xsByteArray *b
	//int position
	
}

void xs_q_setByteAt(_xsvm *)
{
	
}
*/
void xs_q_toString(_xsvm *vm)
{
	Queue q=(Queue)xs_popObject(vm);
	char *b=queueFront(q, q->Size);
	if(b)
	{
		xs_pushString(vm, b);
		free(b);
	}
	else
	{
		xs_pushString(vm, " ");
	}
}


void xs_q_getLength(_xsvm *vm)
{
	Queue q=(Queue)xs_popObject(vm);
	xs_pushNumber(vm, q->Size);	
}

void usequeuelib(_xsvm *vm)
{	
	xs_register(vm, "queue_readByte", 1, xs_q_readByte);
	xs_register(vm, "queue_readShort", 1, xs_q_readShort);
	xs_register(vm, "queue_readInt", 1, xs_q_readInt);
	xs_register(vm, "queue_readFloat", 1, xs_q_readFloat);
	xs_register(vm, "queue_readDouble", 1, xs_q_readDouble);
	xs_register(vm, "queue_readBytes", 2, xs_q_readBytes);
	
	xs_register(vm, "queue_writeByte", 2, xs_q_writeByte);
	xs_register(vm, "queue_writeShort", 2, xs_q_writeShort);
	xs_register(vm, "queue_writeInt", 2, xs_q_writeInt);
	xs_register(vm, "queue_writeFloat", 2, xs_q_writeFloat);
	xs_register(vm, "queue_writeDouble", 2, xs_q_writeDouble);
	xs_register(vm, "queue_writeBytes", 2, xs_q_writeBytes);

	xs_register(vm, "queue_getLength", 1, xs_q_getLength);
	xs_register(vm, "queue_toString",1, xs_q_toString);
	
	xs_register(vm, "queue_createQueue", 0, xs_q_createQueue);
	xs_register(vm, "queue_createQueueByQueue", 1, xs_q_createQueueByQueue);
	
}
