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
#include "segmentation.h"
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设

// **************************** 代码区域 ****************************

/*1.定义；2.地址；3.1核中数据数组同步
#pragma location = 0x2808'070c
__no_init uint8_t m7_1_data[MT9V03X_IMAGE_SIZE];//核间通信图像数据数组
__no_init volatile uint8_t PROCE_FLAG; //核间通信标志位
*/

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_init();                       // 调试串口信息初始化
    // 此处编写用户代码 例如外设初始化代码等
       servo_init();
       Run_Flag = 0;
       imu660rb_init();
      ips200_init(IPS200_TYPE_SPI);
      // ips200pro_init("测试", IPS200PRO_TITLE_BOTTOM, 30);
     //  param_init_and_load();
       printf("Please don't touch the car! Calibrating...\r\n");
       imu_calibration(); // <---- 这个神仙函数必须要调用！
       printf("Calibration Done!\r\n");
       pit_ms_init(PIT_CH0, 1);          // 1ms定时器（平衡控制）
       pit_ms_init(PIT_CH1, 10);         // 10ms定时器（按键扫描）
       key_init(10);
       mt9v03x_init();
       small_driver_uart_init();
       uart_receiver_init();


    // 此处编写用户代码 例如外设初始化代码等
    while(true)
    {
      system_delay_ms(100);  //主循环之前延时
      /*SCB_CleanInvalidateDCache_by_Addr(&m7_1_data, sizeof(m7_1_data)); //同步Dache 和RAM数据
      printf("m7_1_data:");
      for(int i=0;i<MT9V03X_IMAGE_SIZE;i++)    //将数据打印到串口
      {
        printf("%d ",m7_1_data[i]);
      }
      printf("\r\n");
      */

      // 此处编写需要循环执行的代码
      // param_ui_process();
      // printf("%f,%f,%f,%f,%f,%f\n",g_attitude.pitch,g_attitude.yaw,g_attitude.roll ,IMU_TRAN.gyroX,IMU_TRAN.gyroY,IMU_TRAN.gyroZ);
      // printf("%f,%d,%d,%f,%d,%f\n",speedout,steer_output_duty,error1,Encoder_pre,motor_value.receive_left_speed_data,IMU_TRAN.gyroZ);
       //small_driver_set_duty(500,500);
         //system_delay_ms(10);

/********************1核图像处理计算***********************************/
      // 循环执行遥控器任务
         if(mt9v03x_finish_flag == 1)
         {
            ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
            //memcpy(m7_1_data, mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
            //SCB_CleanDCache_by_Addr(m7_1_data, MT9V03X_IMAGE_SIZE);
          //camera_finish_callback(); 摄像头采集完成回调函书最后让mt9v03x_finish_flag置1，实际使用中请勿手动调用该函数
            //PROCE_FLAG = 0;
            //SCB_CleanInvalidateDCache_by_Addr(&PROCE_FLAG,sizeof(PROCE_FLAG))
         }
     //  remote_control_process(); 
       //system_delay_ms(10); // 跑你的原逻辑
      
      
      
      
      
      
        // 此处编写需要循环执行的代码
    }


// **************************** 代码区域 ****************************
}