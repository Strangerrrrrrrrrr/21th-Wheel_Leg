/*
 * pid.c
 *
 *  Created on: 2025年3月31日
 *      Author:榴星宇
 */
#include "pid.h"
IMU660RA_Parameter_t  IMU660RA_Parameter;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度环控制器
// 参数说明     float target           float actual_value
// 返回参数     uint32
// 使用示例     speed_Velocity (&pidv, float target, float actual_value);
// 备注信息speedring_KP =0.06 ,   speedring_KI = 0.00095;/
//-------------------------------------------------------------------------------------------------------------------
pid_v  pidv;   //结构体
float speed_Temp=0;
float speed_limit=0;

float speedring_KP =-0.035 ,  speedring_KI =-0.0018 ;//   float speedring_KP = 0.16,   speedring_KI = 0.00045;//  0.0012  0.08    0.0014
float speed_Velocity (pid_v*k, float target, float actual_value)

{
   k->err1   =target-actual_value;
   k->sum    =k->sum+k->err1;
   k->sum    =k->sum>6000?6000:(k->sum<(-6000)?(-6000):k->sum);
   k->output =speedring_KP*k->err1+speedring_KI*k->sum;

   return k->output;

 }





//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角度环控制器
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Angle_Velocity (&pidJ, float target, float actual_value);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
pid_J  pidJ;  //结构体
float Angle_KP =720,Angle_KD =1 ;  //float Angle_KP =400,Angle_KD =1 ;
float Machine_Mid =-4;
float Angle_KP_Temp =720 ;

float Angle_Velocity (pid_J*k, float target, float actual_value)
{
    k->err1   =target-actual_value;
    k->use    =k->err1-k->err2;
    k->err2   =k->err1;
    k->output =Angle_KP*(k->err1)+Angle_KD*(k->use);
    return k->output;

}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角速度环控制器
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Gyro_PIDVelocity ( &pidJv, float target, float actual_value)  =0.79;
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float Gyro_kP=0.79;
float GYRO_KD=0;
//float gyro_y_lpf,gyro_z_lpf  ;       //角速度
pid_JV   pidJV;
float Gyro_PIDVelocity (pid_JV*k, float target, float actual_value)
{
    k->err1   =target-actual_value;
    k->use    =k->err1-k->err2;
    k->err2   =k->err1;
    k->output =Gyro_kP*(k->err1)+GYRO_KD*(k->use);
    return k->output;
 }





//-------------------------------------------------------------------------------------------------------------------
// 函数简介     转向环控制器
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Gyro_PIDVelocity ( &pidJv, float target, float actual_value)
// 备注信息     陀螺仪转向补偿
//-------------------------------------------------------------------------------------------------------------------


