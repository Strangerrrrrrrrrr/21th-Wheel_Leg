/*
 * Circle.h
 *
 *  Created on: 2025年3月22日
 *      Author: 不懂
 */

#ifndef CODE_CIRCLE_H_
#define CODE_CIRCLE_H_
#include "zf_common_headfile.h"

typedef struct {
//    int16 Bend_Road;                            /*0 :无               1 :右弯道     2 :左弯道*/
    int16 image_element_rings;                  /*0 :无圆环          1 :左圆环       2 :右圆环*/
    int16 ring_big_small;                       /*0:无                     1 :大圆环       2 :小圆环*/
    int16 image_element_rings_flag;             /*圆环进程*/
    int16 straight_long;                        /*长直道标志位*/
//    int16 Garage_Location;                      /*0 :无车库          1 :左车库       2 :右车库*/
//    int16 Zebra_Flag;                           /*0 :无斑马线       1 左车库       2 :右车库*/
//    int16 Ramp;                                  /*0 :无坡道          1：坡道*/
//    int16 RoadBlock_Flag;                        /*0 :无路障            1 :路障*/
//    int16 Out_Road;                               /*0 :无断路      1 :断路*/
} ImageFlagtypedef;

extern int Circle_Flag;
extern float Road_Width_L ,Road_Width_R ;
extern uint8 Number ;
extern float Left_OUTNUM ,Right_OUTNUM ;
extern ImageFlagtypedef ImageFlag;
extern uint8 Ring_Help_Flag;
void Check_Circle(void);
void Run_Circle(void);
extern uint8 Circle_HalfRoadWidth[60];
extern uint8 Left_Circle_Lost ;
extern uint8 Left_line1 ,Left_line2 ;
extern int wubianhang ;

extern int Left_RingsFlag_Point1_Ysite, Left_RingsFlag_Point2_Ysite;
extern int Right_RingsFlag_Point1_Ysite, Right_RingsFlag_Point2_Ysite; //右圆环判断的两点纵坐标
extern uint8 Circle_State;
#endif /* CODE_CIRCLE_H_ */
