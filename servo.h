
#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#include "zf_common_headfile.h"


/*=============腿长基本配置===============*/
#define L1     60
#define L2     90
#define L3     90
#define L4     60
#define L5     38

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt))) // 限幅函数

//================================================定义 五连杆解算 参数结构体===========================================
typedef struct
{
   float alphaLeft, betaLeft;
   float alphaRight, betaRight;
   float XLeft,YLeft;
   float XRight, YRight;
}IKParamTypeDef;
extern IKParamTypeDef IKParam;
void inverseKinematics(void);    //运动学逆解函数
extern float Target_Angle,Target_Angle_Calculate;
extern float SPEED_TO_ANGLE_GAIN ;
extern float SERVO_ANGLE_INCREMENT ;
void set_Angle(float servoLeftFront, float servoLeftRear ,float servoRightFront, float servoRightRear);  //运动学逆解
void Servo_Control(void);

void X_Y_Set(float X ,float Y);  //坐标的限幅


void  assist(float A1,float A2);//辅助腿部运动的函数




void  servo_init(void);         //舵机初始化


void  servo_duty(float dushu);  //设置舵机的占空比





#endif /* CODE_SERVO_H_ */









//IKParam.XLeft左腿X坐标                    IKParam.XRight右腿X坐标
//IKParam.YLeft左腿Y坐标                    IKParam.YRight右腿Y坐标
//aLeft左腿中间变量a                         aRight右腿中间变量a
//bLeft左腿中间变量b                         bRight右腿中间变量b
//cLeft左腿中间变量c                         cRight右腿中间变量c
//dLeft左腿中间变量d                         dRight右腿中间变量d
//eLeft左腿中间变量e                         eRight右腿中间变量e
//fLeft左腿中间变量f                         fRight右腿中间变量f
//alpha1后腿二元一次方程所求的两个解          beta1前腿二元一次方程所求的两个解
//alpha2                                    beta2
//IKParam.alphaLeft最终左后腿的α角度         IKParam.alphaRight最终右后腿的α角度
//IKParam.betaLeft最终左前腿的β角度          IKParam.betaRight最终右前腿的β角度
//alphaLeftToAngle左后腿α角度由弧度制转角度   alphaRightToAngle右后腿α角度由弧度制转角度
//betaLeftToAngle左前腿β角度由弧度制转角度    betaRightToAngle右前腿β角度由弧度制转角度
//servoLeftFront左前腿舵机的目标角度          servoRightFront右前腿舵机的目标角度
//servoLeftRear左后腿舵机的目标角度           servoRightRear右后腿舵机的目标角度









