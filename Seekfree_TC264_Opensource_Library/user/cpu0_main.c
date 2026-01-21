/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
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
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.9.4
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设

//#define BOUNDARY_NUM            (MT9V03X_H * 3 / 2)
#define BOUNDARY_NUM            (LCDH)
//uint8 image_copy[MT9V03X_H][MT9V03X_W];
uint8 image_copy[LCDH][LCDW];
uint8 x1_boundary[LCDH], x2_boundary[LCDH], x3_boundary[LCDH];

uint8 xy_x1_boundary[BOUNDARY_NUM], xy_x2_boundary[BOUNDARY_NUM], xy_x3_boundary[BOUNDARY_NUM];
//  只有Y边界
uint8 xy_y1_boundary[BOUNDARY_NUM], xy_y2_boundary[BOUNDARY_NUM], xy_y3_boundary[BOUNDARY_NUM];
// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

//       seekfree_assistant_camera_information_config(
//               SEEKFREE_ASSISTANT_MT9V03X,
//               image_copy[0],
//               LCDW,
//               LCDH
//      );//发送二值化图像
//       seekfree_assistant_camera_information_config(
//               SEEKFREE_ASSISTANT_MT9V03X,
//               image_copy[0],
//               MT9V03X_W,
//               MT9V03X_H
//       );//发送摄像头原始图像

//        seekfree_assistant_camera_boundary_config(XY_BOUNDARY,
//           BOUNDARY_NUM,
//           xy_x1_boundary,
//           xy_x2_boundary,
//           xy_x3_boundary,
//           xy_y1_boundary,
//           xy_y2_boundary,
//           xy_y3_boundary
//           );//发送边线信息
//       seekfree_assistant_camera_boundary_config(X_BOUNDARY,
//               LCDH,
//               x1_boundary,
//               x2_boundary,
//               x3_boundary,
//               NULL,
//               NULL,
//               NULL);


    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
// 此处编写需要循环执行的代码
//        Seek_Free_Assiatant();
//********************************************************************************************************************************//
        printf("%d\n", imu660ra_gyro_x);
//                         for(int32 i = 0; i < 60; i++)
//                        {
//                           xy_x2_boundary[i] = (uint8)ImageDeal[i].Center;
//                           xy_y2_boundary[i] = (uint8)i;
//                        }
//
//                        for(int32 i = 0; i < 60; i++)
//                        {
//                           xy_x1_boundary[i] = (uint8)ImageDeal[i].LeftBorder;
//                           xy_y1_boundary[i] = (uint8)i;
//                        }
//                        for(int32 i = 0; i < 60; i++)
//                        {
//                            xy_x3_boundary[i] = (uint8)ImageDeal[i].RightBorder;
//                            xy_y3_boundary[i] = (uint8)i;
//                        }
//遍历左右边界2222
//
//        /*************************图传使用代码***************************/

//        //memcpy(image_copy[0],Pixel[0],80*60);                 //发送二值化后的图像
//        //memcpy(image_copy[0],Image_Use[0],80*60);
//        memcpy(image_copy[0],mt9v03x_image[0], MT9V03X_IMAGE_SIZE);//发送摄像头采集的原图像
//        seekfree_assistant_camera_send();
//
//        //********************************************************************************************************************************//
//              //此处编写需要循环执行的代码
//              //菜单使用代码//使用菜单代码的关键步骤1：在中断添加按键扫描 2：在while循环里添加下述代码

               menu_entry();display_entry();
//        if(Run_Flag == 0){
//            ips200_clear();
               ips200_show_int(0,200,wubianhang,3);
               ips200_show_string(260,0,"Encoder");
               ips200_show_int(260,10,motor_value.receive_left_speed_data,5);
               ips200_show_int(260,20,motor_value.receive_right_speed_data,5);

               ips200_show_string(260,30,"OUTPUT");
               ips200_show_int(260,40,duty_YAW,3);
               ips200_show_float(260,50,Left_OUT_Finally,5,1);
               ips200_show_float(260,60,Left_OUT_Finally,5,1);
//                ips200_show_float(260,70,Encoder_pre,3,1);
               ips200_show_string(260,80,"Pitch");
               ips200_show_float(260,90,IMU660RA_Parameter.Pitch,3,1);
