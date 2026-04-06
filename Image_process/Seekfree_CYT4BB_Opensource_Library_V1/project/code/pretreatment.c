/*
*   Created on: 2026-04-03
*   Author: ii
*
*/

#include "pretreatment.h"

extern image_t capture_image;  //摄像头图像结构体变量，指向摄像头图像数据
extern image_t draw_image;     //绘制道路线的图像结构体变量，指向显示屏图像数据
extern int ipts0[POINTS_MAX_LEN][2];
extern int ipts1[POINTS_MAX_LEN][2];
extern int ipts0_num, ipts1_num;
extern float begin_x,begin_y;  //边线检测 起始点坐标
extern float thres;   //边线检测 起始点阈值


void FindEdge(){
    //1. 左右边线检测   
    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);    //动态获取数组大小，总字节/单行字节=行数=点数
    int x1 = capture_image.width / 2 - begin_x;  //计算起始点坐标，假设起始点在图像中心偏左begin_x个像素处
    int y1 = begin_y;
    for(;x1>0;x1--)
    {
        if(IMAGE_PIXEL(&capture_image,x1 - 1 ,y1) < thres)
        break;
    } //循环向左搜索黑线边界
    //左手迷宫巡线同时自适应二值化
    if(IMAGE_PIXEL(&capture_image,x1,y1) >= thres)
    {
        Lmaze_Adapt(&capture_image, blcok_size, clip_value, x1, y1, ipts0, &ipts0_num);  
    }
    else ipts0_num = 0;  //起始点不在白线上则认为没有找到边线

    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);    //动态获取数组大小，总字节/单行字节=行数=点数
    int x2 = capture_image.width / 2 + begin_x;    //计算起始点坐标，假设起始点在图像中心偏右begin_x个像素处
    int y2 = begin_y;
    for(;x2 < capture_image.width; x2++)
    {
        if(IMAGE_PIXEL(&capture_image,x2 + 1 ,y2) < thres)
        break;
    }//循环向右搜索黑线边界
    //右手迷宫巡线同时自适应二值化
    if(IMAGE_PIXEL(&capture_image,x2,y2) >= thres)
    {
        Rmaze_Adapt(&capture_image, blcok_size, clip_value, x2, y2, ipts1, &ipts1_num);  
    }
    else ipts1_num = 0;  //起始点不在白线上则认为没有找到边线

    //2. IPM


    //3. 边线滤波


    //4. 角度变化率



    //5.采样


    //6.中线跟踪
}


void FindCorner(){
    //1. 角点检测

}