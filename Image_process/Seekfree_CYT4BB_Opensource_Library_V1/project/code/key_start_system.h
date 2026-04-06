#ifndef _KEY_START_SYSTEM_H_
#define _KEY_START_SYSTEM_H_

#include "zf_common_headfile.h"
#include "zf_device_key.h"
#include "balance_control.h"

// 按键配置
#define CONTROL_KEY_INDEX   KEY_1        // 控制按键
#define LONG_PRESS_TIME     2000         // 长按关机时间(ms)
#define TARGET_SPEED        150.0f       // 行进目标速度

// 系统状态
typedef enum {
    STATE_DOWN = 0,      // 倒下状态：电机关闭
    STATE_BALANCE,       // 原地平衡：站立但速度=0
    STATE_RUNNING        // 行进状态：匀速前进
} system_state_t;

// 函数声明
void key_start_system_init(void);        // 系统初始化
void key_state_machine_10ms(void);       // 按键状态机（10ms中断调用）
void balance_control_1ms(void);          // 平衡控制执行（1ms中断调用）
system_state_t get_system_state(void);   // 获取当前系统状态

#endif