#include "balance_control.h"
#include "small_driver_uart_control.h" // 确保引入了无刷电机的头文件以获取 motor_value
#include "remote_control.h"            // 确保引入了遥控器头文件以获取 turn_out_global

/*
 * balance_control.c
 *
 * Created on: 2026年2月9日
 * Author:猪小仙
 */
IMU660RB_Parameter_t  IMU660RB_Parameter;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度环控制器
// 参数说明     float target           float actual_value
// 返回参数     uint32
// 使用示例     speed_Velocity (&pidv, float target, float actual_value);
// 备注信息     speedring_KP =6 ,   speedring_KI = 0.28
//-------------------------------------------------------------------------------------------------------------------
pid_v  pidv;   //结构体
float speed_Temp=0;
float speed_limit=0;
float speedring_KP =6 ,  speedring_KI =0.29  ;
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
//-------------------------------------------------------------------------------------------------------------------
pid_J  pidJ;  //结构体
float Angle_KP =400,Angle_KD =1 ;  
float Machine_Mid =0.;
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
//-------------------------------------------------------------------------------------------------------------------
float Gyro_kP=0.9;
float GYRO_KD=0;
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
// 函数简介     转向环控制器 (遥控补偿)
//-------------------------------------------------------------------------------------------------------------------
pid_turn Turn;
float Turn_KP = 0  ,Turn_KP2 = 0,Turn_KD = 0,Turn_KD2 =0;    
float Turn_PID(pid_turn *k, float target, float actual_value)
{
   k->err1= target-actual_value;
   k->output=Turn_KP * k->err1+Turn_KP2 * (ABS(k->err1))+Turn_KD * (k->err1-k->err2)+Turn_KD2 * (-imu660ra_gyro_z);
   k->err2=k->err1;
   return k->output;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     转向环控制器 (单边桥运算)
//-------------------------------------------------------------------------------------------------------------------
float Single_SideBridge_Velocity =0;
pid_turnz Turn1;

// 【核心修改1】：为了对抗电机的物理安装方向，这里必须是负数！
float TurnzZ_KP = 12.0f;    
float TurnzZ_KP2 = 0.0f;
float Turn_KDz2 =  0.0f;     

float TurnZ_PID(pid_turnz *k, float target, float actual_value)
{
   k->err1 = target - actual_value;
   
   // 【核心修改2】：最短路径误差归一化，免疫 0-360 越界跳变，为后续 GPS 融合打下基础！
   while (k->err1 >  180.0f) k->err1 -= 360.0f;
   while (k->err1 < -180.0f) k->err1 += 360.0f;
   
   k->output = TurnzZ_KP * k->err1 + TurnzZ_KP2 * (k->err1) * (ABS(k->err1)) + Turn_KDz2 * (0 - imu660rb_gyro_z);
   return k->output;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     滚转角控制器
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

// =========================================================================================
// 函数简介     编码器直线纠偏 (同步) PID 控制器 (柔性防暴走版)
// =========================================================================================
float Sync_Kp = -5.0f;   // 比例系数 (改小初始值，温柔起步)
float Sync_Ki = 0.0f;  // 积分系数
float sync_out = 0;     
float encoder_sync_integral = 0; 
float filtered_diff = 0; // 引入低通滤波的差值

float Encoder_Sync_PID(float left_speed, float right_speed, float turn_cmd)
{
    // 1. 如果遥控器下发了转向指令，立刻交出控制权！
    if (turn_cmd != 0) 
    {
        encoder_sync_integral = 0; 
        filtered_diff = 0;
        return 0.0f;               
    }

    // 2. 计算左右轮的原始速度差值
    float raw_diff = left_speed + right_speed;

    // 3. 【防疯秘诀1】：一阶低通滤波
    // 过滤掉轮胎压到小坑或者瞬间打滑带来的数值毛刺，让误差变化非常平滑
    filtered_diff = (filtered_diff * 0.7f) + (raw_diff * 0.3f);

    // 4. 积分与限幅防死锁
    encoder_sync_integral += filtered_diff;
    encoder_sync_integral = LIMIT(encoder_sync_integral, -2000, 2000);

    // 5. PI 控制计算原始输出
    float out = Sync_Kp * filtered_diff + Sync_Ki * encoder_sync_integral;
    
    // 6. 【防疯秘诀2】：剥夺最高控制权 (绝对限幅)
    // 规定纠偏力最大只能是 ±300。这样哪怕 PID 算出来要补偿 2000，也只给 300。
    // 这保证了纠偏环永远只能“温柔地微调”，绝对撼动不了几千 PWM 的平衡大局！
    out = LIMIT(out, -1000, 1000);

    return out;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介      速度环中断执行函数
// 参数说明      定时器中断处调用
//-------------------------------------------------------------------------------------------------------------------
int Run_Flag =0;                    //发车标志位
int duty_YAW=0;                     //差速值
float Encoder_pre=0;                //编码器数值采集
float Encoder=0;                    //编码器当前数值采集
float speedout;                     //速度环输出
float angleout;                     //角度环输出 
float lunOUT=0;                     //轮子差速
float singletempV=0;                //单边桥行进速度
uint8 Tumble_num = 0;               
float Left_OUT_Finally = 0, Right_OUT_Finally = 0;//电机最后赋值操作

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      速度环中断执行函数 (每20ms/1ms核心心跳)
//-------------------------------------------------------------------------------------------------------------------
void callback(void)
{
  int16 systym=0;
  static int angle_flag=0;
  static int speedring_flag=0;
  static int turn_FLAG=0;
  static int Servo_Flag = 0;
  
  static float locked_yaw = 0.0f;
  static int yaw_lock_flag = 0;

  speedring_flag++;    angle_flag++;  turn_FLAG++;   Servo_Flag++; systym++;   

  if(speedring_flag%20==0){
        Encoder=(float)((motor_value.receive_left_speed_data+(-motor_value.receive_right_speed_data))/2);
        Encoder_pre *= 0.7;
        Encoder_pre += Encoder * 0.3;
        
        // 喂给导航系统数据
        nav_logic_tick(Encoder_pre, g_attitude.yaw);

        // 油门夺权
        if (current_nav_state == NAV_STATE_PLAYBACK) {
            speed_Temp = nav_target_speed;
        } else {
            speed_Temp = 0; 
        }

        speedout=speed_Velocity(&pidv,speed_Temp,Encoder_pre ) ;
        
        sync_out = Encoder_Sync_PID((float)motor_value.receive_left_speed_data, 
                                    (float)motor_value.receive_right_speed_data, 0); 
        speedring_flag=0;
  }

  if  (angle_flag%5==0)
  {
       IMU660RB_Parameter.Pitch = g_attitude.pitch;
       IMU660RB_Parameter.Roll  = g_attitude.roll;
       angleout=Angle_Velocity (&pidJ, 0-Machine_Mid,-IMU660RB_Parameter.Pitch );
       angle_flag=0;
  }

  lunOUT  = Gyro_PIDVelocity(&pidJV,angleout,imu660rb_gyro_y);

  // ==============================================================================
  // 【核心修改 3：方向盘夺权，偏航角绝对闭环！】
  float turn_compensation = 0;

  if (current_nav_state == NAV_STATE_PLAYBACK) 
  {
      turn_compensation = TurnZ_PID(&Turn1, nav_target_yaw, g_attitude.yaw);
      yaw_lock_flag = 0; 
  }
  else if (current_nav_state == NAV_STATE_RECORDING)
  {
      turn_compensation = 0;
      yaw_lock_flag = 0;
  }
  else 
  {
      if (yaw_lock_flag == 0) {
          locked_yaw = g_attitude.yaw; 
          yaw_lock_flag = 1;
      }
      turn_compensation = TurnZ_PID(&Turn1, locked_yaw, g_attitude.yaw);
  }

  float left_hardware_ratio = 0.85f; 
  
  // 【核心修改 4：彻底修正加减号，全部使用 + 接入转向力，打好配合】
  Left_OUT_Finally  = (int16)( (lunOUT * left_hardware_ratio)  + turn_compensation);
  Right_OUT_Finally = (int16)( -lunOUT                         + turn_compensation); 
  // ==============================================================================
    
  Left_OUT_Finally  = LIMIT(Left_OUT_Finally, -6000, 6000);
  Right_OUT_Finally = LIMIT(Right_OUT_Finally, -6000, 6000);
  
  if(Run_Flag)
  {
      small_driver_set_duty((int16)Left_OUT_Finally, (int16)Right_OUT_Finally);
      if(Servo_Flag % 5 == 0)
      {
            CAR_SERVO_CONTROL(); 
            Servo_Flag = 0;      
      }
  }
  else
  {
      small_driver_set_duty(0, 0);
  } 
}