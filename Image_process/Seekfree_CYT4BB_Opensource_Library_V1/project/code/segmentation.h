/*
*
*       Create on:2026/3/24
*       author: ii
*/
#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <stdint.h>
#include "zf_common_headfile.h"

typedef struct image {
    uint8_t *data;
    uint32_t width;  //图像宽度（列数）column
    uint32_t height; //图像高度（行数）row
    uint32_t step;
} image_t;

extern int clip(int x,int lower,int upper);  //裁剪函数，将x限制在lower和upper之间
#define IMAGE_Pixel(img, x, y)          ((img)->data[(y)*(img)->step+(x)])   //访问img的像素，img为image_t类型指针，x和y为像素坐标，返回该像素的灰度值
#define IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))   //裁剪图像边界当中的越界坐标，[clamped border strategy]
#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}  
//#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}

/*压缩图片 180x120 ->  60x80
void compressimage(void);
#define LCDH 60
#define LCDW 80
extern uint8_t Image_Use[LCDH][LCDW];  //压缩后的图像数据
extern uint8_t Pixel[LCDH][LCDW];  //二值化后的图像数据
*/


// 深拷贝图片，img0和img1不可以指向相同图片
void clone_image(image_t *img0, image_t *img1);

// 清空图片
void clear_image(image_t *img);




// 左手巡线同时自适应二值化，从(x,y)开始向上沿白边线走
void Lmaze_Adapt(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);

// 右手巡线同时自适应二值化，从(x,y)开始向上沿白边线走
void Rmaze_Adapt(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);










// 像素逻辑与，即都为255时才为255
void image_and(image_t *img0, image_t *img1, image_t *img2);

// 像素逻辑或，即都为255时才为255
void image_or(image_t *img0, image_t *img1, image_t *img2);

// 最小值降采样x2
void minpool2(image_t *img0, image_t *img1);

// 3x3高斯滤波，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void blur3(image_t *img0, image_t *img1);

// 3x3 Sobel梯度计算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void sobel3(image_t *img0, image_t *img1);

// 3x3腐蚀运算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void erode3(image_t *img0, image_t *img1);

// 3x3膨胀运算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void dilate3(image_t *img0, image_t *img1);

// 点集三角滤波
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);

// 点集等距采样
void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist);

// 点集局部角度变化率
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);

// 角度变化率非极大抑制
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);


#endif /*SEGMENTATION_H*/