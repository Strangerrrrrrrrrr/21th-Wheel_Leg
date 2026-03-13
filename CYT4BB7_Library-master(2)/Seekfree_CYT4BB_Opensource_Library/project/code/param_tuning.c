#include "param_tuning.h"                                       // 包含刚刚建立的调参头文件

static uint8 save_msg_timer = 0;                                // 静态变量，用于控制屏幕“保存成功”提示弹窗的显示倒计时

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     参数初始化与读取 (开机自动执行)
// 备注信息     读取 Flash，如果检测到之前存过参数，就覆盖代码里的默认值；如果是第一次烧录，则存入默认值。
//-------------------------------------------------------------------------------------------------------------------
void param_init_and_load(void)                                  
{                                                               
    flash_init();                                               // 初始化 Flash 底层驱动
    
    // 把 Flash 第 90 页的数据完整读取到全局联合体缓冲区 flash_union_buffer 中
    flash_read_page_to_buffer(0, PARAM_FLASH_PAGE, FLASH_PAGE_LENGTH);
    
    // 判断缓冲区第 0 个元素是不是我们设定的“魔术字”(0xA5A5A5A5)
    if(flash_union_buffer[0].uint32_type == PARAM_MAGIC_NUM)    
    {                                                           
        // 如果是，说明之前用按键保存过！读取第 1 个元素的值，赋给机械中值
        Machine_Mid = flash_union_buffer[1].float_type;         
    }                                                           
    else                                                        
    {                                                           
        // 如果不是，说明这是您第一次烧录程序，Flash里全是乱码或者0xFF
        // 此时主动执行一次保存，把代码中当前的 Machine_Mid 写入 Flash 建立基准
        param_save_to_flash();                                  
    }                                                           
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将当前参数固化保存至 Flash (掉电不丢失)
//-------------------------------------------------------------------------------------------------------------------
void param_save_to_flash(void)                                  
{                                                               
    flash_buffer_clear();                                       // 写入前，先清空 Flash 数据缓冲区，防止残留脏数据
    
    // 组装数据：
    flash_union_buffer[0].uint32_type = PARAM_MAGIC_NUM;        // 第 0 位存入魔术字，作为“此页有有效数据”的烙印标记
    flash_union_buffer[1].float_type  = Machine_Mid;            // 第 1 位存入我们刚刚用按键调好的机械中值
    
    // 写入前必须先擦除该页
    if(flash_check(0, PARAM_FLASH_PAGE))                        // 检查第 90 页是否为空
    {                                                           
        flash_erase_page(0, PARAM_FLASH_PAGE);                  // 如果不为空，则执行擦除操作
    }                                                           
    
    // 真正执行写入：把组装好的缓冲区数据烧录进 Flash 第 90 页
    flash_write_page_from_buffer(0, PARAM_FLASH_PAGE, FLASH_PAGE_LENGTH);
    
    // 设置屏幕提示显示时间：主循环通常是10ms跑一次，设置100次就是让提示显示 1 秒钟
    save_msg_timer = 100;                                       
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     脱机调参 UI 界面与按键交互逻辑
// 备注信息     放入 main.c 的 while(true) 循环中，非阻塞持续运行
//-------------------------------------------------------------------------------------------------------------------
void param_ui_process(void)                                     
{                                                               
    // ================== 1. 静态字符 UI 显示 ==================
    // (x坐标, y坐标, 字符串内容)
    ips200_show_string(0, 0,  "--- Offline Tuning ---");        // 显示标题
    ips200_show_string(0, 30, "KEY_2 : Pitch UP   (+)");        // 提示按键2功能
    ips200_show_string(0, 50, "KEY_3 : Pitch DOWN (-)");        // 提示按键3功能
    ips200_show_string(0, 70, "KEY_4 : SAVE To FLASH!");        // 提示按键4功能
    
    ips200_show_string(0, 110, "Machine_Mid:");                 // 显示变量名称

    // ================== 2. 动态参数显示 ==================
    // 实时刷新机械中值 (参数说明：x坐标, y坐标, 浮点变量, 整数显示位数, 小数显示位数)
    // IPS200 底层库会自动处理正负号显示
    ips200_show_float(100, 110, Machine_Mid, 2, 2);             

    // ================== 3. 按键扫描与逻辑处理 ==================
    // 说明：逐飞的按键底层会在后台自动扫描并保存状态，我们只需直接读取状态即可
    
    // 处理 KEY_2 按下：增加机械中值
    if(key_get_state(KEY_2) == KEY_SHORT_PRESS)                 
    {                                                           
        Machine_Mid += MID_STEP;                                // 机械中值增加 0.1
        key_clear_state(KEY_2);                                 // 【极重要】消耗掉该按键的按下状态，防止主循环极速刷新导致连加
    }                                                           
    
    // 处理 KEY_3 按下：减小机械中值
    if(key_get_state(KEY_3) == KEY_SHORT_PRESS)                 
    {                                                           
        Machine_Mid -= MID_STEP;                                // 机械中值减小 0.1
        key_clear_state(KEY_3);                                 // 消耗掉按键状态
    }                                                           
    
    // 处理 KEY_4 按下：触发固化保存
    if(key_get_state(KEY_4) == KEY_SHORT_PRESS)                 
    {                                                           
        param_save_to_flash();                                  // 调用保存函数，将当前数值烧录进Flash
        key_clear_state(KEY_4);                                 // 消耗掉按键状态
    }                                                           

    // ================== 4. "保存成功" 弹窗提示逻辑 ==================
    if(save_msg_timer > 0)                                      // 如果倒计时没结束
    {                                                           
        ips200_show_string(10, 150, ">> Param SAVED! <<");      // 在屏幕下方显示保存成功提示
        save_msg_timer--;                                       // 倒计时递减
    }                                                           
    else                                                        
    {                                                           
        ips200_show_string(10, 150, "                  ");      // 倒计时结束，用空格将提示字迹抹除
    }                                                           
}