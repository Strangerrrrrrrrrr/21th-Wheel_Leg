#ifndef CODE_SERVO_H_                                       
#define CODE_SERVO_H_                                       

#include "zf_common_headfile.h"                             
#include "balance_control.h"                                

// ================== 舵机基础硬件配置 ==================
#define steer1_pwm    (TCPWM_CH09_P05_0)                    // 左上舵机
#define steer1_fre    (300)                                 
#define steer1_dir    (1)                                   
#define steer1_center (4400)                                // 左上中位

#define steer2_pwm    (TCPWM_CH12_P05_3)                    // 右上舵机
#define steer2_fre    (300)                                 
#define steer2_dir    (-1)                                  
#define steer2_center (4700)                                // 右上中位

#define steer3_pwm    (TCPWM_CH11_P05_2)                    // 左下舵机
#define steer3_fre    (300)                                 
#define steer3_dir    (-1)                                  
#define steer3_center (4500)                                // 左下中位

#define steer4_pwm    (TCPWM_CH10_P05_1)                    // 右下舵机
#define steer4_fre    (300)                                 
#define steer4_dir    (1)                                   
#define steer4_center (4700)                                // 右下中位

// ================== 舵机控制结构体定义 ==================
typedef struct                                              
{                                                           
   pwm_channel_enum    pwm_pin;                             
   int16               control_frequency;                   
   int16               steer_dir;                           
   int16               center_num;                          
   int8                steer_state;                         
   int16               now_location;                        
} steer_control_struct;                                     

// ================== 外部全局变量声明 ==================
extern int16 error1;                                        
extern int steer_output_duty;                               
extern int16 jump_flag;                                     

// ================== 外部接口函数声明 ==================
void servo_init(void);                                      
void SERVO_PWM_SET(steer_control_struct *servo, int16 d);   
void SERVO_SMOOTH_UPDATE(steer_control_struct *s, int16 t); 
void CAR_SERVO_CONTROL(void);                               

#endif /* CODE_SERVO_H_ */