/*
 * pid.h
 *
 *  Created on: 2025年3月31日
 *      Author: 榴星宇
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_
#include "zf_common_headfile.h"


typedef struct
{

   float  output;//输出写错了
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_v;

typedef struct
{
   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_J;


typedef struct
{
   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_JV;



typedef struct
{
   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_turn;

typedef struct
{

   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_turnz;


typedef struct
{

   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;
}pid_turnsing;


typedef struct
{

   float  output;
   float  err1;
   float  err2;
   float  sum;
   float  use;

}pid_ROLL;




typedef struct _IMU660RA_Parameter_t
{
    float Pitch;
    float Roll;
    float Yaw;
}IMU660RA_Parameter_t;

extern float Encoder;
extern int duty_YAW;
float speed_Velocity (pid_v*k, float target, float actual_value);
float Gyro_PIDVelocity (pid_JV*k, float target, float actual_value);
float Angle_Velocity (pid_J*k, float target, float actual_value);
//float ROLL_PID (pid_ROLL*k, float target, float actual_value);float TurnzZ_KP = 34  ,Turn_KDz2 = 0.4  ;

extern float ROLL_PID (pid_ROLL*k, float target, float actual_value);
extern pid_ROLL  pidROLL;
extern float Single_SideBridge_Velocity;
extern float speedring_KP, speedring_KI,Gyro_kP, GYRO_KD;
extern float Turn_KP ,Turn_KP2 ,Turn_KD ,Turn_KD2,TurnzZ_KP,TurnzZ_KP2,Turn_KDz2 ;
extern float Angle_KP ,Angle_KD ;
extern float Angle_KP_Temp ;
extern float singletempV;
extern float Zebra_Encoder;
extern float Circle_Encoder;
extern float Cross_Ecoder;
extern float Ramp_Encoder;
extern float Obstacle_Encoder;
extern uint8 okl;
extern float p,p1;
extern float speed_Temp;
extern float Encoder_pre;
extern int Run_Flag ;
extern float Zebra_Encoder;
extern uint8 Tumble_num;
extern uint8 STOP_FLAG ;
extern uint16 TIME2;
extern  float Machine_Mid ;
extern float speed_limit;
extern  IMU660RA_Parameter_t  IMU660RA_Parameter;
extern float gyro_y_lpf,gyro_z_lpf;
extern float Left_OUT_Finally , Right_OUT_Finally ;//电机最后赋值操作
extern float SPEED_INCREMENT;
extern uint8 SPEED_DECREMENT;
extern float S_V_Reduce;
extern int Zebra_STOP_STOP;
void Vehicle_STOP(void);
void AccelerateToTargetSpeed(void);
void Vehicle_START(void);
void single_v (void);
void callback (void);

#endif /* CODE_PID_H_ */
