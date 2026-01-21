/*
 * Obstacle.h
 *
 *  Created on: 2025年4月13日
 *      Author: 不懂
 */

#ifndef CODE_OBSTACLE_H_
#define CODE_OBSTACLE_H_

#include "zf_common_headfile.h"

extern uint8 BIG_Obstacle_Flag;
extern uint8 BIG_Obstacle_Flag_Last;

extern uint16 Black_Dot ;
extern uint8 JUMP_NUM ;
extern uint8 White_Col;
extern uint8 Turn_Flag ;
extern uint16 time1 ; //准备跳跃时间
extern uint16 count1;extern uint16 count2;//伸腿计时;//准备跳跃计时
extern uint16 TIME;
extern uint16 Single_STOP_TIME ;
extern uint8 flag_tiaoyue,tiaoyue_flag;
extern uint8 Servo_Control_Flag;
extern uint8 JUMP_FLAG;
extern uint8 TrackWidthChangeFlag ;
extern uint8 Road_Width[60];
//单边桥判断标志位
extern uint8 Single_SideBridge_Flag,Single_Bridge_Flag_Last;
extern uint8 Left_Corner ,Right_Corner,Left_Corner2,Right_Corner2 ;
extern int Left_Corner_Ysite ,Right_Corner_Ysite,Left_Corner_Ysite2 ,Right_Corner_Ysite2 ;
extern float Median_Deviation ;
extern uint8 Single_BridgeYsite1,Single_BridgeYsite2,Single_BridgeYsite3 ;
extern uint8 Single_Bridge_TIME;

extern int Single_3cmBridge ;

extern uint8 White_DOT_NUM;
extern uint8 White_DOT1[80];
extern uint8 White_DOT2[80];

extern uint8 Single_Bridge_Left,Single_Bridge_Right;

extern uint8 White_COL ,White_COL_MAX,White_COL_POS,White_COL_MIN  ;
extern uint8 White_COL_Left ,White_COL_Right ;

extern uint8 White_COL_MAX1, White_COL_POS1 , White_COL_MIN1 ;
extern uint8 White_COL_Left1 ,White_COL_Right1;//白列计数清零防止前面干扰
extern uint8 White_COL_MAX2, White_COL_POS2 , White_COL_MIN2 ;
extern uint8 White_COL_Left2  , White_COL_Right2;
extern float OFFLine ,Col_MAX ;
extern float Obstacle_Encoder_OUT;
extern float Width_Offest ;

//函数声明
void Roll_control(void);
void Check_BIG_Obstacle(void);
void Jump_Obstacle(void);
void Run_Single_Bridge(void);
void Check_Single_SideBridge(void);
void Obstacle_TurnControl(void);
void ChecstraightLength(void);
void Check_TrackWidth(void);
void Check_Single_Bridge(void);

#endif /* CODE_OBSTACLE_H_ */
