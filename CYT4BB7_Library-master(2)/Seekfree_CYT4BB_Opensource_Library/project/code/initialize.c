#include "initialize.h"

float Exposure = 550;
void all_init(void)

{
    ips200_set_dir(IPS200_CROSSWISE);                // 屏幕方向设置
    ips200_set_color(RGB565_RED , RGB565_YELLOW);    // 屏幕颜色设置
    ips200_set_font(IPS200_6X8_FONT);                // 屏幕字体设置
    ips200_init(IPS200_TYPE_SPI);
    imu660rb_init();
    key_init(5);
    small_driver_uart_init();
    interrupt_global_enable (1);
    time_init();
}



void time_init (void)
{


      pit_ms_init(PIT_CH0, 1);     
}



