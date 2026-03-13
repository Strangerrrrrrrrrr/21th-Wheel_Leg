#include "servo.h"                                          

steer_control_struct SERVO1, SERVO2, SERVO3, SERVO4;        

int16 jump_flag = 0;                                        
int16 error1 = 0;                                           
int steer_output_duty = 0;                                  

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机初始化函数
//-------------------------------------------------------------------------------------------------------------------
void servo_init(void)                                       
{                                                           
    SERVO1.pwm_pin = steer1_pwm;  SERVO1.control_frequency = steer1_fre;
    SERVO1.steer_dir = steer1_dir; SERVO1.center_num = steer1_center; SERVO1.steer_state = 1;
    SERVO1.now_location = steer1_center;
    
    SERVO2.pwm_pin = steer2_pwm;  SERVO2.control_frequency = steer2_fre;
    SERVO2.steer_dir = steer2_dir; SERVO2.center_num = steer2_center; SERVO2.steer_state = 1;
    SERVO2.now_location = steer2_center;
    
    SERVO3.pwm_pin = steer3_pwm;  SERVO3.control_frequency = steer3_fre;
    SERVO3.steer_dir = steer3_dir; SERVO3.center_num = steer3_center; SERVO3.steer_state = 1;
    SERVO3.now_location = steer3_center;
    
    SERVO4.pwm_pin = steer4_pwm;  SERVO4.control_frequency = steer4_fre;
    SERVO4.steer_dir = steer4_dir; SERVO4.center_num = steer4_center; SERVO4.steer_state = 1;
    SERVO4.now_location = steer4_center;
    
    pwm_init(SERVO1.pwm_pin, SERVO1.control_frequency, SERVO1.now_location); 
    pwm_init(SERVO2.pwm_pin, SERVO2.control_frequency, SERVO2.now_location);     
    pwm_init(SERVO3.pwm_pin, SERVO3.control_frequency, SERVO3.now_location); 
    pwm_init(SERVO4.pwm_pin, SERVO4.control_frequency, SERVO4.now_location); 
}                                                           

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机直接赋值函数 (不带平滑，紧急跳变用)
//-------------------------------------------------------------------------------------------------------------------
void SERVO_PWM_SET(steer_control_struct *servo_control, int16 duty) 
{                                                                   
    if(servo_control->steer_state)                                  
    {                                                               
        duty = LIMIT(duty, servo_control->center_num - 2000, servo_control->center_num + 2000); 
        servo_control->now_location = duty;                         
        pwm_set_duty(servo_control->pwm_pin, duty);                 
    }                                                               
}                                                                   

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机平滑更新算法（核心步进限制器）
//-------------------------------------------------------------------------------------------------------------------
void SERVO_SMOOTH_UPDATE(steer_control_struct *servo_control, int16 target_offset) 
{                                                                                  
    if(!servo_control->steer_state) return;                                        

    int16 target_pwm = servo_control->center_num + (target_offset * servo_control->steer_dir); 
    target_pwm = LIMIT(target_pwm, servo_control->center_num - 1500, servo_control->center_num + 1500); 
    
    int16 error = target_pwm - servo_control->now_location;                        
    
    // 【平滑度调节】此处数字代表每次允许的PWM最大脉宽变化，值越小腿动得越柔和
    int16 step = LIMIT(error, -200, 200);                                            
    
    servo_control->now_location += step;                                           
    pwm_set_duty(servo_control->pwm_pin, servo_control->now_location);             
}                                                                                  

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     【全新架构】小车车腿舵机联合控制核心任务 (纯速度环驱动打角版)
// 备注信息     废除腿长伸缩，废除角度环干扰。完全由速度环支配前后摆角来改变重心。
//-------------------------------------------------------------------------------------------------------------------
void CAR_SERVO_CONTROL(void)                                        
{                                                                   
    static float speed_to_swing_filter = 0;                         // 静态变量，用于对速度环信号进行低通滤波平滑             
    int16 steer_target_offset[4] = {0};                             // 四大舵机的最终目标偏置

    // 1. 防摔倒安全系数 (当车体严重倾斜时，强制减小打角幅度，防止腿部动作导致彻底翻车)
    float pitch_offset = (30.0f - LIMIT(ABS(IMU660RB_Parameter.Pitch + Machine_Mid), 0.0f, 30.0f)); 

    // 2. 速度环原始数据转换 
    // 将无刷电机发来的 speedout 进行缩小和限幅，防止打角幅度过激
    int raw_speed_duty = LIMIT((int)speedout / 10, -300, 300) * 6;   
    
    // 3. 施加防摔倒衰减
    steer_output_duty = (int16)((float)raw_speed_duty * pitch_offset * 0.033f); 
    
    // 4. 【极重度一阶低通滤波】
    // 将 95% 留给历史值，只有 5% 是新值。这会让腿部的打角动作像“深呼吸”一样缓慢且极具韧性。
    // 这完美隔离了底盘由于高速PID震荡带来的高频杂波，彻底避免“轮腿共振”！
    speed_to_swing_filter = (speed_to_swing_filter * 0.7f) + ((float)steer_output_duty * 0.3f); 

    // ================== 【并级控制核心】运动学混控矩阵 ==================
    
    // 【切断腿长伸缩】：不再使用上下舵机同向控制来改变车体高度，写死为 0！
    float length_comp = 0;                   
    
    // 【赋予纯摆角控制】：将平滑后的速度环数据，直接作为整条腿前倾或后仰的摆角量！
    // 这样速度环一有误差，车腿就会主动向前或向后“跨步”，改变重心，诱骗无刷底盘去追赶速度。
    float swing_comp = speed_to_swing_filter;                                           

    // 分配到物理舵机
    // 【摆角运动学】：上下舵机必须给定【相反符号】的偏置，才能让整条腿保持长度不变的前提下，产生前后摆动！
    
    // 结算左侧腿 (舵机1为左上，舵机3为左下)
    steer_target_offset[0] = (int16)(length_comp + swing_comp);     // 左上舵机: + 摆角补偿
    steer_target_offset[2] = (int16)(length_comp - swing_comp);     // 左下舵机: - 摆角补偿
    
    // 结算右侧腿 (舵机2为右上，舵机4为右下)
    steer_target_offset[1] = (int16)(length_comp + swing_comp);     // 右上舵机: + 摆角补偿
    steer_target_offset[3] = (int16)(length_comp - swing_comp);     // 右下舵机: - 摆角补偿
    // ====================================================================
     
    error1 = steer_target_offset[0];                                // 给主循环 printf 用作观察
    
    // 5. 执行机构调用区
    if (Run_Flag == 1)                                              
    {                                                               
        SERVO_SMOOTH_UPDATE(&SERVO1, steer_target_offset[0]);       
        SERVO_SMOOTH_UPDATE(&SERVO2, steer_target_offset[1]);       
        SERVO_SMOOTH_UPDATE(&SERVO3, steer_target_offset[2]);       
        SERVO_SMOOTH_UPDATE(&SERVO4, steer_target_offset[3]);       
    }                                                               
}