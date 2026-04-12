/*
 * ALL.C
 *
 *  Created on: 2025年4月2日
 *      Author: 榴星宇
 */

#include "ALL.h"
float Exposure = 550;
void all_init(void)

{
    ips200_set_dir(IPS200_CROSSWISE);                // 屏幕方向设置
    ips200_set_color(RGB565_RED , RGB565_YELLOW);    // 屏幕颜色设置
    ips200_set_font(IPS200_6X8_FONT);                // 屏幕字体设置
    ips200_init(IPS200_TYPE_SPI);
    mt9v03x_init();
    //参数保存初始化
    paraflash_init();
    mt9v03x_set_exposure_time(Exposure);
//    //按键初始化
    button_init();
    imu660ra_init();
    key_init(5);
    servo_init();
    //servo_duty(90);
    lpf_init();
    small_driver_uart_init();
    Buzzer_Init();
    interrupt_global_enable (1);
    time_init();

   // Seek_Free_Assistant_Init();
}



void time_init (void)
{


    pit_ms_init(CCU60_CH0, 1);
    pit_ms_init(CCU60_CH1, 1);
    pit_init(CCU61_CH0,1000);
    pit_init(CCU61_CH1,5000);
}



