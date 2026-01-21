/*
 * Seek_Free_Assiatant.c
 *
 *  Created on: 2025年2月11日
 *      Author: 榴星宇
 */
#include "Seek_Free_Assistant.h"

void Seek_Free_Assistant_Init(void){//wifi_spi模块初始化
    wifi_spi_init("image","12345678");
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);

    //初始化通道数量
//    seekfree_assistant_oscilloscope_data.channel_num = 2;

//    seekfree_assistant_oscilloscope_data.channel_num = 4;

    seekfree_assistant_oscilloscope_data.channel_num = 5;
}

void Seek_Free_Assiatant(void){
    // 解析上位机发送过来的参数，解析后数据会存放在seekfree_assistant_oscilloscope_data数组中，可以通过在线调试的方式查看数据
    // 例程为了方便因此写在了主循环，实际使用中推荐放到周期中断等位置，需要确保函数能够及时的被调用，调用周期不超过20ms
    seekfree_assistant_data_analysis();
    // 遍历
    for(uint8_t i = 0; i < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; i++)
    {
        // 更新标志位
        if(seekfree_assistant_parameter_update_flag[i])
        {
            seekfree_assistant_parameter_update_flag[i] = 0;

            //逐飞助手通过各个通道发送数据
//            Velocity_KP = seekfree_assistant_parameter[0];//前面的数组数字代表通道号
//            Velocity_KI = seekfree_assistant_parameter[1];//前面的数组数字代表通道号

//            Angle_KP  = seekfree_assistant_parameter[0];//前面的数组数字代表通道号
//            Angle_KD  = seekfree_assistant_parameter[1];//前面的数组数字代表通道号


//            Gyro_KP  = seekfree_assistant_parameter[0];//前面的数组数字代表通道号
//            Gyro_KD  = seekfree_assistant_parameter[1];//前面的数组数字代表通道号
//
//            Turn_KP  = seekfree_assistant_parameter[0];
//            Turn_KP2 = seekfree_assistant_parameter[1];
//            Turn_KD  = seekfree_assistant_parameter[2];
//            Turn_KD2 = seekfree_assistant_parameter[3];
//            Target_Speed = seekfree_assistant_parameter[4];
//            Tow_Point = seekfree_assistant_parameter[5];

        }
        //对各个通道赋值相应变量

        //  seekfree_assistant_oscilloscope_data.data[0] = Target_Speed;   //前面的数组数字代表通道号
//          seekfree_assistant_oscilloscope_data.data[1] = motor_value.receive_left_speed_data;

//          seekfree_assistant_oscilloscope_data.data[0] = Machine_Mid;   //前面的数组数字代表通道号
//          seekfree_assistant_oscilloscope_data.data[1] = (Target_Angle_L) + IMU660RA_Parameter.Pitch;

//        seekfree_assistant_oscilloscope_data.data[0] = 0;   //前面的数组数字代表通道号
//        seekfree_assistant_oscilloscope_data.data[1] = (-Target_Gyro_L) + imu660ra_gyro_y;


        //seekfree_assistant_oscilloscope_data.data[0] = LCDW / 2 - 1;   //前面的数组数字代表通道号
       // seekfree_assistant_oscilloscope_data.data[1] = (uint8)ImageStatus.Det_True;

        //向逐飞助手发送数据
        seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
    }

        system_delay_ms(10);
    // 有可能会在逐飞助手软件上看到波形更新不够连续，这是因为使用WIFI有不确定的延迟导致的
}
