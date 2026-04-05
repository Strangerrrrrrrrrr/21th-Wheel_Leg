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
         pure_nav_init();
         
         int display_count = 0;          // 屏幕刷新降频计数器
      //   int print_count = 0;
   // int print_count = 0; // 定义一个打印计数器
    
    // 此处编写用户代码 例如外设初始化代码等
    while(true)
    {
      // 此处编写需要循环执行的代码
    // ---------------------------------------------------------
        // 1. 遥控器接收与状态机处理 (大脑中枢，必须不断循环)
        // ---------------------------------------------------------
        remote_control_process(); 
       
        // ---------------------------------------------------------
        // 2. 屏幕动态数据刷新 (降频执行，防止耗尽 CPU 时间阻碍控制)
        // ---------------------------------------------------------
        display_count++;
        if(display_count >= 1000) // 这个值可以根据屏幕闪烁程度自行改大或改小
        {
            display_count = 0;
            
            // 刷新当前导航状态
            if (pure_nav_state == PURE_NAV_IDLE) {
                ips200_set_color(RGB565_BLACK, RGB565_WHITE);
                ips200_show_string(80, 40, "IDLE    "); 
            } else if (pure_nav_state == PURE_NAV_RECORDING) {
                ips200_set_color(RGB565_RED, RGB565_WHITE); 
                ips200_show_string(80, 40, "RECORD  ");
            } else if (pure_nav_state == PURE_NAV_PLAYBACK) {
                ips200_set_color(RGB565_GREEN, RGB565_WHITE); 
                ips200_show_string(80, 40, "PLAYBACK");
            }

            // 刷新已记录的点位数量
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
            ips200_show_uint(80, 70, pure_waypoint_count, 4);

            // ========================================================
            // 【里程标定小工具】：如果你想标定 5cm 到底对应多少脉冲，
            // 可以取消下面两行的注释，看着屏幕推车记录 pure_current_x 的变化
            // ========================================================
            // ips200_show_float(80, 100, pure_current_x, 4, 1);
            // ips200_show_float(80, 130, pure_current_y, 4, 1);
        }
    }
    
    }
      
      
      
        // 此处编写需要循环执行的代码
    


// **************************** 代码区域 ****************************
