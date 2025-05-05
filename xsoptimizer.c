
#include "xsoptimizer.h"
#include "xs.h"

void xs_optimize(_xsvm *vm)
{
	_stack *gc=&vm->gcode;
	_instruction *ins;
	_instruction *pre;
	_instruction *pre2;
	int *ccount;
	
	if( vm->currentObject!=vm->globalObject )
	{
		gc=&vm->code;
	}

	ccount=&(gc->count);
	ins=(_instruction *)gc->buff+*ccount-1;
	pre=ins-1;
	pre2=ins-2;
	
	switch(ins->opCode)
	{
		case opPUSH:
		case opPOP:
		{
			break;
		}
		case opJNZ_TEMP:
		{
			int opCode=0;

			if( *ccount<3 ) return ;
			ins->address2=0;
			switch( pre->opCode )
			{
				case opGT: opCode=opJG;break;
				case opGE: opCode=opJGE;break;
				case opLT: opCode=opJL;break;
				case opLE: opCode=opJLE;break;
				case opEQ: opCode=opJEQ;break;
				case opNE: opCode=opJNE;break;  
			}
			if( opCode>0 )
			{
				gc->count--;
				ins->address2=opCode;
				*pre=*ins;
			}
			break;
		}
		case opASSIGN:
		{
			ins->scope2=XSSTACK;
			if(*ccount>1 && pre->opCode==opPUSH )
			{
				ins->scope2=pre->scope;
				ins->address2=pre->address;
				*pre=*ins;
				*ccount=*ccount-1;
			}
			break;
		}
		case opADD:
		case opSUB:
		case opDIV:
		case opMUL:
		case opMOD:
		{
			ins->scope2=XSSTACK;
			ins->scope=XSSTACK;
			if(*ccount>2 )
			{
				if( pre->opCode==opPUSH )
				{
					ins->address=pre->address;
					ins->scope=pre->scope;
					*pre=*ins;
					*ccount=*ccount-1;
					if(pre2->opCode==opPUSH )
					{
						ins->address2=pre2->address;
						ins->scope2=pre2->scope;
						*pre2=*ins;
						*ccount=*ccount-1;
					}
				}
			}
			break;
		}
		case opNEG:
		{
			ins->scope=XSSTACK;
			if(*ccount>1 && pre->opCode==opPUSH )
			{
				ins->scope=pre->scope;
				ins->address=pre->address;
				*pre=*ins;
				*ccount=*ccount-1;
			}
			
			break;
		}

	}

}
