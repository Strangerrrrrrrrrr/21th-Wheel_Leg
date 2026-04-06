#include "balance_control.h"
/*
 * balance_control.c
 *
 *  Created on: 2026年2月9日
 *  Author:猪小仙
 */
IMU660RB_Parameter_t  IMU660RB_Parameter;
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
float speedring_KP =6 ,  speedring_KI =0.28  ;//  float speedring_KP =-0.035 ,  speedring_KI =-0.0018 ;//  
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
// 返回参数     浮点型`
// 使用示例     Angle_Velocity (&pidJ, float target, float actual_value);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
pid_J  pidJ;  //结构体
float Angle_KP =400,Angle_KD =0.1 ;  //float Angle_KP =400,Angle_KD =1 ;
float  Machine_Mid =-1;
float Angle_KP_Temp =0 ;

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
float Gyro_kP=0.9;
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
float Turn_KP = 0  ,Turn_KP2 = 0,Turn_KD = 0,Turn_KD2 =0;    
float Turn_PID(pid_turn *k, float target, float actual_value)
{
    //计算当前误差
   k->err1= target-actual_value;
   k->output=Turn_KP * k->err1+Turn_KP2 * (ABS(k->err1))+Turn_KD * (k->err1-k->err2)+Turn_KD2 * (-imu660ra_gyro_z);
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
float Single_SideBridge_Velocity =0;//单边桥行进速度
pid_turnz Turn1;
float TurnzZ_KP = 0  ,TurnzZ_KP2=0, Turn_KDz2 = 0;
float TurnZ_PID(pid_turnz *k, float target, float actual_value)
{
    //计算当前误差
   k->err1= target-actual_value;
   k->output=TurnzZ_KP * k->err1+TurnzZ_KP2*(k->err1)*(ABS(k->err1))+Turn_KDz2 * (0 - imu660rb_gyro_z);
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
float ROll_KP=0.0;    float ROll_KD= 0.0;    float ROll_KI=0.0;
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
// 函数简介      速度环中断执行函数
// 参数说明      float target           float actual_value
// 返回参数     浮点型
// 使用示例     void callback()；
// 备注信息    定时器中断处调用
//-------------------------------------------------------------------------------------------------------------------
int Run_Flag =0;                    //发车标志位
int duty_YAW=0;                     //差速值
float Encoder_pre=0;                //编码器数值采集
float Encoder=0;                    //编码器当前数值采集
float speedout;                     //速度环输出
float angleout;                     //角度环输出 
float lunOUT=0;                     //轮子差速
float singletempV=0;              //单边桥行进速度
uint8 Tumble_num = 0;               
float Left_OUT_Finally = 0, Right_OUT_Finally = 0;//电机最后赋值操作
void callback(void)

{
  int16 systym=0;
  static int angle_flag=0;
  static int speedring_flag=0;
  static int turn_FLAG=0;
  static int Servo_Flag = 0;
  speedring_flag++;    angle_flag++;  turn_FLAG++;   Servo_Flag++; systym++;   //标志位计数
  if(speedring_flag%20==0){
//***************************************速度环编码器速度获取*************************************************************************//
        //对编码器的值进行滤波
  Encoder=(float)((motor_value.receive_left_speed_data+(-motor_value.receive_right_speed_data))/2);
        //一阶低通滤波器
  Encoder_pre *= 0.7;
 //===一阶低通滤波器
  Encoder_pre += Encoder * 0.3;
  speedout=speed_Velocity(&pidv,speed_Temp,Encoder_pre ) ;
       
  speedring_flag=0;
  }
//角度环作用位置
  if  (angle_flag%5==0)
  {
       IMU660RB_Parameter.Pitch = g_attitude.pitch;
       IMU660RB_Parameter.Roll  = g_attitude.roll;
       angleout=Angle_Velocity (&pidJ, 0-Machine_Mid,-IMU660RB_Parameter.Pitch );//0.5
       angle_flag=0;
  }
//角速度环作用位置
      lunOUT  = Gyro_PIDVelocity(&pidJV,angleout,imu660rb_gyro_y);

    // Left_OUT_Finally  = (int16)lunOUT;
    // Right_OUT_Finally =  -(int16)lunOUT;
      Left_OUT_Finally  = (int16)((lunOUT*0.75) + turn_out_global);
      Right_OUT_Finally = (int16)(-lunOUT + turn_out_global); 
    // ==============================================================
    
    // 【新增】：执行运动状态机 (前进->转圈->前进)
      //motion_plan_tick();
      
        if(Run_Flag)
    {
        // 1. 无刷电机：响应极快，可以 1ms 更新一次
        small_driver_set_duty((int16)Left_OUT_Finally, (int16)Right_OUT_Finally);
        
        // 2. 舵机腿部补偿：使用分频器，每 5ms (200Hz) 结算并更新一次
       // 注意：这里借用了您前面定义的 Servo_Flag
      if(Servo_Flag % 5 == 0)
      {
            CAR_SERVO_CONTROL(); // 调用我们上一轮重新生成的平滑舵机核心
            Servo_Flag = 0;      // 防止变量溢出
      }
    }
    else
    {
        // 停机状态
        small_driver_set_duty(0, 0);
        // 如果需要停机时舵机也回到中立姿态，可以把 CAR_SERVO_CONTROL() 拿出来，
        // 或者单独调用 SERVO_SMOOTH_UPDATE 回归 center_num
    } 
      
   }


