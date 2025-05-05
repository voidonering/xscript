
#include "xsvm.h"
#include "time.h"

int main(int argc, char *argv[])
{

	_xsvm *vm;
	int rt,fsize;
	double t1,t2;
	char *sbuf;
	FILE *fh;
	char *xsfile="E:\\test.xs";

	if(argc>1)
	{
		xsfile=argv[1];
	}

	fh=fopen(xsfile,"r");
	if(fh==NULL )
	{
		printf("Can not open default test file: %s, press Enter to exit...",xsfile);
		getchar();
		return 0;
	}
	
	fseek(fh,0,2);
	fsize=ftell(fh)+1;
	sbuf=calloc(fsize,1);
	fseek(fh,0,0);
	fread(sbuf,1,fsize,fh);
	fclose(fh);

	vm=xs_init(argv[0]);
	t1=clock();
	xs_compile( vm, sbuf );
	
	if( rt!=XS_SUCCESS)
	{
		printf("error: sourceline %d, symbol:%s\n",vm->lexer.currentLine,vm->lexer.currentToken);
		getchar();
		goto END;
	}
	t2=clock();
	printf("\ncompile time(s): %f\n",(t2-t1)/1000);
	
	printf("\nthe compiled VM Instructions :\n");
	xs_deCompile(vm, NULL);
	printf("\n\npress any key to run script...\n");
	getchar();
	t1=clock();

	if( xs_run(vm)==XS_EXCEPTION )
	{
		printf("\nERROR\n");
	}

	t2=clock();
	
	printf("\nrun time(s): %f\n",(t2-t1)/1000);
	//printVariables(vm);
	
	//printEnv(vm);
END:
	xs_free(vm);
	printf("\npress ENTER to exit...\n");
	getchar();

    return 0;
}
