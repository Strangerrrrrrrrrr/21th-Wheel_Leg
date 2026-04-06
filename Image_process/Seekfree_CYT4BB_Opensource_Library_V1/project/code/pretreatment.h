/*
*   Created on: 2026-04-06
*   Author: ii
*/

#ifndef PRETREATMENT_H
#define PRETREATMENT_H  

#include  <stdint.h>
#include  "zf_common_headfile.h"
#include  "segmentation.h"

#define POINTS_MAX_LEN  (MT9V03X_H)  //最大点数，不能超过图像高度

//原图左右边线
int ipts0[POINTS_MAX_LEN][2]      //存储左边线点坐标的数组，第一维为点数，第二维为坐标（0为x坐标，1为y坐标）
int ipts1[POINTS_MAX_LEN][2]      //存储右边线点坐标的数组
int ipts0_num, ipts1_num        //实际找到的左边线点数和右边线点数 = 总字节 /单行字节数 = 行数

//边线提取&处理
void FindEdge(void)

//角点提取&筛选
void FindCorner(void)

//绘制道路元素
//设定 image_t img_line = DEF_IMAGE()
void DrawRoad(void)  


//单边桥特征检测
void FindBridge(void)


//台阶特征检测
void FindStairs(void)



#endif /*PRETREATMENT_H*/