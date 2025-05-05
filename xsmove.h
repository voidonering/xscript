#ifndef XSMOVE_H_
#define XSMOVE_H_


/*实现一些动画移动的特效
 * 线性移动
 * 加速运动
 * 正弦运动
 * 模拟弹球
 * 等11种运动方式
 * 
 * b: begin
 * t: current time
 * c: change in value
 * d: duration
 * easeIn：从0开始加速的缓动；
 * easeOut：减速到0的缓动；
 *
 */ 

#ifdef __cplusplus
extern "C" {
#endif
	
enum _MOVE_MODE
{
	//0
	MOVE_MODE_LINEAR,
	
	//1-3
	MOVE_MODE_QUAD_EASEIN,
	MOVE_MODE_QUAD_EASEOUT,
	MOVE_MODE_EASEINOUT,
	
	//4-6
	MOVE_MODE_CUBIC_EASEIN,
	MOVE_MODE_CUBIC_EASEOUT,
	MOVE_MODE_CUBIC_EASEINOUT,
	
	//7-9
	MOVE_MODE_QUART_EASEIN,
	MOVE_MODE_QUART_EASEOUT,
	MOVE_MODE_QUART_EASEINOUT,

	//10-12
	MOVE_MODE_QUINT_EASEIN,
	MOVE_MODE_QUINT_EASEOUT,
	MOVE_MODE_QUINT_EASEINOUT,

	//13-15
	MOVE_MODE_SINE_EASEIN,
	MOVE_MODE_SINE_EASEOUT,
	MOVE_MODE_SINE_EASEINOUT,

	//16-18
	MOVE_MODE_EXPO_EASEIN,
	MOVE_MODE_EXPO_EASEOUT,
	MOVE_MODE_EXPO_EASEINOUT,

	//19-21
	MOVE_MODE_CIRC_EASEIN,
	MOVE_MODE_CIRC_EASEOUT,
	MOVE_MODE_CIRC_EASEINOUT,

	//22-24
	MOVE_MODE_ELASTIC_EASEIN,
	MOVE_MODE_ELASTIC_EASEOUT,
	MOVE_MODE_ELASTIC_EASEINOUT,

	//25-27
	MOVE_MODE_BACK_EASEIN,
	MOVE_MODE_BACK_EASEOUT,
	MOVE_MODE_BACK_EASEINOUT,

	//29-31
	MOVE_MODE_BOUNCE_EASEIN,
	MOVE_MODE_BOUNCE_EASEOUT,
	MOVE_MODE_BOUNCE_EASEINOUT,
		
};

double xs_Linear(double t, double c, double d);

double xs_Quad_easeIn(double t, double c, double d) ;
double xs_Quad_easeOut(double t, double c, double d);
double xs_Quad_easeInOut(double t, double c, double d);

double xs_Cubic_easeIn(double t, double c, double d);
double xs_Cubic_easeOut(double t, double c, double d);
double xs_Cubic_easeInOut(double t, double c, double d);

double xs_Quart_easeIn(double t,  double c, double d);
double xs_Quart_easeOut(double t,  double c, double d);
double xs_Quart_easeInOut(double t, double c, double d);

double xs_Quint_easeIn(double t, double c, double d);
double xs_Quint_easeOut(double t, double c, double d);
double xs_Quint_easeInOut(double t, double c, double d);

double xs_Sine_easeIn(double t,  double c, double d, double cycles);
double xs_Sine_easeOut(double t,  double c, double d);
double xs_Sine_easeInOut(double t,  double c, double d);

double xs_Expo_easeIn(double t, double c, double d);
double xs_Expo_easeOut(double t, double c, double d);
double xs_Expo_easeInOut(double t, double c, double d);

double xs_Circ_easeIn(double t,  double c, double d);
double xs_Circ_easeOut(double t,  double c, double d);
double xs_Circ_easeInOut(double t,double c, double d);

double xs_Elastic_easeIn(double t, double c, double d,double a,double p);
double xs_Elastic_easeOut(double t, double c, double d,double a,double p);
double xs_Elastic_easeInOut(double t, double c, double d, double a, double p);

double xs_Back_easeIn(double t,  double c, double d,double s);
double xs_Back_easeOut(double t,  double c, double d,double s);
double xs_Back_easeInOut(double t, double c, double d, double s);

double xs_Bounce_easeIn(double t, double c, double d);
double xs_Bounce_easeOut(double t, double c, double d);
double xs_Bounce_easeInOut(double t, double c, double d);

#ifdef __cplusplus
}
#endif

#endif /*XSMOVE_H_*/
