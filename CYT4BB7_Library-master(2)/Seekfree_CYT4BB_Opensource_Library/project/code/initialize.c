#include "initialize.h"
#include "pure_nav.h" // 引入以获取变量

float Exposure = 550;

void all_init(void)
{
    Run_Flag = 0;
    servo_init();
    imu660rb_init();
    ips200_init(IPS200_TYPE_SPI);             
    ips200_set_dir(IPS200_CROSSWISE);         // 设置屏幕为横屏显示，视野更宽
    ips200_clear();                           // 清空屏幕为纯白底色
    flash_init();
    imu_calibration();                        // <---- 这个神仙函数必须要调用！
    key_init(10); 
   
    // ================== 绘制 UI 静态框架 ==================
    ips200_set_color(RGB565_RED, RGB565_WHITE); 
    ips200_show_string(10, 10, "--- 2D PURE NAV ---"); 
    
    ips200_set_color(RGB565_BLACK, RGB565_WHITE); 
    ips200_show_string(10, 40, "Status:");        
    ips200_show_string(10, 70, "Points:");        
    
    // 取消注释配合 main.c 里的标定工具
    // ips200_show_string(10, 100, "Curr X:"); 
    // ips200_show_string(10, 130, "Curr Y:"); 

    // 改为遥控器通道操作提示
    ips200_set_color(RGB565_BLUE, RGB565_WHITE);
    ips200_show_string(10, 160, "CH4: Record / Save");
    ips200_show_string(10, 190, "CH6: Auto Playback");
    // ======================================================

    small_driver_uart_init();
    time_init();
    uart_receiver_init(); // 串口接收（含遥控器SBUS解析等）
}


void time_init (void)
{
    pit_ms_init(PIT_CH0, 1);          // 1ms定时器（平衡控制核心心跳）
    pit_ms_init(PIT_CH1, 10);         // 10ms定时器（按键扫描）  
}


