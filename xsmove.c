
/*实现一些动画移动的特效
 * 线性移动
 * 加速运动
 * 正弦运动
 * 模拟弹球
 * 等
 */ 
#include "xsmove.h"
#include "math.h"
#undef PI
#define PI (3.14159265358979323846)

double xs_Linear(double t, double c, double d)
{ 
	return c*t/d; 
}
double xs_Quad_easeIn(double t, double c, double d) 
{        
	t=t/d;
	return c*t*t;
}
double xs_Quad_easeOut(double t, double c, double d)
{
	t=t/d;
    return -c *t*(t-2);
}
double xs_Quad_easeInOut(double t, double c, double d)
{
	t=2*t/d;
    if (t < 1)
    {
    	return c/2*t*t;
    }
    t--;
    return -c/2 * (t*(t-2) - 1);
}

double xs_Cubic_easeIn(double t, double c, double d)
{
	t=t/d;
    return c*t*t*t;
}
double xs_Cubic_easeOut(double t, double c, double d)
{
	t=t/d-1;
    return c*(t*t*t + 1) ;
}
double xs_Cubic_easeInOut(double t, double c, double d)
{
	t=2*t/d;
    if (t < 1) return c/2*t*t*t;
    t=t-2;
    return c/2*(t*t*t + 2);
}
double xs_Quart_easeIn(double t,  double c, double d)
{
	t=t/d;
    return c*t*t*t*t ;
}
double xs_Quart_easeOut(double t,  double c, double d)
{
	t=t/d-1;
    return -c * (t*t*t*t - 1) ;
}
double xs_Quart_easeInOut(double t, double c, double d)
{
	t=2*t/d;
    if (t < 1) return c/2*t*t*t*t ;
    
    t=t-2;
    return -c/2 * (t*t*t*t - 2);
}
double xs_Quint_easeIn(double t,  double c, double d)
{
	t=t/d;
    return c*t*t*t*t*t ;
}
double xs_Quint_easeOut(double t, double c, double d)
{
	t=t/d-1;
    return c*(t*t*t*t*t + 1) ;
}
double xs_Quint_easeInOut(double t, double c, double d)
{
	t=2*t/d;
    if (t < 1) return c/2*t*t*t*t*t ;
    
    t=t-2;
    return c/2*(t*t*t*t*t + 2) ;
}

double xs_Sine_easeIn(double t,  double c, double d, double cycles)
{
    //return -c * cos(t/d * (PI/2)) + c ;
    return c * sin(t/d * 2* PI * cycles +PI/2) ;  
}
double xs_Sine_easeOut(double t,  double c, double d)
{
    //return c * sin(t/d * (PI/2)) ;
	//完整周期2π
    return c * sin(t/d * 2* PI) ;  
}
double xs_Sine_easeInOut(double t,  double c, double d)
{
    return -c/2 * (cos(PI*t/d) - 1) ;
}

double xs_Expo_easeIn(double t, double c, double d)
{
    return (t==0) ? 0 : c * pow(2, 10 * (t/d - 1)) ;
}
double xs_Expo_easeOut(double t,  double c, double d)
{
    return (t==d) ? 0+c : c * (-pow(2, -10 * t/d) + 1) ;
}
double xs_Expo_easeInOut(double t,  double c, double d)
{
    if (t==0) return 0;
    if (t==d) return c;
    t=2*t/d;
    if (t < 1) return c/2 * pow(2, 10 * (t - 1)) ;
    t--;
    return c/2 * (-pow(2, -10 * t) + 2) ;
}

double xs_Circ_easeIn(double t, double c, double d)
{
	t=t/d;
    return -c * (sqrt(1 - t*t) - 1) ;
}
double xs_Circ_easeOut(double t, double c, double d)
{
	t=t/d-1;
    return c * sqrt(1 - t*t) ;
}
double xs_Circ_easeInOut(double t,  double c, double d)
{
	t=2*t/d;
    if (t < 1) return -c/2 * (sqrt(1 - t*t) - 1) ;
    
    t=t-2;
    return c/2 * (sqrt(1 - t*t) + 1) ;
}

double xs_Elastic_easeIn(double t, double c, double d,double a,double p)
{
    double s;
    if (t==0) return 0;  
    t=t/d;
    if (t==1) return c;  
    if (!p) p=d*0.3;
    if (!a || a < fabs(c)) 
    { 
    	a=c; 
    	s=p/4; 
    }
    else 
    	s = p/(2*PI) * asin (c/a);
    
    t--;
    return -(a*pow(2,10*t) * sin( (t*d-s)*(2*PI)/p )) ;
}
double xs_Elastic_easeOut(double t, double c, double d,double a,double p)
{
    double s;
	if (t==0) return 0;  
	t=t/d;
    if (t==1) return c;  
    if (!p) p=d*0.3;

    if (!a || a < fabs(c)) 
    { 
    	a=c; 
    	s=p/4; 
    }
    else 
    	s = p/(2*PI) * asin (c/a);
    
    return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c);
}
double xs_Elastic_easeInOut(double t, double c, double d, double a, double p)
{    
    double s;
    if (t==0) return 0;  
    t=t*2/d;
    if (t==2) return c;  
    if (!p) p=d*(0.3*1.5);

    if (!a || a < fabs(c)) 
    { 
    	a=c; 
    	s=p/4; 
    }
    else 
    {
    	s = p/(2*PI) * asin (c/a);
    }
    
    t--;
    if (t < 1) 
    {
    	return -0.5*(a*pow(2,10*t) * sin( (t*d-s)*(2*PI)/p )) ;
    }
    
    return a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p )*0.5 + c;
}

double xs_Back_easeIn(double t, double c, double d,double s)
{
	t=t/d;
    if (s == 0) s = 1.70158;
    return c*t*t*((s+1)*t - s) ;
}
double xs_Back_easeOut(double t,  double c, double d,double s)
{
	t=t/d-1;
    if (s == 0) s = 1.70158;
    return c*(t*t*((s+1)*t + s) + 1) ;
}
double xs_Back_easeInOut(double t,  double c, double d, double s)
{
    if (s == 0) s = 1.70158; 
    t=2*t/d;
    s=s*1.525;
    if (t < 1) return c/2*(t*t*((s+1)*t - s)) ;
    
    t-=2;
    s=s*1.525;
    return c/2*(t*t*((s+1)*t + s) + 2) ;
}

double xs_Bounce_easeOut(double t,  double c, double d)
{
	t=t/d;
    if (t < (1/2.75)) 
    {
        return c*(7.5625*t*t);
    } 
    else if (t < (2/2.75)) 
    {
    	t=t-1.5/2.75;
        return c*(7.5625*t*t + 0.75);
    } 
    else if (t < (2.5/2.75)) 
    {
    	t=t-2.25/2.75;
        return c*(7.5625*t*t + 0.9375) ;
    } 
    else 
    {
    	t=t-2.625/2.75;
        return c*(7.5625*t*t + 0.984375) ;
    }
}

double xs_Bounce_easeIn(double t, double c, double d)
{
    return c - xs_Bounce_easeOut(d-t, c, d) ;
}

double xs_Bounce_easeInOut(double t,  double c, double d)
{
    if (t < d/2) 
    {
    	return xs_Bounce_easeIn(t*2, c, d) * 0.5 ;
    }
    else 
    	return xs_Bounce_easeOut(t*2-d, c, d) * 0.5 + c*0.5 ;
}
