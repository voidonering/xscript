/*
垃圾清理
清理无效的引用，释放内存地址
*/
#include "xsconfig.h"
#include "xsgc.h"
#include "xsdef.h"
#include "xs.h"
#include "xslog.h"
#include "xshash.h"
//var a=createObject();
//var b=a;
//var a=0;
//#define DEBUG_IN_SYMBIAN
#ifdef XS_DEBUG_GC
char xslogbuf[256];
#endif


static unsigned int hashfromkey(void *k)
{
    return (unsigned int)k;
}

static int equalkeys(void *k1, void *k2)
{
    return k1==k2;
}

struct hashtable * xs_createGc()
{
	return create_hashtable(32, hashfromkey, equalkeys);
}

void xs_gcAddObject(_xsvm *vm, xsuint o,disposeapi dispose)
{	
	_objectReference *oo=(_objectReference *)hashtable_search( vm->gch, (void*)o);
	if( oo )
	{
		oo->refCount++;
		#ifdef XS_DEBUG_GC			
		sprintf(xslogbuf,"++object:%d, %d",o, oo->refCount);
		xs_log(xslogbuf);
		#endif	
	}
	else
	{		
		_objectReference *oref=(_objectReference*)malloc(sizeof(_objectReference));
		oref->object=o;
		oref->refCount=1;
		oref->dispose=dispose;
		hashtable_insert(vm->gch, (void*)o, oref );
		
		#ifdef XS_DEBUG_GC			
		sprintf(xslogbuf,"add object:%d, %d",o, oref->refCount);
		xs_log(xslogbuf);
		#endif	
	}
	
}

void xs_addRef(_xsvm *vm, xsuint o)
{
	_objectReference *oo=(_objectReference*)hashtable_search( vm->gch, (void*)o);
	if( oo )
	{
		oo->refCount++;
		#ifdef XS_DEBUG_GC			
		sprintf(xslogbuf,"++object:%d, %d",o, oo->refCount);
		xs_log(xslogbuf);
		#endif	
	}
}

//删除一切对象
void xs_removeAllRef(_xsvm *vm)
{
	//vm->gch
	return;
}

int xs_removeRef(_xsvm *vm, xsuint o)
{
	_objectReference *oo=(_objectReference*)hashtable_search( vm->gch, (void*)o);
	if( oo )
	{
		oo->refCount--;
		#ifdef XS_DEBUG_GC			
		sprintf(xslogbuf,"--object:%d, %d",o, oo->refCount);
		xs_log(xslogbuf);
		#endif	
		if(oo->refCount<=0)
		{
			//free object
			oo->dispose(o);
			hashtable_remove(vm->gch, (void*)o);

			free(oo);
			#ifdef XS_DEBUG_GC			
			sprintf(xslogbuf,"free object:%d",o);
			xs_log(xslogbuf);
			#endif				
		}
		
		return 1;
	}
	else
	{
		return 0;
	}
}

