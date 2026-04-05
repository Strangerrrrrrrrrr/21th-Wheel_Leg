#include "balance_control.h"
#include "small_driver_uart_control.h" 
#include "remote_control.h"            
#include "pure_nav.h"  
#include "servo.h"     

IMU660RB_Parameter_t  IMU660RB_Parameter;

pid_v     pidv;   
float     speed_Temp = 0;         
float     speed_limit = 0;
float     speedring_KP = 6, speedring_KI = 0.29;

pid_J     pidJ;  
float     Angle_KP = 400, Angle_KD = 1;  
float     Machine_Mid = 0.0f;     
float     Angle_KP_Temp = 0;

pid_JV    pidJV;
float     Gyro_kP = 0.9f, GYRO_KD = 0;

pid_turnz Turn1;
float     TurnzZ_KP = 10.0f;    
float     TurnzZ_KP2 = 0.0f;
float     Turn_KDz2 =  0.0f;     

float speed_Velocity(pid_v* k, float target, float actual_value) {
   k->err1   = target - actual_value;
   k->sum    = k->sum + k->err1;
   k->sum    = LIMIT(k->sum, -6000, 6000);
   k->output = speedring_KP * k->err1 + speedring_KI * k->sum;
   return k->output;
}

float Angle_Velocity(pid_J* k, float target, float actual_value) {
    k->err1   = target - actual_value;
    k->use    = k->err1 - k->err2;
    k->err2   = k->err1;
    k->output = Angle_KP * (k->err1) + Angle_KD * (k->use);
    return k->output;
}

float Gyro_PIDVelocity(pid_JV* k, float target, float actual_value) {
    k->err1   = target - actual_value;
    k->use    = k->err1 - k->err2;
    k->err2   = k->err1;
    k->output = Gyro_kP * (k->err1) + GYRO_KD * (k->use);
    return k->output;
}

float TurnZ_PID(pid_turnz* k, float target, float actual_value) {
   k->err1 = target - actual_value;
   while (k->err1 >  180.0f) k->err1 -= 360.0f;
   while (k->err1 < -180.0f) k->err1 += 360.0f;
   k->output = TurnzZ_KP * k->err1 + TurnzZ_KP2 * (k->err1) * (ABS(k->err1)) + Turn_KDz2 * (0 - imu660rb_gyro_z);
   return k->output;
}

int   Run_Flag = 0;                   
float Encoder_pre = 0;                
float Encoder = 0;                    
float speedout = 0;                   
float angleout = 0;                   
float lunOUT = 0;                     
float Left_OUT_Finally = 0, Right_OUT_Finally = 0; 

void callback(void)
{
    static int speedring_flag = 0;
    static int angle_flag = 0;
    static int Servo_Flag = 0;
    
    // 【防打架：状态机与锁头标志位】
    static int yaw_lock_flag = 0;
    static float locked_yaw = 0.0f;

    speedring_flag++;  angle_flag++;  Servo_Flag++;   

    if(speedring_flag % 20 == 0)
    {
        Encoder = (float)((motor_value.receive_left_speed_data + (-motor_value.receive_right_speed_data)) / 2);
        Encoder_pre = (Encoder_pre * 0.7f) + (Encoder * 0.3f); 
        
        pure_nav_logic_tick(Encoder_pre, g_attitude.yaw);
        
        float current_target_speed = 0.0f;
        if (pure_nav_state == PURE_NAV_PLAYBACK) {
            current_target_speed = pure_nav_speed_out; 
        } else {
            current_target_speed = speed_Temp; 
        }

        speedout = speed_Velocity(&pidv, current_target_speed, Encoder_pre);
        speedring_flag = 0;
    }

    if (angle_flag % 5 == 0)
    {
        IMU660RB_Parameter.Pitch = g_attitude.pitch;
        // 【核心还原】：恢复你纯正的“重心诱导控制”逻辑！
        // 角度环的唯一任务就是维持平衡（目标只认机械零位 0 - Machine_Mid）。
        // speedout 不再参与这里，而是送到 servo.c 里去控制车腿摆动。
        angleout = Angle_Velocity(&pidJ, 0 - Machine_Mid, -IMU660RB_Parameter.Pitch);
        angle_flag = 0;
    }

    lunOUT = Gyro_PIDVelocity(&pidJV, angleout, imu660rb_gyro_y);

    // ---------------------------------------------------------
    // 4. 转向与航向锁定逻辑仲裁 (已彻底消灭打架)
    // ---------------------------------------------------------
    float turn_compensation = 0;

    if (pure_nav_state == PURE_NAV_PLAYBACK) 
    {
        turn_compensation = TurnZ_PID(&Turn1, pure_nav_yaw_out, g_attitude.yaw);
        yaw_lock_flag = 0; 
    }
    else if (pure_nav_state == PURE_NAV_RECORDING)
    {
        // 录制模式下，允许遥控打角，松开摇杆时输出 0（彻底放弃抵抗，方便你手动推车）
        if (ABS(turn_out_global) > 5.0f) {
            turn_compensation = turn_out_global; 
        } else {
            turn_compensation = 0; 
        }
        yaw_lock_flag = 0;
    }
    else 
    {
        // 正常遥控模式下
        if (ABS(turn_out_global) > 5.0f) {
            turn_compensation = turn_out_global; 
            yaw_lock_flag = 0;
        } else {
            // 重新介入锁头时，强制清除历史积分和微分误差，彻底消灭松手抽动！
            if (yaw_lock_flag == 0) {
                locked_yaw = g_attitude.yaw; 
                yaw_lock_flag = 1;
                Turn1.sum = 0.0f; 
                Turn1.err2 = 0.0f; 
                Turn1.err1 = 0.0f;
            }
            turn_compensation = TurnZ_PID(&Turn1, locked_yaw, g_attitude.yaw);
        }
    }

    float left_hardware_ratio = 0.85f; 
  
    Left_OUT_Finally  = (int16)( (lunOUT * left_hardware_ratio)  + turn_compensation);
    Right_OUT_Finally = (int16)( -lunOUT                         + turn_compensation); 
    
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