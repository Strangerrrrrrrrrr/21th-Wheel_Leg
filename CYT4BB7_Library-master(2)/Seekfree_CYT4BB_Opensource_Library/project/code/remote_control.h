#ifndef _REMOTE_CONTROL_H_
#define _REMOTE_CONTROL_H_

#include "zf_common_headfile.h"
#include "balance_control.h"         // 引入 Run_Flag 和 speed_Temp 等底盘控制变量
#include "zf_device_uart_receiver.h" // 引入遥控器接收机结构体

// ================== 遥控器通道标定参数 (基于实测) ==================
// CH1: 物理方向轮 (控制左右转向)
#define CH1_MIN         691
#define CH1_CENTER      1096
#define CH1_MAX         1500
#define CH1_DEADZONE    30        // 方向轮在中位 ±30 范围内，转向输出绝对为 0

// CH2: 物理油门扳机 (控制前进后退)
#define CH2_MIN         608
#define CH2_CENTER      1003
#define CH2_MAX         1400
#define CH2_DEADZONE    30        // 扳机在中位 ±30 范围内，目标输出绝对为 0

// CH5: 按键开关 (实测值为 192 和 1792)
#define CH5_THRESHOLD   1000      // 判定按键按下与松开的临界阈值

// ================== 运动目标幅值限幅 ==================
// 已经降低最大速度以获得更柔和的控制体验
#define MAX_SPEED_TARGET  250.0f  // 对应 speed_Temp 的最大绝对值
#define MAX_TURN_TARGET   500.0f  // 对应 turn_out_global 的最大绝对值 (可根据实际转向灵敏度微调)

// 全局转向输出变量声明 (由于在 motion_plan.c 中已定义，这里必须使用 extern)
extern float turn_out_global;

// 遥控器处理任务函数声明
void remote_control_process(void);

#endif