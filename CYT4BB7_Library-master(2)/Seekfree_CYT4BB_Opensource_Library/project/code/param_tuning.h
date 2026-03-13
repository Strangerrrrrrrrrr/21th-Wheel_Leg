#ifndef _PARAM_TUNING_H_                                        // 防止头文件重复包含
#define _PARAM_TUNING_H_

#include "zf_common_headfile.h"                                 // 包含逐飞科技底层硬件驱动头文件
#include "balance_control.h"                                    // 包含平衡控制头文件

// ================== Flash 存储配置 ==================
#define PARAM_FLASH_PAGE    90                                  // 指定使用 Flash 的第 90 页来存储参数
#define PARAM_MAGIC_NUM     0xA5A5A5A5                          // 定义一个魔术字（识别码），用于判断 Flash 中是否存过有效参数

// ================== 按键调节步长配置 ==================
#define MID_STEP            0.1f                                // 每次短按按键，机械中值的增减步长

// ================== 外部变量声明 ==================
extern float Machine_Mid;                                       // 声明在 balance_control 中定义的机械中值变量

// ================== 外部接口声明 ==================
void param_init_and_load(void);                                 // 声明初始化与加载函数（开机时调用）
void param_save_to_flash(void);                                 // 声明保存参数到 Flash 函数
void param_ui_process(void);                                    // 声明屏幕显示与按键交互核心任务（放入主循环）

#endif /* _PARAM_TUNING_H_ */