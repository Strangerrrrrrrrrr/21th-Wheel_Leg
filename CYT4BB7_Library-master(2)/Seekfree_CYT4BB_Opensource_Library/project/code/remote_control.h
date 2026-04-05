#ifndef _REMOTE_CONTROL_H_
#define _REMOTE_CONTROL_H_

#include "zf_common_headfile.h"
#include "balance_control.h"         
#include "zf_device_uart_receiver.h" 

// CH1: 物理方向轮 (控制左右转向)
#define CH1_MIN         691
#define CH1_CENTER      1096
#define CH1_MAX         1500
#define CH1_DEADZONE    30        

// CH2: 物理油门扳机 (控制前进后退)
#define CH2_MIN         608
#define CH2_CENTER      1003
#define CH2_MAX         1400
#define CH2_DEADZONE    30        

// CH5: 按键开关
#define CH5_THRESHOLD   1000      

// 运动目标幅值限幅
#define MAX_SPEED_TARGET  250.0f  
#define MAX_TURN_TARGET   500.0f  

extern float turn_out_global;
void remote_control_process(void);

#endif