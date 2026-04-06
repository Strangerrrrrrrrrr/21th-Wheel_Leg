/*
*
*       Create on :2026/3/24
*       author: ii
*
*/

#include "init.h"
float Exposure = 550;

void all_init()
{
    ips200_set_dir(IPS200_CROSSWISE);  //设置屏幕显示方向
    ips200_set_color(RGB565_RED, RGB565_YELLOW);  //设置屏幕颜色
    ips200_set_font(IPS200_6X8_FONT);  //设置屏幕字体
    ips200_init(IPS200_TYPE_SPI);

    mt9v03x_init();
    paraflash_init(); //参数保存（exposure）初始化 
    mt9v03x_set_exposure_time(Exposure);


    interrupt_global_enable(1);  //开启全局中断,触发图像回调
    time_init();
}

void time_init()
{
    pit_ms_init(PIT_CH0, 1);  // Example: Set PIT channel 0 to trigger every 1000 ms (1 second)
    pit_ms_init(PIT_CH1,1);   // Example: Set PIT channel 1 to trigger every 500 ms (0.5 second)
    pit_init(PIT_CH10, 1000);      // Example: Set PIT channel 2 to trigger every 10000 us (10 ms)  
    pit_init(PIT_CH11, 5000);     // Example: Set PIT channel 10 to trigger every 10000 us (10 ms)
}

