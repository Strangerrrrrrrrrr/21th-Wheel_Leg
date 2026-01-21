/*
 * servo.c
 *
 *  Created on: 2025年4月6日
 *      Author: 榴星宇
 */


#include"servo.h"

//float center1= 0,center2= 0,center3=0,center4=0;
float center1= 4650,center2= 4700,center3=4500,center4=4700;//
//float center1= 4650,center2= 4550,center3=4500,center4=4700;//
uint8_t lowest = 70;
uint8_t highest = 130;
IKParamTypeDef IKParam;
int16_t alphaLeftToAngle,betaLeftToAngle,alphaRightToAngle,betaRightToAngle;  //弧度转换成角度

void servo_init(void)
{

       pwm_init(ATOM0_CH1_P21_3, 300, center1);  //左上815  7960-1800// ATOM0_CH1_P21_3
       pwm_init(ATOM1_CH0_P21_2, 300, center2);  //右下765  7580-1580//ATOM1_CH0_P21_2,
       pwm_init(ATOM1_CH2_P21_4, 300, center3);  //左下725  1200-7380//ATOM1_CH2_P21_4
       pwm_init(ATOM0_CH3_P21_5 ,300, center4);  //右上715  1260-7300//ATOM0_CH3_P21_5 精度不高




}

//_constrain(dushu,10,170);   //加个限幅函数
//    pwm_set_duty(ATOM0_CH1_P21_3, 7850-dushu / 180 *6000);       //左上       7490-Angle_set / 180 `*5780
//    pwm_set_duty(ATOM0_CH3_P21_5, dushu/ 180 *6000  + 1550);    //右上,       Angle_set/ 180 *5920  + 1400
//    pwm_set_duty(ATOM1_CH2_P21_4, dushu/ 180 *6000  + 1650);  //左下         Angle_set/ 180 *5780  + 1710
//    pwm_set_duty(ATOM1_CH0_P21_2, 7400-dushu / 180 *6000 );   //右下          7560-Angle_set / 180 *5830   6000
//


void  servo_duty(float dushu)  //目前的精度不够  比例转换
{
    _constrain(dushu,10,170);   //加个限幅函数
        pwm_set_duty(ATOM0_CH1_P21_3, 7650-dushu / 180 *6000);       //左上       7490-Angle_set / 180 `*5780
        pwm_set_duty(ATOM0_CH3_P21_5, dushu/ 180 *6000  +1700);    //右上,       Angle_set/ 180 *5920  + 1400
        pwm_set_duty(ATOM1_CH2_P21_4, dushu/ 180 *6000  + 1500);  //左下         Angle_set/ 180 *5780  + 1710
        pwm_set_duty(ATOM1_CH0_P21_2, 7700-dushu / 180 *6000 );   //右下          7560-Angle_set / 180 *5830   6000
}


void X_Y_Set(float X ,float Y)
 {
  _constrain(Y , 28, 145);//X Y坐标限幅
  _constrain(X , -37, 75);
  //腿部坐标
  IKParam.XLeft = X;
  IKParam.XRight = X;
  IKParam.YLeft = Y;
  IKParam.YRight = Y;
}
/************************************/
/*         ***************
          *               *
 *       *                 *
 *     *                    *
 *    *                      *
 *   *                        *
 *  *                          *
 *   *                        *
 *    *                      *
 *     *                    *
        *                  *
         *                *
 *        *              *
 *         *            *
 *          *          *
 *           *        *
 *            *      *
 *             *    *
 *              *  *
 *********************************/
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机角度设置
//-------------------------------------------------------------------------------------------------------------------
void set_Angle(float servoLeftFront, float servoLeftRear ,float servoRightFront, float servoRightRear)
{
    if(servoLeftFront<20)servoLeftFront=20;
    if(servoLeftRear<20)servoLeftRear=20;
    if(servoRightFront<20)servoRightFront=20;
    if(servoRightRear<20)servoRightRear=20;

    if(servoLeftFront>170)servoLeftFront=170;
    if(servoLeftRear>170)servoLeftRear=170;
    if(servoRightFront>170)servoRightFront=170;
    if(servoRightRear>170)servoRightRear=170;


    pwm_set_duty(ATOM0_CH1_P21_3, 7650-servoRightFront/ 180 *6000  );    //左上               7490-servoRightFront / 180 *5780
    pwm_set_duty(ATOM1_CH2_P21_4, servoLeftFront / 180 *6000 + 1500);    //左下                servoLeftFront / 180 *5780 + 1710
    pwm_set_duty(ATOM0_CH3_P21_5, servoRightRear/ 180 *6000  +1700);      //右上                  servoRightRear/ 180 *5920  + 1400
    pwm_set_duty(ATOM1_CH0_P21_2, 7700-servoLeftRear / 180 *6000   );      //右下                  7560-servoLeftRear / 180 *5830




}



void assist(float A1,float A2)  //腿部辅助控制     此处为增量式pid但是精度不够高
{

   if(A1<-25)
    {
        A1=-25;
        A2=-25;
   }

   if(A1>25)
    {
        A1=25;
        A2=25;
   }
   A1=90-A1;
   A2=90+A2;
//
//

////记得更改
     pwm_set_duty(ATOM0_CH1_P21_3, 7650-A2 / 180 * 6000 );       //左上
     pwm_set_duty(ATOM1_CH2_P21_4, A2 / 180 *6000+ 1500 );       //左下
     pwm_set_duty(ATOM0_CH3_P21_5, A1/ 180 *6000 + 1700 );       //右上
     pwm_set_duty(ATOM1_CH0_P21_2, 7700-A1 / 180 * 6000 );       //右下



}






