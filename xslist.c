
#include "xslist.h"

void listResize(_list* t)
{
	if(t->count>=t->maxCount)
	{
		int newCount=(t->count*110)/100+2;
		char *temp=(char*)calloc(1,newCount*4);
		if(t->buff)
		{
			memcpy(temp,t->buff,t->count*4);
			free( t->buff);
		}
		
		t->maxCount=newCount;
		t->buff=(void **)temp;

	}
	
}

void listNew(_list* p,int typesize, int count)
{
	p->count=0;
	p->maxCount=count;
	p->typeSize=typesize;
	p->buff=(void**)calloc(p->maxCount*4,1);
	listResize(p);
}

void listFree(_list *p)
{
	if( p->buff)
	{
		int i;
		for(i=0;i<p->count;i++)
		{
			free( p->buff[i] );
		}
		free( p->buff);
	}
}

int listAdd(_list *l,void *v)
{
	void *m=malloc(l->typeSize);
	listResize(l);
	memcpy(m,v,l->typeSize);
	//memcpy(l->buff+sizeof(int)*l->count,m
	l->buff[l->count]=m;
	l->count++;
	return l->count-1;
}

void listCopy(_list *dst,_list *src)
{
	int i;
	void *m;
	//free dst memory
	for( i=0; i<dst->count; i++ )
	{
		free(dst->buff[i]);
	}

	dst->count=src->count;
	dst->maxCount=src->maxCount;
	dst->typeSize=src->typeSize;

	for(i=0; i<src->count; i++ )
	{
		m=malloc(src->typeSize);
		memcpy(m,src->buff[i],src->typeSize);
		dst->buff[i]=m;
	}
}

void listRemove(_list *l, int i)
{
	if(i<l->count && i>=0)
	{
		free(l->buff[i]);
		memcpy(l->buff+i, l->buff+i+1,4* (l->count-i-1));	
		l->count--;
	}
}

//=listGetByIndex
void* listGet(_list *l, int i)
{
	if(i<l->count && i>=0)
	{
		return l->buff[i];
		//memcpy(v,l->buff+l->typeSize*i,l->typeSize);
	}
	return NULL;
}

/*
void listModify(_list *l, int i, void* v)
{
	if(i<l->count && i>=0)
	{
		memcpy(l->buff+l->typeSize*i,v,l->typeSize);
	}
}
*/

