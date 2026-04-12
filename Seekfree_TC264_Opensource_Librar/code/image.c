/*
 * image.c
 *
 *  Created on: 2025年5月5日
 *      Author: 榴星宇
 */
#include "image.h"

/*二值化后存放数组*/uint8 Pixel[LCDH][LCDW];//   60*80
uint8 Image_Use[LCDH][LCDW];          //灰度图像
/*阈值计算时变量定义*///变量依次为 初次计算阈值，             使用的阈值，             阈值下限                 阈值补偿值
            uint8 ThreShold_Value_Original = 0, ThreShold_Value = 0;  float Thresold_MIN = 180, compensation_value = -10;
            float Thresold_MAX = 215;
uint8 Threshold_deal(uint8* image, uint16 col,uint16 row,uint32 pixel_threshold) {
#define GrayScale 256
  uint16 width = col;//列
  uint16 height = row;//行
  int pixelCount[GrayScale];//像素计数[灰度]；
  float pixelPro[GrayScale];//像素专业版[灰度模式]
  int i, j, pixelSum = width * height;//int i, j, 像素总和 = 宽度 * 高度
  uint8 threshold = 0;
  uint8* data = image;  //指向像素数据的指针
  //遍历灰度图
  for (i = 0; i < GrayScale; i++) {
    pixelCount[i] = 0;
    pixelPro[i] = 0;
  }

  uint32 gray_sum = 0;//灰度级
  //统计灰度级中每个像素在整幅图像中的个数
  for (i = 0; i < height; i += 1) {
    for (j = 0; j < width; j += 1) {
      pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
      gray_sum += (int)data[i * width + j];  //灰度值总和
    }
  }

  //计算每个像素值的点在整幅图像中的比例
  for (i = 0; i < GrayScale; i++) {
    pixelPro[i] = (float)pixelCount[i] / pixelSum;
  }

  //遍历灰度级[0,255]
  float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
  w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
  for (j = 0; j < pixel_threshold; j++) {
    w0 +=
        pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和 即背景部分的比例
    u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

    w1 = 1 - w0;
    u1tmp = gray_sum / pixelSum - u0tmp;

    u0 = u0tmp / w0;    //背景平均灰度
    u1 = u1tmp / w1;    //前景平均灰度
    u = u0tmp + u1tmp;  //全局平均灰度
    deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
    if (deltaTmp > deltaMax) {
      deltaMax = deltaTmp;
      threshold = (uint8)j;
    }
    if (deltaTmp < deltaMax) break;
  }
  return threshold;
}

