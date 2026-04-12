/*
 * Common.h
 *
 *  Created on: 2025年3月18日
 *      Author: 不懂
 */

#ifndef CODE_COMMON_H_
#define CODE_COMMON_H_
#include "zf_common_headfile.h"

// 宏定义绝对值函数，换为新名字 MY_ABS
#define MY_ABS(x) ((x) < 0 ? -(x) : (x))

// 宏定义，将 value 限幅在 0 到 9 之间
#define CLAMP_0_9(x) (((x) < 0) ? 0 : (((x) > 9) ? 9 : (x)))
// 宏定义，将 value 限幅在 5 到 50 之间
#define CLAMP_5_50(x) (((x) < 5) ? 5 : (((x) > 50) ? 50 : (x)))
// 宏定义，将 value 限幅在 0 到 59 之间
#define CLIP_DATA(x) ((x) < 0? 0 : ((x) > 59? 59 : (x)))
// 宏定义，将 value 限幅在 0 到 79 之间
#define CLAMP(value) ((value) < 0 ? 0 : ((value) > 79 ? 79 : (value)))

extern uint8 Zebra_Time_Flag ;
extern uint16 Zebra_Time;
extern int JUMP_START_FLAG;
extern uint16 Buzzer_Count;
extern uint8 num1;
extern int Information_Flag,Image_Flag;

extern uint8 Left_Straight_Flag ,Right_Straight_Flag;
extern uint8 length_max ;
extern uint8 Corner_LEFT,Corner_RIGHT;
extern float div_div;
void compressimage(void);
void TOF_Init(void);
void Buzzer_Init(void);
void Buzzer(void);
extern float Target_Speed_Temp ;
extern float variance, variance_acc;  //方差
void Parameter_Init(void);
void Check_Straight(void);
void Stayguy_ADS(int x_start, int y_start, int x_end, int y_end);
float Angle_Calculation(int x1, int y1, int x2, int y2, int x3, int y3);
void Curvature_Calculation(void);
float Straight_Judge(uint8 dir, uint8 start, uint8 end);     //返回结果小于1即为直线
float Straight_Judge_2(uint8 dir, uint8 start, uint8 end);
uint8 len_maxget(void);
void Pixel_Filter(void);
#endif /* CODE_COMMON_H_ */