//
               ips200_show_string(100,110,"Circle");
               ips200_show_int(100,120,ImageFlag.image_element_rings,2);
               ips200_show_int(100,130,ImageFlag.image_element_rings_flag,2);
               ips200_show_uint(100,140,Ring_Help_Flag,1);

               ips200_show_string(100,150,"LOST1");
               ips200_show_float(100,160,ImageStatus.Left_Line,2,1);
               ips200_show_float(100,170,ImageStatus.Right_Line,2,1);
               ips200_show_uint(100,180,ImageStatus.WhiteLine,2);
               ips200_show_string(100,190,"LOST2");
               ips200_show_uint(100,200,LeftBoundary_Error,3);
               ips200_show_uint(100,210,RightBoundary_Error,3);
               ips200_show_string(100,220,"OFFLine");
               ips200_show_uint(100,230,ImageStatus.OFFLine,2);
               ips200_show_string(140,110,"variance");
               ips200_show_uint(140,120,variance_acc ,3);

               ips200_show_string(140,130,"C_Ysite");
               ips200_show_uint(140,140,Left_RingsFlag_Point1_Ysite,2);
               ips200_show_uint(140,150,Right_RingsFlag_Point1_Ysite,2);
               ips200_show_string(140,160,"C_State");
               ips200_show_uint(140,170,Circle_State,1);
               ips200_show_float(140,180,g_attitude.yaw,3,1);
               ips200_show_float(140,190,IMU660RA_Parameter.Yaw,3,1);
               ips200_show_string(140,200,"Cross");
               ips200_show_uint(150,210,Left_ERR,3);
               ips200_show_uint(150,220,Right_ERR,3);

               ips200_show_string(190,110,"Zebra");
               ips200_show_uint(190,120,Zebra_Flag,1);
               ips200_show_uint(190,130,Zebra_Stop_Flag,1);
               ips200_show_uint(190,140,Zebra_Time,3);
               ips200_show_float(190,150,Zebra_Encoder,5,1);

               ips200_show_string(175,170,"Obstacle");
               ips200_show_uint(190,180,BIG_Obstacle_Flag,1);
               ips200_show_uint(190,190,BIG_Obstacle_Flag_Last,1);
               ips200_show_uint(190,200,Black_Dot,3);
               ips200_show_string(190,210,"JUMP");
               ips200_show_uint(190,220,JUMP_FLAG,1);

               ips200_show_string(230,110,"Bridge");
//                ips200_show_uint(230,90,IKParam.XLeft,2);
//                ips200_show_uint(230,100,IKParam.YLeft,2);
//                ips200_show_uint(230,110,IKParam.XRight,2);
//                ips200_show_uint(230,120,IKParam.YRight,2);

               ips200_show_uint(230,120,Single_SideBridge_Flag,2);
               ips200_show_uint(230,130,Single_Bridge_Left,2);
               ips200_show_uint(230,140,Single_Bridge_Right,2);

               ips200_show_string(230,150,"MAX_POS");     ips200_show_uint(285,190,TrackWidthChangeFlag,3);
               ips200_show_uint(230,160,White_COL_MAX1,2);ips200_show_uint(285,200,White_COL_Left1,2);
               ips200_show_uint(230,170,White_COL_POS1,2);ips200_show_uint(285,210,White_COL_Right1,2);
               ips200_show_uint(230,180,White_COL_MAX2,2);ips200_show_uint(285,220,White_COL_Left2,2);
               ips200_show_uint(230,190,White_COL_POS2,2);ips200_show_uint(285,230,White_COL_Right2,2);
               ips200_show_uint(230,200,length_max,2);
               ips200_show_string(230,210,"S_Encoder");
               ips200_show_float(230,220,Obstacle_Encoder,5,1);

               ips200_show_string(270,110,"Straight");
               ips200_show_uint(275,120,Straight_Left_Varance,3);
               ips200_show_uint(275,130,Straight_Right_Varance,3);

               ips200_show_uint(275,140,Straight_Left_Varance1,3);
               ips200_show_uint(275,150,Straight_Right_Varance1,3);

               ips200_show_uint(275,160,Left_Straight_Flag,1);
               ips200_show_uint(275,170,Right_Straight_Flag,1);
              //图像阈值显示
               ips200_show_uint(130,10,ThreShold_Value,3);
               ips200_show_float(130,20,compensation_value,3,1);
               ips200_show_uint(130,30,Thresold_MIN,3);
               ips200_show_uint(130,40,Thresold_MAX,3);
////
////           //平衡环参数显示
               ips200_show_uint(170,10,Servo_Control_Flag,1);
