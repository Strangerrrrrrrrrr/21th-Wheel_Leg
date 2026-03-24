#include "initialize.h"

float Exposure = 550;
void all_init(void)

{
    // Run_Flag = 0;
    servo_init();
    imu660rb_init();
    ips200_init(IPS200_TYPE_SPI);             
    ips200_set_dir(IPS200_CROSSWISE);         // 设置屏幕为横屏显示，视野更宽
    ips200_clear();                           // 清空屏幕为纯白底色
    // ips200pro_init("测试", IPS200PRO_TITLE_BOTTOM, 30);
    //  param_init_and_load();
    // printf("Please don't touch the car! Calibrating...\r\n");
    imu_calibration(); // <---- 这个神仙函数必须要调用！
    // printf("Calibration Done!\r\n");
    key_init(10); 
    flash_init();
    nav_init();
    // 绘制 UI 静态框架
    ips200_set_color(RGB565_RED, RGB565_WHITE); 
    ips200_show_string(10, 10, "--- SEEKFREE NAV ---"); 
    
    ips200_set_color(RGB565_BLACK, RGB565_WHITE); 
    ips200_show_string(10, 40, "Status:");        
    ips200_show_string(10, 70, "Points:");        
    
    ips200_set_color(RGB565_BLUE, RGB565_WHITE);
    ips200_show_string(10, 130, "KEY1: Start Auto-Play");
    ips200_show_string(10, 160, "KEY2: Start Recording");
    ips200_show_string(10, 190, "KEY3: Stop & Save");
    //mt9v03x_init();
    small_driver_uart_init();
    time_init();
     //  uart_receiver_init();//串口接收



}



void time_init (void)
{
       pit_ms_init(PIT_CH0, 1);          // 1ms定时器（平衡控制）
       pit_ms_init(PIT_CH1, 10);         // 10ms定时器（按键扫描）  
}



