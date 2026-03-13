#ifndef _MOTION_PLAN_H_
#define _MOTION_PLAN_H_

#include "zf_common_headfile.h"
#include "balance_control.h"
#include "quaternion.h"

// ================== 运动状态机枚举 ==================
typedef enum {
    MOTION_IDLE = 0,        // 停车待机状态
    MOTION_FORWARD_5S,      // 前进5秒阶段
    MOTION_ROTATE_720,      // 旋转两圈阶段
    MOTION_FORWARD_AGAIN    // 再次恢复直线前进阶段
} motion_state_e;

// ================== 外部变量声明 ==================
extern float turn_out_global;       // 最终输出给底盘的偏航补偿量
extern float target_yaw_global;     // 当前期望的全局偏航角

// ================== 函数声明 ==================
void motion_plan_init(void);        // 运动规划初始化
void motion_plan_tick(void);        // 运动规划周期任务 (需放在1ms定时器中调用)

#endif /* _MOTION_PLAN_H_ */