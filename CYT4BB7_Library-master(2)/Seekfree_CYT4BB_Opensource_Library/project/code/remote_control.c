#include "remote_control.h"

// 记录 CH5 按钮上一次的状态
// 初始化为 -1，作为“系统刚上电，还未进行首帧标定”的安全锁标志，防止上电爆冲
static int last_ch5_state = -1; 
float turn_out_global = 0;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     遥控器摇杆线性映射函数 (带死区处理)
// 参数说明     raw_val: 遥控器原始值 | min/center/max: 标定参数 | deadzone: 死区范围
// 返回参数     float: -1.0 到 1.0 的比例系数
//-------------------------------------------------------------------------------------------------------------------
static float map_rc_channel(int raw_val, int min_val, int center_val, int max_val, int deadzone) 
{
    // 1. 极限保护：限制原始数据在最大最小值之间，防止算出绝对值大于 1.0 的超调系数
    if (raw_val > max_val) raw_val = max_val;
    if (raw_val < min_val) raw_val = min_val;

    // 2. 死区判断：摇杆在中位附近的死区内，直接输出 0.0，彻底消除电位器公差导致的溜车
    if (raw_val > (center_val - deadzone) && raw_val < (center_val + deadzone)) 
    {
        return 0.0f; 
    }

    // 3. 归一化映射：将数值转换为 -1.0 ~ 1.0 的比例
    if (raw_val >= center_val + deadzone) 
    {
        // 正向映射 (输出 0.0 ~ 1.0)
        return (float)(raw_val - (center_val + deadzone)) / (float)(max_val - (center_val + deadzone));
    } 
    else 
    {
        // 负向映射 (输出 -1.0 ~ 0.0)
        return (float)(raw_val - (center_val - deadzone)) / (float)((center_val - deadzone) - min_val);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     遥控器主处理任务 (需在 main.c 的 while(1) 中不断调用)
//-------------------------------------------------------------------------------------------------------------------
void remote_control_process(void)
{
    // 1. 判断底层是否拼接好了一帧完整的遥控器串口数据
    if (1 == uart_receiver.finsh_flag) 
    {
        // 2. 检查失控保护：接收机是否与遥控器保持有效连接 (1 为正常，0 为失控)
        if (1 == uart_receiver.state) 
        {
            // === A. 获取当前帧的通道原始数据 ===
            int ch1_raw = uart_receiver.channel[0]; // CH1 (物理方向轮 -> 管转向)
            int ch2_raw = uart_receiver.channel[1]; // CH2 (物理油门扳机 -> 管前后)
            int ch5_raw = uart_receiver.channel[4]; // CH5 (使能开关)
            
            // === B. CH5 启停逻辑 (带上电防爆冲安全锁) ===
            // 判断当前物理状态：大于阈值(1000)视为按下(1)，否则视为松开(0)
            int current_ch5_state = (ch5_raw > CH5_THRESHOLD) ? 1 : 0; 
            
            // 【安全锁拦截】：如果是开机后收到的第一帧数据
            if (last_ch5_state == -1) 
            {
                // 只悄悄记录当前物理状态作为基准，绝对不触发启动动作！
                last_ch5_state = current_ch5_state; 
            }
            else 
            {
                // 边沿检测：当前是按下(1) 且 上一次是松开(0) 时，才触发状态翻转
                if (current_ch5_state == 1 && last_ch5_state == 0) 
                {
                    Run_Flag = !Run_Flag; // 翻转小车使能标志
                    
                    if (Run_Flag) {
                        printf("车子已使能: 目标速度已解锁！\r\n");
                    } else {
                        printf("车子已失能: 停机倾倒！\r\n");
                    }
                }
                // 同步更新历史状态
                last_ch5_state = current_ch5_state; 
            }
            
            // === C. 摇杆运动映射逻辑 (含丝滑算法) ===
            if (Run_Flag == 1) 
            {
                // 1. 获取基础线性比例系数 (-1.0 到 1.0)
                float forward_ratio = map_rc_channel(ch2_raw, CH2_MIN, CH2_CENTER, CH2_MAX, CH2_DEADZONE);
                float turn_ratio    = map_rc_channel(ch1_raw, CH1_MIN, CH1_CENTER, CH1_MAX, CH1_DEADZONE);
                
                // 2. 引入非线性摇杆曲线 (三次函数 x^3)
                // 让摇杆在中间区域的变化非常平缓，消除暴躁感
                forward_ratio = forward_ratio * forward_ratio * forward_ratio;
                
                // 3. 计算物理目标期望值
                float target_speed = forward_ratio * MAX_SPEED_TARGET;  
                float target_turn  = turn_ratio * MAX_TURN_TARGET;      
                
                // 4. 一阶低通滤波 (惯性缓启动 / 软刹车)
                // 过滤掉手指抖动引起的瞬间跳变，使加减速如同真车一般柔和
                speed_Temp      = (speed_Temp * 0.90f) + (target_speed * 0.10f);
                turn_out_global = (turn_out_global * 0.70f) + (target_turn * 0.30f);      
            }
            else 
            {
                // 失能状态下，摇杆指令一律作废，目标绝对清零
                speed_Temp      = 0.0f;
                turn_out_global = 0.0f;
            }
        }
        else 
        {
            // === D. 遥控器失控保护触发 (信号丢失或遥控关机) ===
            Run_Flag        = 0;     // 强制断电趴窝
            speed_Temp      = 0.0f;  // 速度目标归零
            turn_out_global = 0.0f;  // 转向目标归零
        }

        // 3. 必须清零完成标志位，底层中断才能继续接收下一帧
        uart_receiver.finsh_flag = 0; 
    }
}