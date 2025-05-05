#ifndef _XSPARSER_H
#define _XSPARSER_H

#define XSGO "$G"
#define XSGLOBALOBJECT "$G."
#define XSONUPDATE "$G.onUpdate"
#define XSONEVENT "$G.onEvent"
#define isMatch(p1,p2) strcmp(p1,p2)==0
#define XSIF "if"
#define XSELSEIF "elseif"
#define XSELSE "else"
#define XSWHILE "while"
#define XSFOR "for"
#define XSINCLUDE "#include"
#define XSSWITCH "switch"
#define XSFUNCTION "function"
#define XSCLASS "class"
#define XSNEW "new"
#define XSVAR "var"
#define XSBREAK "break"
#define XSCONTINUE "continue"
#define XSRETURN "return"
#define XSINC "++"
#define XSDEC "--"
#define XSLE "<="
#define XSLE2 "=<"
#define XSGE ">="
#define XSGE2 "=>"
#define XSEQ "=="
#define XSNE "!="
#define XSOR "||"
#define XSAND "&&"
#define XSCONC ".."



int parseStatements(_xsvm *vm);
int parseStatement(_xsvm *vm);
int parseExpression(_xsvm *vm);
int parseBody(_xsvm *vm);

int parseAssign(_xsvm *vm);
int parseArrayInitialize(_xsvm *vm, char *ptemp);
int parseFunctionDefine(_xsvm *vm);
int parseFunctionArgs(_xsvm *vm);
int parseFunctionCall(_xsvm *vm);
int parseVariableDefine(_xsvm *vm);
int parseFunctionCallArgs(_xsvm *vm, int* paramCount);
int parseCallAPI(_xsvm *vm, char* apiName,int paramCount);

int parseClassDefine(_xsvm *vm);
int parseNewClass(_xsvm *vm);

int parseLoopFor(_xsvm *vm);
int parseLoopWhile(_xsvm *vm);
//int parseSwitch(_xsvm *vm);

int parseIf(_xsvm *vm);

//void parseValue(_xsvm *vm);

int toNumber(char* p, double *rt);

//int toString(_value v, char *p);
int isVariable(char *p);
int isString(char *p);
int isNumber(char* p);
char *toCString(char *p);

#endif

