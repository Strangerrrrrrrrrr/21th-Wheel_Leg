#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_
#include "zf_common_headfile.h"

extern float thres;          //边线检测 起始点阈值
extern float block_size;     //自适应阈值block大小
extern float clip_value;     //自适应阈值调整量
extern float begin_x;        //起始点距离图像中心的左右偏移量
extern float begin_y;        //起始点距离图像中心的上下偏移量
















void flash_Nag_Write(void);
void flash_Nag_Read(void);


#endif
