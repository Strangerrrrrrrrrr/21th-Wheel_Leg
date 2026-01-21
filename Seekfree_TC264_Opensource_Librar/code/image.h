/*
 * image.h
 *
 *  Created on: 2025年5月5日
 *      Author: 榴星宇
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_
#include "zf_common_headfile.h"


#define LCDH 60
#define LCDW 80

extern uint8 Pixel[LCDH][LCDW];
extern uint8 Image_Use[LCDH][LCDW];
extern uint8 ThreShold_Value;
extern float Thresold_MIN,compensation_value;//最小值和阈值的补偿值
extern float Thresold_MAX ;

uint8 Threshold_deal(uint8* image, uint16 col,uint16 row,uint32 pixel_threshold);
void Binarization(void);
void Image_Change(void);
void Image_Change_2(void);
extern uint8 Left_ERR,Right_ERR;
#endif /* CODE_IMAGE_H_ */
