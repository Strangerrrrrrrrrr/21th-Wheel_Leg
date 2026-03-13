#include "motion_plan.h"

// 运动控制全局变量
motion_state_e current_motion = MOTION_IDLE;    
uint32_t motion_timer = 0;                      
float start_yaw = 0.0f;                         

// 【新增核心】：用于无限累加的当前目标偏航角
float current_target_yaw = 0.0f; 

float target_yaw_global = 0.0f;                 
float turn_out_global = 0.0f;                   

// ================== 偏航角闭环 PID 参数 ==================
float YAW_KP = 7.5f;   
float YAW_KD = 0.1f;   

// ================== 运动参数配置 ==================
#define TARGET_SPEED     250.0f      

// 如果转完往右偏（转少了），加大这个值（如 735.0f）
// 如果转完往左偏（转多了），减小这个值（如 705.0f）
#define ROTATE_ANGLE     720.0f     

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角闭环 PD 控制器
//-------------------------------------------------------------------------------------------------------------------
static float yaw_closed_loop_control(float target_yaw, float current_yaw)
{
    float err = target_yaw - current_yaw;
    float output = YAW_KP * err - YAW_KD * IMU_TRAN.gyroZ;
    
    // 限制转弯最大输出，宁可转慢点也绝不让轮子打滑，否则转完必偏！
    return LIMIT(output, -400, 400); 
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     运动规划初始化 (发车瞬间调用)
//-------------------------------------------------------------------------------------------------------------------
void motion_plan_init(void)
{
    current_motion = MOTION_FORWARD_5S; 
    motion_timer = 0;                   
    
    start_yaw = g_attitude.yaw;         
    
    // 初始化时，当前目标角度就是起跑方向
    current_target_yaw = start_yaw; 
    target_yaw_global = current_target_yaw;      
    
    speed_Temp = TARGET_SPEED;          
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     运动规划状态机 (放入 1ms 周期内调用)
//-------------------------------------------------------------------------------------------------------------------
void motion_plan_tick(void)
{
    static uint16 stable_cnt = 0; 

    // 1. 系统停止保护
    if (Run_Flag == 0) 
    {
        current_motion = MOTION_IDLE;
        turn_out_global = 0.0f;
        speed_Temp = 0.0f;
        stable_cnt = 0; 
        return;
    }
    
    // 2. 检测到发车，执行一次初始化
    if (current_motion == MOTION_IDLE && Run_Flag == 1)
    {
        motion_plan_init();
    }

    // 3. 状态机流转 (无限闭环版)
    switch (current_motion)
    {
        case MOTION_FORWARD_5S:
            speed_Temp = TARGET_SPEED;          
            target_yaw_global = current_target_yaw; // 死死锁住当前阶段应该走的方向
            
            motion_timer++;                     
            if (motion_timer >= 5000)           // 笔直往前跑满 5 秒
            {
                current_motion = MOTION_ROTATE_720; 
                motion_timer = 0;               
                stable_cnt = 0;                 
                
                // 【无限循环的核心秘籍】
                // 进入旋转状态的瞬间，直接把目标方向凭空加上 720 度！
                // 这样无论它转了多少次，下一次的目标永远是“当前方向+两圈”
                current_target_yaw += ROTATE_ANGLE; 
            }
            break;
            
        case MOTION_ROTATE_720:
            speed_Temp = 0.0f;                  
            target_yaw_global = current_target_yaw; // 目标变为加上了 720 度的最新方向
            
            motion_timer++;                     
            
            // 误差进入 5 度以内，开始累计稳定时间
            if (ABS(g_attitude.yaw - target_yaw_global) < 5.0f) 
            {
                stable_cnt++;
                // 强制在原地愣住 800ms 刹稳车身
                if (stable_cnt > 800) 
                {
                    // 【状态流转魔法】：转完后不进入闲置状态，而是重新切回前进状态！
                    current_motion = MOTION_FORWARD_5S; 
                    motion_timer = 0;
                    stable_cnt = 0;
                }
            }
            else
            {
                stable_cnt = 0; // 偏出去了就重新计时
            }
            
            // 超时保护：万一卡主转不动，挣扎 6 秒后强行突破，依然切回前进状态！
            if (motion_timer > 6000) 
            {
                current_motion = MOTION_FORWARD_5S; 
                motion_timer = 0;
                stable_cnt = 0;
            }
            break;
            
        case MOTION_FORWARD_AGAIN:
            // 原先的一次性收尾状态，现在被彻底废弃不用了！
            break;
            
        default:
            break;
    }

    // 4. 算出最终转向补偿力
    turn_out_global = yaw_closed_loop_control(target_yaw_global, g_attitude.yaw);
}