//舵机辅助平衡控制函数
float Target_Angle = 0,Target_Angle_Calculate;

float SPEED_TO_ANGLE_GAIN   = 0.1;
float SERVO_ANGLE_INCREMENT = 0.09;

void Servo_Control(void){

    Target_Angle_Calculate = -((speed_Temp - Encoder_pre ) * SPEED_TO_ANGLE_GAIN );

    if(Target_Angle > Target_Angle_Calculate) Target_Angle = Target_Angle - SERVO_ANGLE_INCREMENT;
    if(Target_Angle < Target_Angle_Calculate) Target_Angle = Target_Angle + SERVO_ANGLE_INCREMENT;

    assist(Target_Angle, Target_Angle);
}




//// ========================五连杆逆运算===========================//运动学逆解

 void inverseKinematics(void)
{

float alpha1,alpha2,beta1,beta2;      //弧度角

uint16_t servoLeftFront,servoLeftRear;  //duo'ji


float aLeft = 2 * IKParam.XLeft * L1;
float bLeft = 2 * IKParam.YLeft * L1;

float cLeft = 0;

float dLeft = 2 * L4 * (IKParam.XLeft - L5);
float eLeft = 2 * L4 * IKParam.YLeft;
float fLeft = ((IKParam.XLeft - L5) * (IKParam.XLeft - L5) + L4 * L4 + IKParam.YLeft * IKParam.YLeft - L3 * L3);

cLeft = IKParam.XLeft * IKParam.XLeft + IKParam.YLeft * IKParam.YLeft + L1 * L1 - L2 * L2;

alpha1 = 2 * atan((bLeft + sqrt((aLeft * aLeft) + (bLeft * bLeft) - (cLeft * cLeft))) / (aLeft + cLeft));
alpha2 = 2 * atan((bLeft - sqrt((aLeft * aLeft) + (bLeft * bLeft) - (cLeft * cLeft))) / (aLeft + cLeft));
beta1 = 2 * atan((eLeft + sqrt((dLeft * dLeft) + eLeft * eLeft - (fLeft * fLeft))) / (dLeft + fLeft));
beta2 = 2 * atan((eLeft - sqrt((dLeft * dLeft) + eLeft * eLeft - (fLeft * fLeft))) / (dLeft + fLeft));


alpha1 = (alpha1 >= 0)?alpha1:(alpha1 + 2 * PI);
alpha2 = (alpha2 >= 0)?alpha2:(alpha2 + 2 * PI);




if(alpha1 >= PI/4) IKParam.alphaLeft = alpha1;
else IKParam.alphaLeft = alpha2;
if(beta1 >= 0 && beta1 <= PI/4) IKParam.betaLeft = beta1;
else IKParam.betaLeft = beta2;

alphaLeftToAngle = (int)((IKParam.alphaLeft / 6.28) * 360);//弧度转角度
betaLeftToAngle = (int)((IKParam.betaLeft / 6.28) * 360);


servoLeftFront= 270- alphaLeftToAngle;
servoLeftRear = 90 +betaLeftToAngle;

  uint16_t servoRightFront, servoRightRear;
  float aRight = 2 * IKParam.XRight * L1;
  float bRight = 2 * IKParam.YRight * L1;
  float cRight = IKParam.XRight * IKParam.XRight + IKParam.YRight * IKParam.YRight + L1 * L1 - L2 * L2;
  float dRight = 2 * L4 * (IKParam.XRight - L5);
  float eRight = 2 * L4 * IKParam.YRight;
  float fRight = ((IKParam.XRight - L5) * (IKParam.XRight - L5) + L4 * L4 + IKParam.YRight * IKParam.YRight - L3 * L3);

  IKParam.alphaRight = 2 * atan((bRight + sqrt((aRight * aRight) + (bRight * bRight) - (cRight * cRight))) / (aRight + cRight));
  IKParam.betaRight = 2 * atan((eRight - sqrt((dRight * dRight) + eRight * eRight - (fRight * fRight))) / (dRight + fRight));


   alpha1 = 2 * atan((bRight + sqrt((aRight * aRight) + (bRight * bRight) - (cRight * cRight))) / (aRight + cRight));
   alpha2 = 2 * atan((bRight - sqrt((aRight * aRight) + (bRight * bRight) - (cRight * cRight))) / (aRight + cRight));
   beta1 = 2 * atan((eRight + sqrt((dRight * dRight) + eRight * eRight - (fRight * fRight))) / (dRight + fRight));
   beta2 = 2 * atan((eRight - sqrt((dRight * dRight) + eRight * eRight - (fRight * fRight))) / (dRight + fRight));

   alpha1 = (alpha1 >= 0)?alpha1:(alpha1 + 2 * PI);
   alpha2 = (alpha2 >= 0)?alpha2:(alpha2 + 2 * PI);
   if(alpha1 >= PI/4) IKParam.alphaRight = alpha1;
   else IKParam.alphaRight = alpha2;
   if(beta1 >= 0 && beta1 <= PI/4) IKParam.betaRight = beta1;
   else IKParam.betaRight = beta2;
   alphaRightToAngle = (int)((IKParam.alphaRight / 6.28) * 360);
   betaRightToAngle = (int)((IKParam.betaRight / 6.28) * 360);




   servoRightFront = 270 - alphaRightToAngle;
   servoRightRear  = 90 + betaRightToAngle;


   set_Angle(servoLeftFront,servoLeftRear,servoRightFront,servoRightRear);
}