uint8 Threshold_Calculate1(uint8 *image, uint16 col, uint16 row,uint16 num)   //注意计算阈值的一定要是原图像
{
   #define GrayScale 256
    uint16 width = col;
    uint16 height = row-num;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = num; i < height+num; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {

                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

               w1=1-w0;
               u1tmp=gray_sum/pixelSum-u0tmp;

                u0 = u0tmp / w0;              //背景平均灰度
                u1 = u1tmp / w1;              //前景平均灰度
                u = u0tmp + u1tmp;            //全局平均灰度
                deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
                if (deltaTmp > deltaMax)
                {
                    deltaMax = deltaTmp;
                    threshold = (uint8)j;
                }
                if (deltaTmp < deltaMax)
                {
                    break;
                }
        }

    return threshold;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 Threshold_Calculate2(uint8 *image, uint16 col, uint16 row,uint16 num)   //注意计算阈值的一定要是原图像
{
   #define GrayScale 256
    uint16 width = col;
    uint16 height = row - num;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 1; i < height; i+=2)
    {
        for (j = num; j < width+num; j+=2)
        {
            if( data[i * width + j]  > ThreShold_Value_Original )
            {
                pixelCount[(int)data[i * width + j]]++ ;  //将当前的点的像素值作为计数数组的下标
                gray_sum+=(int)data[i * width + j] ;       //灰度值总和
            }
            else
            {
                pixelCount[(int) ThreShold_Value_Original]++ ;  //将当前的点的像素值作为计数数组的下标
                gray_sum+= (int) ThreShold_Value_Original ;       //灰度值总和
            }
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {

                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

               w1=1-w0;
               u1tmp=gray_sum/pixelSum-u0tmp;

                u0 = u0tmp / w0;              //背景平均灰度
                u1 = u1tmp / w1;              //前景平均灰度
                u = u0tmp + u1tmp;            //全局平均灰度
                deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
                if (deltaTmp > deltaMax)
                {
                    deltaMax = deltaTmp;
                    threshold = (uint8)j;
                }
                if (deltaTmp < deltaMax)
                {
                    break;
                }
        }

    return threshold;
}


//void Binarization(void) {
//
//    //一次大津法//阳光分割
////    ThreShold_Value = Threshold_deal(Image_Use[0], LCDW, LCDH, 255) + compensation_value;
//    ThreShold_Value = Threshold_deal(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 255) + compensation_value;
//    // 阈值下限限幅
//    if (ThreShold_Value < Thresold_MIN)
//        ThreShold_Value = Thresold_MIN;
//
//    if(ThreShold_Value > Thresold_MAX){
//        ThreShold_Value = Thresold_MAX;
//    }
//    uint8 i, j = 0;
//    uint8 thre;
//    for (i = 0; i < LCDH; i++) {
//        for (j = 0; j < LCDW; j++) {
//            int col_extra_threshold = 0;//列阈值补偿
//            // 判断列数是否小于 10 或者大于 70
//            if (j < 10 || j > 70) {
//                col_extra_threshold = -5;
//            }
//            // 加上列相关的额外阈值
//            thre = ThreShold_Value + col_extra_threshold;
//
//            // 数值越大，显示的内容越多，较浅的图像也能显示出来
//            if (Image_Use[i][j] > thre)
//                Pixel[i][j] = 255; // 白
//            else
//                Pixel[i][j] = 0;   // 黑
//        }
//    }
//}

void Binarization(void) {
    // 一次大津法//阳光分割
//    ThreShold_Value = Threshold_deal(Image_Use[0], LCDW, LCDH, 255) + compensation_value;
//    ThreShold_Value = Threshold_deal(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 255) + compensation_value;

    ThreShold_Value_Original = Threshold_Calculate1(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 0);
    ThreShold_Value  = Threshold_Calculate2(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 0) + compensation_value;
    // 阈值下限限幅
    if (ThreShold_Value < Thresold_MIN)
        ThreShold_Value = Thresold_MIN;

    if(ThreShold_Value > Thresold_MAX){
        ThreShold_Value = Thresold_MAX;
      }
    uint8 i, j;
    uint8 thre;

    for (i = 0; i < LCDH; i++) {
        // 行阈值补偿 - 处理第30行到第0行时，每隔5行阈值增加2
        int row_extra_threshold = 0;
        if (i <= 40) {
            row_extra_threshold = (40 - i) / 2 * 1; // 计算行补偿值
        }

    for (j = 0; j < LCDW; j++) {
        // 列阈值补偿
        int col_extra_threshold = 0;
        // 判断列数是否小于 15 或者大于 65
        if (j < 10 || j > 70) {
            col_extra_threshold = -15;
        }

        // 总阈值 = 基础阈值 + 行补偿 + 列补偿
        thre = ThreShold_Value + row_extra_threshold + col_extra_threshold;

        // 二值化处理：数值越大，显示的内容越多，较浅的图像也能显示出来
        if (Image_Use[i][j] > thre)
            Pixel[i][j] = 255; // 白
        else
            Pixel[i][j] = 0;   // 黑
    }
  }
}

uint8 Left_ERR = 0,Right_ERR = 0;
uint8 Left_ERR_Flag = 0,Right_ERR_Flag = 0;
void Image_Change(void){

    //清零后使用
    Right_ERR_Flag = 0;Left_ERR_Flag = 0;
    Left_ERR = 0;      Right_ERR = 0;
    for(uint8 Ysite = 59;Ysite > 0 ;Ysite--){
        if(Pixel[Ysite][2] == 255 || Pixel[Ysite][4] == 255){
            Left_ERR++;
            if(Left_ERR > 15){
                for(uint8 Xsite = 2;Xsite <= 15;Xsite++){
                    if(Pixel[40][Xsite]==255){
                        Pixel[25][Xsite]=0;
                        Pixel[26][Xsite]=0;
                        Pixel[27][Xsite]=0;
                    }
                    else break;
                }
            }
        }
        if(Pixel[Ysite][78] == 255 || Pixel[Ysite][76] == 255 ){
            Right_ERR++;
            if(Right_ERR > 15){
                for(uint8 Xsite = 79;Xsite >= 64;Xsite--){
                    if(Pixel[40][Xsite]==255){
                        Pixel[25][Xsite]=0;
                        Pixel[26][Xsite]=0;
                        Pixel[27][Xsite]=0;
                    }
                    else break;
                }
            }
        }
    }
}


void Image_Change_2(void){

    //清零后使用
    Right_ERR_Flag = 0;Left_ERR_Flag = 0;
    Left_ERR = 0;      Right_ERR = 0;
    for(uint8 Ysite = 59;Ysite > 0 ;Ysite--){
        if(Pixel[Ysite][2] == 255 || Pixel[Ysite][4] == 255){
            Left_ERR++;
        }
        if(Pixel[Ysite][78] == 255 || Pixel[Ysite][76] == 255 ){
            Right_ERR++;
        }

        if(Left_ERR >= 15 && Right_ERR >= 15){
            for(uint8 Xsite = 2;Xsite <= 15;Xsite++){
                if(Pixel[45][Xsite]==255){
                    Pixel[27][Xsite]=0;
                    Pixel[28][Xsite]=0;
                    Pixel[29][Xsite]=0;
                }
                else break;
      }
            for(uint8 Xsite = 79;Xsite >= 64;Xsite--){
                if(Pixel[45][Xsite]==255){
                    Pixel[27][Xsite]=0;
                    Pixel[28][Xsite]=0;
                    Pixel[29][Xsite]=0;
                }
                else break;
            }
        }
    }
}
