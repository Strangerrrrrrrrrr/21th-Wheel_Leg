/*********************************************************************************************************************
* CYT4BB Opensourec Library 即（ CYT4BB 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 CYT4BB 开源库的一部分
*
* CYT4BB 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main_cm7_0
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 9.40.1
* 适用平台          CYT4BB
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2024-1-4       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
// **************************** 代码区域 ****************************


int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_init();                       // 调试串口信息初始化
    // 此处编写用户代码 例如外设初始化代码等
  
         all_init();
         int print_count = 0;
   // int print_count = 0; // 定义一个打印计数器
    
    // 此处编写用户代码 例如外设初始化代码等
    while(true)
    {
      // 此处编写需要循环执行的代码
      // param_ui_process();
     // printf("%f,%f,%f,%f,%f,%f\n",g_attitude.pitch,g_attitude.yaw,g_attitude.roll ,IMU_TRAN.gyroX,IMU_TRAN.gyroY,IMU_TRAN.gyroZ);
      // printf("%f,%d,%d,%f,%d,%f\n",speedout,steer_output_duty,error1,Encoder_pre,motor_value.receive_left_speed_data,IMU_TRAN.gyroZ);
       //small_driver_set_duty(500,500);
       //  system_delay_ms(10);
      // 循环执行遥控器任务
         //  if(mt9v03x_finish_flag)
        //   {
        //     ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
           
         //      mt9v03x_finish_flag=0;
        //   }
       //remote_control_process(); 
       
       if (key_get_state(KEY_1) == KEY_SHORT_PRESS) 
        {
            if (current_nav_state == NAV_STATE_IDLE && waypoint_count > 0)
            {
                Run_Flag = 1;                      
                nav_set_state(NAV_STATE_PLAYBACK); 
            }
            key_clear_state(KEY_1);                
        }

        if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            if (current_nav_state == NAV_STATE_IDLE)
            {
                Run_Flag = 0;                      
                nav_set_state(NAV_STATE_RECORDING);
            }
            key_clear_state(KEY_2);                
        }

        if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            if (current_nav_state == NAV_STATE_RECORDING || current_nav_state == NAV_STATE_PLAYBACK)
            {
                Run_Flag = 0;                      
                nav_set_state(NAV_STATE_IDLE);     // 会自动触发 Flash 保存
            }
            key_clear_state(KEY_3);                
        }

        // ======================= 屏幕 UI 刷新 =======================
        if (current_nav_state == NAV_STATE_IDLE) {
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
            ips200_show_string(80, 40, "IDLE    "); 
        } else if (current_nav_state == NAV_STATE_RECORDING) {
            ips200_set_color(RGB565_RED, RGB565_WHITE); 
            ips200_show_string(80, 40, "RECORD  ");
        } else if (current_nav_state == NAV_STATE_PLAYBACK) {
            ips200_set_color(RGB565_GREEN, RGB565_WHITE); 
            ips200_show_string(80, 40, "PLAYBACK");
        }

        ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        ips200_show_uint(80, 70, waypoint_count, 4);

        // ======================= 5cm 里程标定监测打印 =======================
        print_count++;
        if(print_count >= 500000) // 减速打印，根据你的主频可以自行调大调小
        {
            print_count = 0;
            // 看着这个数字，推 5 厘米，看数字增加了多少，然后填入 navigation.h
            printf("当前里程计数值: %f\r\n", total_accumulated_distance);
        }
    }
      
      
      
        // 此处编写需要循环执行的代码
    


// **************************** 代码区域 ****************************
}