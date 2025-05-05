/*
2009/03/09 TangJun
standard math library
*/
#include <stdlib.h>
#include <math.h>
#include "xsmathlib.h"

#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)

void math_abs (_xsvm *vm) 
{
	xs_pushNumber(vm, fabs(xs_popNumber(vm)) );  
}

void math_sin (_xsvm *vm) 
{
	xs_pushNumber(vm, sin(xs_popNumber(vm)) );    
}

void math_sinh (_xsvm *vm) 
{
	xs_pushNumber(vm, sinh(xs_popNumber(vm)) );    
}

void math_cos (_xsvm *vm) 
{
	xs_pushNumber(vm, cos(xs_popNumber(vm)) );      
}

void math_cosh (_xsvm *vm) 
{
	xs_pushNumber(vm, cosh(xs_popNumber(vm)) );    
}

void math_tan (_xsvm *vm) 
{
	xs_pushNumber(vm, tan(xs_popNumber(vm)) );    
}

void math_tanh (_xsvm *vm) 
{
	xs_pushNumber(vm, tanh(xs_popNumber(vm)) );      
}

void math_asin (_xsvm *vm) 
{
	xs_pushNumber(vm, asin(xs_popNumber(vm)) );    
}

void math_acos (_xsvm *vm) 
{
	xs_pushNumber(vm, acos(xs_popNumber(vm)) );    
}

void math_atan (_xsvm *vm) 
{
	xs_pushNumber(vm, atan(xs_popNumber(vm)) );     
}

void math_ceil (_xsvm *vm) 
{
	xs_pushNumber(vm, ceil(xs_popNumber(vm)) );    
}

void math_toint (_xsvm *vm) 
{
	int v=(int)xs_popNumber(vm);
	xs_pushNumber(vm,  v);    
}

void math_floor (_xsvm *vm) 
{
	xs_pushNumber(vm, floor(xs_popNumber(vm)) );    
}

void math_fmod (_xsvm *vm) 
{
	double vr=xs_popNumber(vm);
	double vl=xs_popNumber(vm);
	xs_pushNumber( vm,fmod(vl,vr) );    
}

void math_sqrt (_xsvm *vm) 
{
	xs_pushNumber(vm, sqrt(xs_popNumber(vm)) );    
}

void math_pow (_xsvm *vm) 
{
	double vr=xs_popNumber(vm);
	double vl=xs_popNumber(vm);
	xs_pushNumber(vm, pow(vl,vr) );    
}

void math_log (_xsvm *vm) 
{
	xs_pushNumber( vm,log(xs_popNumber(vm)) );    
}

void math_log10 (_xsvm *vm) 
{
	xs_pushNumber(vm, log10(xs_popNumber(vm)) );    
}

void math_exp (_xsvm *vm) 
{
	xs_pushNumber(vm, exp(xs_popNumber(vm)) );    
}

void math_deg (_xsvm *vm) 
{
	xs_pushNumber(vm, xs_popNumber(vm)/RADIANS_PER_DEGREE );    
}

void math_rad (_xsvm *vm) 
{
	xs_pushNumber(vm, xs_popNumber(vm)*RADIANS_PER_DEGREE );    
}

void math_random (_xsvm *vm) 
{
	xs_pushNumber(vm, rand() );  
}

void usemathlib(_xsvm *vm) 
{
    xs_register(vm,"math_abs", 1,  math_abs);
    xs_register(vm,"math_acos", 1, math_acos);
    xs_register(vm,"math_asin", 1, math_asin);
    xs_register(vm,"math_atan", 1, math_atan);
    xs_register(vm,"math_ceil", 1, math_ceil);
    xs_register(vm,"math_cosh", 1,  math_cosh);
    xs_register(vm,"math_cos", 1,  math_cos);
    xs_register(vm,"math_deg",  1, math_deg);
    xs_register(vm,"math_exp",  1, math_exp);
    xs_register(vm,"math_floor",1, math_floor);
    xs_register(vm,"math_toint",1, math_toint);
        
    xs_register(vm,"math_fmod",  2, math_fmod);
    xs_register(vm,"math_log10",1, math_log10);
    xs_register(vm,"math_log",  1, math_log);
    xs_register(vm,"math_pow",  2, math_pow);
    xs_register(vm,"math_rad",  1, math_rad);
    xs_register(vm,"math_random",  1,   math_random);
    xs_register(vm,"math_sinh",  1, math_sinh);
    xs_register(vm,"math_sin",  1, math_sin);
    xs_register(vm,"math_sqrt",1,  math_sqrt);
    xs_register(vm,"math_tanh", 1,  math_tanh);
    xs_register(vm,"math_tan",  1, math_tan);
}