pid_turn Turn;
float Turn_KP = 0  ,Turn_KP2 = 0,Turn_KD = 0,Turn_KD2 = 0.22;    //67     0.19  float Turn_KP = 46 ,Turn_KP2 = 0,Turn_KD = 0,Turn_KD2 = 0.18;    //67     0.19
float Turn_PID(pid_turn *k, float target, float actual_value)
{
    //计算当前误差
   k->err1= target-actual_value;
   k->output=Turn_KP * k->err1+Turn_KP2 * (MY_ABS(k->err1))+Turn_KD * (k->err1-k->err2)+Turn_KD2 * (-imu660ra_gyro_z);
   k->err2=k->err1;
   return k->output;

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     转向环控制器
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Gyro_PIDVelocity ( &pidJv, float target, float actual_value)
// 备注信息     陀螺仪转向补偿单边桥的运算
//-------------------------------------------------------------------------------------------------------------------
float Single_SideBridge_Velocity =400;//单边桥行进速度
pid_turnz Turn1;
float TurnzZ_KP = 33  ,TurnzZ_KP2=0.6, Turn_KDz2 = 0.5;
float TurnZ_PID(pid_turnz *k, float target, float actual_value)
{
    //计算当前误差
   k->err1= target-actual_value;
   k->output=TurnzZ_KP * k->err1+TurnzZ_KP2*(k->err1)*(MY_ABS(k->err1))+Turn_KDz2 * (0 - imu660ra_gyro_z);
   return k->output;

}







//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滚转角控制器
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Gyro_PIDVelocity ( &pidJv, float target, float actual_value)
// 备注信息     陀螺仪转向补偿
//-------------------------------------------------------------------------------------------------------------------


pid_ROLL  pidROLL;
float ROll_KP=0.032;    float ROll_KD= 0.32;    float ROll_KI=0;
float ROLL_PID (pid_ROLL*k, float target, float actual_value)
 {

    k->err1= target- actual_value;
    k->sum +=k->err1;
    k->use=k->err1-k->err2;
    k->err2=k->err1;
    k->output= ROll_KP * k->err1+ROll_KD * k->use;
    return k->output;


 }
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      加速函数
float SPEED_INCREMENT = 20;
void Vehicle_START(void){
    if ( speed_Temp < speed_limit) {
         speed_Temp+= SPEED_INCREMENT;

            //速度限幅
            if(speed_Temp >=  speed_limit)
            {
                speed_Temp =   speed_limit;
            }

         } else {
             speed_Temp =  speed_limit;
         }
    }



void single_v (void)
{

    if ( speed_Temp < singletempV)
    {
            speed_Temp+= 10;

               //速度限幅
               if(speed_Temp >=  singletempV)
               {
                   speed_Temp =   singletempV;
               }


              else {
                speed_Temp =  singletempV;
                }
       }
    }



// 函数简介减速函数
uint8 SPEED_DECREMENT = 50;
void Vehicle_STOP(void)
{
    if (speed_Temp > 75)
    {
        speed_Temp -= SPEED_DECREMENT;

        //减速限幅
        if(speed_Temp < 75)
        {
            speed_Temp = 0;
        }

    }
      else {
          speed_Temp = 0;
     }
}



//float Single_SideBridge_Velocity = 180;//单边桥行进速度
uint8 STOP_FLAG = 0;
uint8 okl=0;
uint16 num = 0;
float Zebra_Encoder  = 0;
float Circle_Encoder = 0;
float Cross_Ecoder   = 0;
float Ramp_Encoder   = 0;
float Obstacle_Encoder = 0;
float S_V_Reduce = 30;
int Zebra_STOP_STOP = 1;

void  DecelerateToTargetSpeed(void)
{
    if(Zebra_Stop_Flag && Zebra_Encoder > 400&& Zebra_STOP_STOP)
    {
          Vehicle_STOP();
    }
    if(Single_SideBridge_Flag){
           //识别到单边桥先急停再加到慢速去跑
           if(STOP_FLAG == 0){
               if (speed_Temp > Single_SideBridge_Velocity) {
                   speed_Temp -= S_V_Reduce;//30
                   //速度限幅
                   if(speed_Temp <= Single_SideBridge_Velocity)
                  {
                       speed_Temp = Single_SideBridge_Velocity;
                   }

                } else {
                    speed_Temp = Single_SideBridge_Velocity;
                    //减速完成标志位
                    STOP_FLAG = 1;
                    Servo_Control_Flag=0;
                    //Turn_Flag=0;  //转向换关必
                    //Servo_Control_Flag=0;

                    }
          }

}

}
//void AccelerateToTargetSpeed(void)
//{
//
////    if(Run_Flag){//发车缓慢加速
////      if(Target_Speed < Target_Speed_Temp){
////        // 加速过程中逐步增加速度
////        Target_Speed += SPEED_INCREMENT;
////        // 防止速度超过直线行驶的最大速度
////        if (Target_Speed > Target_Speed_Temp){
////            Target_Speed = Target_Speed_Temp;
////        }
////      }
////    }
//
//    // 直线加速
//    if(ImageStatus.straight_acc) {
//        if(speed_Temp < StraightLine_Velocity){
//            // 加速过程中逐步增加速度
//            speed_Temp += 10;
//            // 防止速度超过直线行驶的最大速度
//            if (speed_Temp > StraightLine_Velocity){
//                speed_Temp = StraightLine_Velocity;
//            }
//        }
//    }else{
//        speed_Temp = 400;
//    }
//
//}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度环中断执行函数
// 参数说明     float target           float actual_value
// 返回参数     浮点型
// 使用示例     Gyro_PIDVelocity ( &pidJv, float target, float actual_value)
// 备注信息     陀螺仪转向补偿
//-------------------------------------------------------------------------------------------------------------------
int Run_Flag =0;
int duty_YAW=0;
float Encoder_pre=0;
float Encoder=0;
float speedout;
float angleout;
float lunOUT=0;
float singletempV=130;
uint8 Tumble_num = 0;
float Left_OUT_Finally = 0, Right_OUT_Finally = 0;//电机最后赋值操作
void callback(void)

{
  static int stop_num=0;
  static int angle_flag=0;
  static int speedring_flag=0;
  static int turn_FLAG=0;
  static int Servo_Flag = 0;
  speedring_flag++;    angle_flag++;  turn_FLAG++;   Servo_Flag++;

  if(speedring_flag%20==0)
  {
      if( Single_SideBridge_Flag )//Zebra_Stop_Flag ||
      {
//     //斑马线单边桥减速调整
        // Zebra_Encoder += (float)((motor_value.receive_left_speed_data + (-motor_value.receive_right_speed_data)) /2 / 10);
      //减速调整
         DecelerateToTargetSpeed();

       }
      else{
        if(Run_Flag  == 1)
        {
            Vehicle_START();
        }
      }

             if(STOP_FLAG == 1&& Single_SideBridge_Flag==1 ){
                 //单边桥距离积分用于跳出单边桥状态
               Obstacle_Encoder += (float)( (motor_value.receive_left_speed_data + (-motor_value.receive_right_speed_data))/2 / 10);
           }


             //***************************************速度环编码器速度获取*************************************************************************//
             //对编码器的值进行滤波
        Encoder=(float)((motor_value.receive_left_speed_data+(-motor_value.receive_right_speed_data))/2);
        //一阶低通滤波器
        Encoder_pre *= 0.7;
        //===一阶低通滤波器
        Encoder_pre += Encoder * 0.3;
        speedout=speed_Velocity(&pidv,speed_Temp,Encoder_pre ) ;


        if(JUMP_FLAG){    //跳跃屏蔽速度环
              speedout=0;
         }
              speedring_flag=0;
  }



//角度环作用位置
  if  (angle_flag%5==0)
  {
       Icm_Data();
       IMU660RA_Parameter.Pitch = g_attitude.pitch;
       IMU660RA_Parameter.Roll  = g_attitude.roll;
       angleout=Angle_Velocity (&pidJ, Machine_Mid+(speedout),IMU660RA_Parameter.Pitch );//0.5
       angle_flag=0;

  }
//角速度环作用位置



   imu660ra_get_gyro();

//   gyro_y_lpf = lpf_operator(&lpf_current_gyro_y,imu660ra_gyro_y,0.001);
//   gyro_z_lpf = lpf_operator(&lpf_current_gyro_z,imu660ra_gyro_z,0.001);
   lunOUT  = Gyro_PIDVelocity(&pidJV,angleout,imu660ra_gyro_y);

//转向环作用位置
         if (Turn_Flag){
         duty_YAW = (int16)Turn_PID(&Turn,LCDW / 2 - 1,(float)ImageStatus.Det_True);
         if (Single_SideBridge_Flag==1 &&Servo_Control_Flag==0  )//单边桥转向环
              {
                 duty_YAW= (int16)TurnZ_PID( &Turn1, LCDW / 2 - 1,(float)ImageStatus.Det_True);
              }

        }














//疯转保护
      if(Left_OUT_Finally >= 7000 || Right_OUT_Finally>= 7000){
          stop_num ++;
          if(stop_num > 2000){
              Run_Flag= 0;
              stop_num = 0;
          }
      }
      if(Left_OUT_Finally <= -7000 || Right_OUT_Finally  <= -7000){
          stop_num ++;
          if(stop_num > 2000){
              Run_Flag = 0;
              stop_num = 0;
          }
      }

      //摔倒保护（更加严格）
//      if( g_attitude.pitch < -30){
//         // Tumble_num ++;
////          if(Tumble_num > 30){
////              Run_Flag = 0;
////              Tumble_num = 0;
////          }
//          small_driver_set_duty(0 , 0);
//      }
//      if(g_attitude.pitch > 30){
////          Tumble_num ++;
////          if(Tumble_num > 30){
////              Run_Flag = 0;
////              Tumble_num = 0;
//
//          small_driver_set_duty(0 , 0);
//          }
//      }
//电机最终赋值
      Left_OUT_Finally  =-lunOUT+duty_YAW;
      Right_OUT_Finally = lunOUT+duty_YAW;
//发车标志位
      if(Run_Flag == 1)
                {
          if(Servo_Control_Flag){
////       //         //利用舵机改变重心控制平衡
                   if(Servo_Flag % 3 == 0){//舵机控制周期三毫秒
                       Servo_Control();
                       Servo_Flag = 0;
                   }
               }

                 small_driver_set_duty( Left_OUT_Finally,  Right_OUT_Finally);

                 }
                 else
                 {
                     small_driver_set_duty(0 ,0);
                 }


   }