////             //转向环参数显示
               //目标速度显示
               ips200_show_float(200,40,speed_Temp,3,1);
               ips200_show_float(200,0,Turn_KP,3,2);
               ips200_show_float(200,10,Turn_KP2,3,2);
               ips200_show_float(200,20,Turn_KD,3,2);
               ips200_show_float(200,30,Turn_KD2,3,2);
////                //舵机辅助平衡参数显示
//                ips200_show_float(260,130,SPEED_TO_ANGLE_GAIN,3,2);
//                ips200_show_float(260,140,SERVO_ANGLE_INCREMENT,3,2);
//        }

//               ips200_show_int(0,150,wubianhang,3);
//               ips200_show_uint(0,200,Number,3);
//
//               ips200_show_int(0,80, ImageDeal[48].Wide,2);
//               ips200_show_int(0,90, ImageDeal[49].Wide,2);
//               ips200_show_int(0,100,ImageDeal[50].Wide,2);
//               ips200_show_int(0,110,ImageDeal[51].Wide,2);
//               ips200_show_int(0,120,ImageDeal[52].Wide,2);
//               ips200_show_int(0,130,ImageDeal[53].Wide,2);
//               ips200_show_int(0,140,ImageDeal[54].Wide,2);
//               ips200_show_int(0,150,Road_Width[CLIP_DATA(ImageStatus.OFFLine)],2);
//               ips200_show_int(0,160,Road_Width[CLIP_DATA(ImageStatus.OFFLine + 1)],2);
//               ips200_show_int(0,170,Road_Width[CLIP_DATA(ImageStatus.OFFLine + 2)],2);
//               ips200_show_int(0,180,Road_Width[CLIP_DATA(ImageStatus.OFFLine + 3)],2);
//               ips200_show_int(0,190,Road_Width[CLIP_DATA(ImageStatus.OFFLine + 4)],2);
//               ips200_show_int(0,150,ImageDeal[CLIP_DATA(ImageStatus.OFFLine)].Wide,2);
//               ips200_show_int(0,160,ImageDeal[CLIP_DATA(ImageStatus.OFFLine+1)].Wide,2);
//               ips200_show_int(0,170,ImageDeal[CLIP_DATA(ImageStatus.OFFLine+2)].Wide,2);
//               ips200_show_int(0,180,ImageDeal[CLIP_DATA(ImageStatus.OFFLine+3)].Wide,2);
//               ips200_show_int(0,190,ImageDeal[CLIP_DATA(ImageStatus.OFFLine+4)].Wide,2);

        if(Information_Flag){
//                ips200_show_uint(80,0,White_DOT1[20],2);
//                ips200_show_uint(80,10,White_DOT1[22],2);
//                ips200_show_uint(80,20,White_DOT1[24],2);
//                ips200_show_uint(80,30,White_DOT1[26],2);
//                ips200_show_uint(80,40,White_DOT1[28],2);
//                ips200_show_uint(80,50,White_DOT1[30],2);
//                ips200_show_uint(80,60,White_DOT1[32],2);
//                ips200_show_uint(80,70,White_DOT1[34],2);
//                ips200_show_uint(80,80,White_DOT1[36],2);
//                ips200_show_uint(80,90,White_DOT1[38],2);
//                ips200_show_uint(80,100,White_DOT1[40],2);
//                ips200_show_uint(80,110,White_DOT1[42],2);
//                ips200_show_uint(80,120,White_DOT1[44],2);
//                ips200_show_uint(80,130,White_DOT1[44],2);
//                ips200_show_uint(80,140,White_DOT1[46],2);
//                ips200_show_uint(80,150,White_DOT1[48],2);
//                ips200_show_uint(80,160,White_DOT1[50],2);
//                ips200_show_uint(80,170,White_DOT1[52],2);
//                ips200_show_uint(80,180,White_DOT1[54],2);
//                ips200_show_uint(80,190,White_DOT1[55],2);
//                ips200_show_uint(80,200,White_DOT1[56],2);
//                ips200_show_uint(80,210,White_DOT1[57],2);
//                ips200_show_uint(80,220,White_DOT1[58],2);
//                ips200_show_uint(80,230,White_DOT1[59],2);

//                ips200_show_uint(80,0,White_DOT2[20],2);
//                ips200_show_uint(80,10,White_DOT2[22],2);
//                ips200_show_uint(80,20,White_DOT2[24],2);
//                ips200_show_uint(80,30,White_DOT2[26],2);
//                ips200_show_uint(80,40,White_DOT2[28],2);
//                ips200_show_uint(80,50,White_DOT2[30],2);
//                ips200_show_uint(80,60,White_DOT2[32],2);
//                ips200_show_uint(80,70,White_DOT2[34],2);
//                ips200_show_uint(80,80,White_DOT2[36],2);
//                ips200_show_uint(80,90,White_DOT2[38],2);
//                ips200_show_uint(80,100,White_DOT2[40],2);
//                ips200_show_uint(80,110,White_DOT2[42],2);
//                ips200_show_uint(80,120,White_DOT2[44],2);
//                ips200_show_uint(80,130,White_DOT2[44],2);
//                ips200_show_uint(80,140,White_DOT2[46],2);
//                ips200_show_uint(80,150,White_DOT2[48],2);
//                ips200_show_uint(80,160,White_DOT2[50],2);
//                ips200_show_uint(80,170,White_DOT2[52],2);
//                ips200_show_uint(80,180,White_DOT2[54],2);
//                ips200_show_uint(80,190,White_DOT2[55],2);
//                ips200_show_uint(80,200,White_DOT2[56],2);
//                ips200_show_uint(80,210,White_DOT2[57],2);
//                ips200_show_uint(80,220,White_DOT2[58],2);
//                ips200_show_uint(80,230,White_DOT2[59],2);


            ips200_show_int(0,80, ImageDeal[0].Wide,2);
            ips200_show_int(0,90, ImageDeal[1].Wide,2);
            ips200_show_int(0,100,ImageDeal[2].Wide,2);
            ips200_show_int(0,110,ImageDeal[3].Wide,2);
            ips200_show_int(0,120,ImageDeal[4].Wide,2);
            ips200_show_int(0,130,ImageDeal[5].Wide,2);
            ips200_show_int(0,140,ImageDeal[6].Wide,2);
            ips200_show_int(0,150,ImageDeal[7].Wide,2);
            ips200_show_int(0,160,ImageDeal[8].Wide,2);
            ips200_show_int(0,170,ImageDeal[9].Wide,2);
            ips200_show_int(0,180,ImageDeal[10].Wide,2);
            ips200_show_int(0,190,ImageDeal[11].Wide,2);
            ips200_show_int(0,200,ImageDeal[12].Wide,2);
            ips200_show_int(0,210,ImageDeal[13].Wide,2);
            ips200_show_int(0,220,ImageDeal[14].Wide,2);
            ips200_show_int(0,230,ImageDeal[15].Wide,2);

            ips200_show_int(20,80, ImageDeal[16].Wide,2);
            ips200_show_int(20,90, ImageDeal[17].Wide,2);
            ips200_show_int(20,100,ImageDeal[18].Wide,2);
            ips200_show_int(20,110,ImageDeal[19].Wide,2);
            ips200_show_int(20,120,ImageDeal[20].Wide,2);
            ips200_show_int(20,130,ImageDeal[21].Wide,2);
            ips200_show_int(20,140,ImageDeal[22].Wide,2);
            ips200_show_int(20,150,ImageDeal[23].Wide,2);
            ips200_show_int(20,160,ImageDeal[24].Wide,2);
            ips200_show_int(20,170,ImageDeal[25].Wide,2);
            ips200_show_int(20,180,ImageDeal[26].Wide,2);
            ips200_show_int(20,190,ImageDeal[27].Wide,2);
            ips200_show_int(20,200,ImageDeal[28].Wide,2);
            ips200_show_int(20,210,ImageDeal[29].Wide,2);
            ips200_show_int(20,220,ImageDeal[30].Wide,2);
            ips200_show_int(20,230,ImageDeal[31].Wide,2);

            ips200_show_int(40,80, ImageDeal[32].Wide,2);
            ips200_show_int(40,90, ImageDeal[33].Wide,2);
            ips200_show_int(40,100,ImageDeal[34].Wide,2);
            ips200_show_int(40,110,ImageDeal[35].Wide,2);
            ips200_show_int(40,120,ImageDeal[36].Wide,2);
            ips200_show_int(40,130,ImageDeal[37].Wide,2);
            ips200_show_int(40,140,ImageDeal[38].Wide,2);
            ips200_show_int(40,150,ImageDeal[39].Wide,2);
            ips200_show_int(40,160,ImageDeal[40].Wide,2);
            ips200_show_int(40,170,ImageDeal[41].Wide,2);
            ips200_show_int(40,180,ImageDeal[42].Wide,2);
            ips200_show_int(40,190,ImageDeal[43].Wide,2);
            ips200_show_int(40,200,ImageDeal[44].Wide,2);
            ips200_show_int(40,210,ImageDeal[45].Wide,2);
            ips200_show_int(40,220,ImageDeal[46].Wide,2);
            ips200_show_int(40,230,ImageDeal[47].Wide,2);

            ips200_show_int(60,80, ImageDeal[48].Wide,2);
            ips200_show_int(60,90, ImageDeal[49].Wide,2);
            ips200_show_int(60,100,ImageDeal[50].Wide,2);
            ips200_show_int(60,110,ImageDeal[51].Wide,2);
            ips200_show_int(60,120,ImageDeal[52].Wide,2);
            ips200_show_int(60,130,ImageDeal[53].Wide,2);
            ips200_show_int(60,140,ImageDeal[54].Wide,2);
            ips200_show_int(60,150,ImageDeal[55].Wide,2);
            ips200_show_int(60,160,ImageDeal[56].Wide,2);
            ips200_show_int(60,170,ImageDeal[57].Wide,2);
            ips200_show_int(60,180,ImageDeal[58].Wide,2);
            ips200_show_int(60,190,ImageDeal[59].Wide,2);
}

    if(Image_Flag){
        for(uint8 i = 0;i < LCDW + 80;i++){
            ips200_draw_point(i+ 100,Single_BridgeYsite1 + 50,RGB565_PURPLE);
            ips200_draw_point(i+ 100,Single_BridgeYsite1 + 50,RGB565_PURPLE);
            ips200_draw_point(i+ 100,Single_BridgeYsite2 + 50,RGB565_GREEN);
            ips200_draw_point(i+ 100,Single_BridgeYsite2 + 50,RGB565_GREEN);
        }

///**************************************************显示图像信息***********************************************************************/
        ips200_show_gray_image(100, 50, Image_Use[0], LCDW, LCDH, LCDW, LCDH, 0);//用于显示压缩后后的图像
        ips200_show_gray_image(180, 50, Pixel[0], LCDW, LCDH, LCDW, LCDH, 0);    //用于显示二值化后的图像
//        ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0); //用于显示摄像头直接采取到的灰度图像（逆透视时用这个更好）
///**************************************************显示图像信息***********************************************************************/
//      //显示边界信息
        for(uint8 i = 0; i < LCDH; i++){
            ips200_draw_point(ImageDeal[i].LeftBorder  + 180,i + 50,RGB565_BLUE);
            ips200_draw_point(ImageDeal[i].RightBorder + 180,i + 50,RGB565_GREEN);

            ips200_draw_point(ImageDeal[i].Center + 100,i + 50,RGB565_RED);
            ips200_draw_point(ImageDeal[i].Center + 180,i + 50,RGB565_RED);
        }
//
//        for(uint8 i = 0; i < LCDH; i++){
//            ips200_draw_point(ImageDeal[i].LeftBoundary_First + 200,i + 30,RGB565_BLUE);
//            ips200_draw_point(ImageDeal[i].LeftBoundary_First + 200,i + 90,RGB565_BLUE);
//
//            ips200_draw_point(ImageDeal[i].RightBoundary_First + 200,i + 30,RGB565_BROWN);
//            ips200_draw_point(ImageDeal[i].RightBoundary_First + 200,i + 90,RGB565_BROWN);
//        }
//        for(uint8 i = 0; i < LCDH; i++){
//            ips200_draw_point(ImageDeal[i].LeftBoundary + 100,i + 50,RGB565_BLUE);
//            ips200_draw_point(ImageDeal[i].RightBoundary + 100,i + 50,RGB565_BROWN);
//        }
//        for(uint8 i = 0;i < 120;i++){
//            ips200_draw_point(180 + White_COL_POS,i + 30,RGB565_BROWN);
//        }
//        for(uint8 i = 0;i < 120;i++){
//            ips200_draw_point(180 + White_COL_POS1,i + 30,RGB565_BROWN);
//        }
//        for(uint8 i = 0;i < 120;i++){
//            ips200_draw_point(180 + White_COL_POS2,i + 30,RGB565_BROWN);
//        }
    }

        // 此处编写需要循环执行的代码
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************
