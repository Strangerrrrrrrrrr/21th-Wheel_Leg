/*
 * Obstacle.c
 *
 *  Created on: 2025年4月13日
 *      Author: 不懂
 */
#include "Obstacle.h"

uint8 BIG_Obstacle_Flag = 0;
uint8 BIG_Obstacle_Flag_Last = 0;

uint16 Black_Dot = 0,Black_Dot2 = 0;
uint8 JUMP_NUM = 0;
uint8 White_Col;


float OFFLine = 25;float Col_MAX = 35;
void Check_BIG_Obstacle(void){

    //先清零后使用
    Black_Dot = 0;Black_Dot2 = 0;/*黑点数目*/

    //黑点数量统计+黑白跳变检测
    for(uint8 Ysite = 10 ;Ysite < LCDH - 10; Ysite++){
        for(uint8 Xsite = 25 ;Xsite < LCDW - 25; Xsite++){

            if(Pixel[Ysite][Xsite] == 0){
                Black_Dot++;
            }

            if(Pixel[Ysite][Xsite] == 0 && Ysite == ImageStatus.OFFLine - 2){
                Black_Dot2 ++;
            }
        }
    }

    //最长白列计算gf
    White_Col = len_maxget();
    if(Black_Dot > 400                //图像中间黑点数判断

     &&Black_Dot2 > 20
     &&ImageStatus.Left_Line  < 5    //左边无边行要求
     &&ImageStatus.Right_Line < 5    //右边无边行要求
     //左右不丢线判断
     &&LeftBoundary_Error  < 2
     &&RightBoundary_Error < 2
     &&White_Col <= Col_MAX                //最长白列数要求
     &&ImageStatus.OFFLine >= OFFLine      //截至行应该很大
     &&BIG_Obstacle_Flag_Last == 0    //历史标志位满足//一次只跑一次
      ){
        BIG_Obstacle_Flag = 1;
        BIG_Obstacle_Flag_Last = 1;
        Buzzer_Count = 6;
    }
}

/**********************路障跳跃***********************/
/*************启动三秒跳跃一次，注意安全！*************/

uint8 flag_tiaoyue = 0,tiaoyue_flag = 0;
uint16 count1 = 0; //准备跳跃计时
uint16 count2 = 0; //第二次跳跃计时
uint16 TIME = 0;   //屏蔽速度环计时
uint8 Servo_Control_Flag = 1;  //舵机控制标志位
uint8 JUMP_FLAG = 0;
uint16 time1 = 5; //准备跳跃时间
uint16 Single_STOP_TIME = 0;

uint8 Turn_Flag = 1;
void Jump_Obstacle(void){
    count1++;
    //准备下次跳跃计时
//    if(flag_tiaoyue)count2++;
    if(count1==time1)
    {
        //达到时长跳跃标志位
        JUMP_FLAG = 1;
//        Target_Speed = 300;
        //关闭舵机控制
        Servo_Control_Flag = 0;
        //转向关闭
        duty_YAW = 0;
       if(flag_tiaoyue==0)
         {
            if(time1==100)
               {
                servo_duty(90);
                 flag_tiaoyue=1;//下次跳跃标志位

                 Turn_Flag = 1;//开启转向环
                 BIG_Obstacle_Flag = 0;
               }
            if(time1==5)
               {
                 time1 = 100;
                 tiaoyue_flag=1;

                //把角度换参数减半，防止电机猛转无法跳跃后稳住
                 Angle_KP = (Angle_KP / 2);
                 //关闭一帧转向
                 duty_YAW = 0;
                 //转向环关闭
                 Turn_Flag = 0;

                 //五连杆运动学逆解算伸腿
                 X_Y_Set(19,145);
                 inverseKinematics();
               }
         }
       count1=0;//为隔离伸腿和收腿时间间隔，为了让舵机正常打两次角，以免两次重合
    }

    //原地跳跃测试//达到时长3s开始下一次跳跃
//    if(count2==3000)
//    {
//      time1=50;
//      flag_tiaoyue=0;
//
//      Servo_Utilization(90);
//      count2=0;
//    }
}

//单边桥判断标志位
uint8 Single_SideBridge_Flag = 0,Single_Bridge_Flag_Last = 0;
uint8 Left_Corner = 0,Right_Corner = 0,Left_Corner2 = 0,Right_Corner2 = 0;
int Left_Corner_Ysite = 0,Right_Corner_Ysite = 0,Left_Corner_Ysite2 = 0,Right_Corner_Ysite2 = 0;

void Corner_Check(void){
    //临时标志位先清零后使用//临时标志位先清零后使用//临时标志位先清零后使用
    //是否找到角点标志位
    Left_Corner  = 0;         Right_Corner = 0;
    Left_Corner2 = 0;         Right_Corner2 = 0;
    //角点坐标变量
    Left_Corner_Ysite = 0;    Right_Corner_Ysite = 0;
    Left_Corner_Ysite2 = 0;    Right_Corner_Ysite2 = 0;


    //单边桥近端拐点（左）
    for(int Ysite = 50; Ysite > ImageStatus.OFFLineBoundary; Ysite--){
        if(
           //边界大跳跃
             ImageDeal[CLIP_DATA(Ysite-2)].LeftBoundary - ImageDeal[CLIP_DATA(Ysite)].LeftBoundary_First > 7
           &&ImageDeal[CLIP_DATA(Ysite-3)].LeftBoundary - ImageDeal[CLIP_DATA(Ysite)].LeftBoundary_First > 7
        ){
            Left_Corner = 1;
            Left_Corner_Ysite = Ysite;
            break;
        }
        //如果出现边线撕裂的情况就会满足不了上述情况
//        else if(
//            ImageDeal[CLIP_DATA(Ysite-1)].LeftBoundary - ImageDeal[CLIP_DATA(Ysite)].LeftBoundary   > 2
//          &&ImageDeal[CLIP_DATA(Ysite-2)].LeftBoundary - ImageDeal[CLIP_DATA(Ysite-1)].LeftBoundary > 2
//          &&ImageDeal[CLIP_DATA(Ysite-3)].LeftBoundary - ImageDeal[CLIP_DATA(Ysite-2)].LeftBoundary > 2
//        ){
//            Left_Corner = 1;
//            Left_Corner_Ysite = Ysite;
//            break;
//        }
    }
    //单边桥远端拐点（左）
    for(int Ysite = 50; Ysite > ImageStatus.OFFLineBoundary; Ysite--){
        if(
           //边界大跳跃
             ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite-2].LeftBoundary > 5
           &&ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite-3].LeftBoundary > 5

        ){
            Left_Corner2 = 1;
            Left_Corner_Ysite2 = Ysite;
            break;
        }

//        else if(
//            ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite-1].LeftBoundary_First   > 2
//          &&ImageDeal[Ysite-1].LeftBoundary_First - ImageDeal[Ysite-2].LeftBoundary_First > 2
//          &&ImageDeal[Ysite-2].LeftBoundary_First - ImageDeal[Ysite-3].LeftBoundary_First > 2
//        ){
//            Left_Corner2 = 1;
//            Left_Corner_Ysite2 = Ysite;
//            break;
//        }
    }

    //单边桥近端拐点（右）
    for(int Ysite = 50; Ysite > ImageStatus.OFFLineBoundary; Ysite--){
    if(
           //边界大跳跃
           ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite-2].RightBoundary > 7
         &&ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite-3].RightBoundary > 7
        ){
            Right_Corner = 1;
            Right_Corner_Ysite = Ysite;
            break;
        }
//    else if(
//        //边界撕裂
//         ImageDeal[Ysite].RightBoundary_First - ImageDeal[Ysite-1].RightBoundary_First > 2
//       &&ImageDeal[Ysite-1].RightBoundary_First - ImageDeal[Ysite-2].RightBoundary_First > 2
//       &&ImageDeal[Ysite-2].RightBoundary_First - ImageDeal[Ysite-3].RightBoundary_First > 2
//       ){
//        Right_Corner = 1;
//        Right_Corner_Ysite = Ysite;
//        break;
//      }
    }
    //单边桥远端拐点（右）
    for(int Ysite = 50; Ysite > ImageStatus.OFFLineBoundary; Ysite--){
        if(
            //边界大跳跃
              ImageDeal[Ysite-2].RightBoundary - ImageDeal[Ysite].RightBoundary_First > 5
            &&ImageDeal[Ysite-3].RightBoundary - ImageDeal[Ysite].RightBoundary_First > 5

        ){
            Right_Corner2 = 1;
            Right_Corner_Ysite2 = Ysite;
            break;
        }

//        else if(
//             ImageDeal[Ysite-1].RightBoundary- ImageDeal[Ysite].RightBoundary  > 2
//           &&ImageDeal[Ysite-2].RightBoundary- ImageDeal[Ysite-1].RightBoundary > 2
//           &&ImageDeal[Ysite-3].RightBoundary- ImageDeal[Ysite-2].RightBoundary > 2
//        ){
//            Right_Corner2 = 1;
//            Right_Corner_Ysite2 = Ysite;
//            break;
//        }
    }
}


uint8 TrackWidthChangeFlag = 0;
uint8 Road_Width[LCDH]={0};

uint8 Single_BridgeYsite1 = 0,Single_BridgeYsite2 = 0;
uint8 Black_Dot3 = 0;
void Check_TrackWidth(void){
    //赛道宽度突变标志位
    TrackWidthChangeFlag = 0;
    //近端远端赛道宽度突变行
    Single_BridgeYsite1 = 0;Single_BridgeYsite2 = 0,Single_BridgeYsite3 = 0;
    //赛道宽度突变出某一行的黑点数
    Black_Dot3 = 0;

    //存储实际赛道宽度（利用八领域）
    for(uint8 Ysite = 59;Ysite >= ImageStatus.OFFLineBoundary + 1;Ysite--){
        Road_Width[Ysite] =  ImageDeal[Ysite].RightBoundary - ImageDeal[Ysite].LeftBoundary;
    }
    for(uint8 Ysite = (uint8)ImageStatus.OFFLineBoundary;Ysite > 0;Ysite--){
        Road_Width[Ysite] = Road_Width[Ysite + 1];
    }

    //近端突变
    for(uint8 Ysite = 55;Ysite >= ImageStatus.OFFLineBoundary + 1;Ysite--){

        if(Road_Width[Ysite] - Road_Width[CLIP_DATA(Ysite - 2 )] >= 5){
            Single_BridgeYsite1 = Ysite - 2;
            break;
        }
    }
    //远端突变1
    for(uint8 Ysite = Single_BridgeYsite1;Ysite >= ImageStatus.OFFLineBoundary + 1;Ysite --){

        if(Road_Width[CLIP_DATA(Ysite - 2)] - Road_Width[Ysite] >= 2 ){
            Single_BridgeYsite2 = Ysite;
            break;
        }
    }

    //突变坐标满足要求
    if(
        (Single_BridgeYsite1  >= Single_BridgeYsite2 + 1
         &&Single_BridgeYsite1 <= 50
         &&Single_BridgeYsite2 != 0
         &&Single_BridgeYsite2 != 0)
        ){
        TrackWidthChangeFlag = 1;
    }
}

uint8 TrackWidthChangeFlag2 = 0;
uint8 Single_BridgeYsite3 = 0;
void Check_TrackWidth2(void){
    //赛道宽度突变标志位
    TrackWidthChangeFlag = 0;
    //近端远端赛道宽度突变行
    Single_BridgeYsite3  = 0;
    //近端突变
    for(uint8 Ysite = 55; Ysite >= ImageStatus.OFFLine; Ysite--){

        if(Road_Width[Ysite] - Road_Width[CLIP_DATA(Ysite - 2 )] >= 10){
            Single_BridgeYsite3 = Ysite - 2;

            break;
        }
    }

    if(Single_BridgeYsite3 > 10 && Single_BridgeYsite3 < 35){
        TrackWidthChangeFlag2 = 1;
    }
}

uint8 Single_Bridge_3CM = 0,Single_Bridge_5CM = 0;
uint8 Single_Bridge_TIME = 4;

uint8 White_DOT1_NUM = 0;
uint8 White_DOT1[LCDW] = {0};
uint8 White_DOT2[LCDW] = {0};

uint8 Single_Bridge_Left = 0,Single_Bridge_Right = 0;

uint8 White_COL = 0,White_COL_MAX = 0,White_COL_POS = 0,White_COL_MIN = 0 ;
uint8 White_COL_Left = 0,White_COL_Right = 0;
void Check_WhiteDot(void){

    Check_TrackWidth();

    Single_Bridge_Left = 0,Single_Bridge_Right = 0;
    White_COL = 0;White_COL_Left = 0;White_COL_Right = 0;
    White_COL_MAX = 0;White_COL_POS = 0;
    for(uint8 i;i < LCDW;i++){
        White_DOT1[i] = 0;
        White_DOT2[i] = 0;
    }//先清零后使用//先清零后使用//先清零后使用//先清零后使用

    //右单边桥的识别
    //统计左右最长白列的值(左侧)
    if(Single_Bridge_Right == 0 && Single_Bridge_Left == 0){
        for(uint8 Xsite = 20;Xsite < LCDW-20;Xsite++){
            for(uint8 Ysite = 55;Ysite > 0;Ysite--){
                if(Pixel[Ysite][Xsite] == 0){
                    White_DOT1[Xsite] = 55 - Ysite;
                    break;
                }else if(Ysite == 1){
                    White_DOT1[Xsite] = 55;
                    break;
                }
            }
            //存放最长白列在那一列处
            if(White_COL_MAX <= White_DOT1[Xsite]){
                White_COL_POS = Xsite;
                White_COL_MAX = White_DOT1[Xsite];
                White_COL_MIN = White_DOT1[Xsite + 3];
            }
        }//计算每一行的最长白列数

        //统计左右最长白列的值(左侧)
        for(uint8 Xsite = 20;Xsite < White_COL_POS; Xsite++){
            if(
                 MY_ABS((White_DOT1[White_COL_POS] - White_DOT1[Xsite])) <= 1 /*&& White_DOT1[Xsite] < 45*/
               ){
                White_COL_Left ++;
            }
        }
        //统计左右最长白列的值(右侧)
        for(uint8 Xsite = LCDW-20;Xsite >  White_COL_POS; Xsite--){
            if((MY_ABS(White_DOT1[Xsite] - White_DOT1[White_COL_POS + 1]) <= 1)/*&& White_DOT1[Xsite] < 45*/)
            {
                White_COL_Right ++;
            }
        }

        //判断右单边桥
        if(
             Straight_Left_Varance    <= 1&& Straight_Right_Varance  > 1
             &&Straight_Left_Varance1 > 1 && Straight_Right_Varance1 > 1
           &&White_DOT1[White_COL_POS] - White_DOT1[White_COL_POS + 3] >= 2 && White_COL_Left > 5 && White_COL_Right > 8
           &&White_COL_Right - White_COL_Left  < 10
           //无边行判断
           &&ImageStatus.Left_Line  < 3
           &&ImageStatus.Right_Line < 3
           //左边丢线
           &&LeftBoundary_Error  < 3
           &&RightBoundary_Error < 3
           //中线方差
           &&variance_acc < 20
           &&TrackWidthChangeFlag
        ){
           Single_Bridge_Right = 1;
           Buzzer_Count = 6;
        }
        else{
            Single_Bridge_Right = 0;Single_Bridge_Left = 0;
        }
    }
    White_COL_MAX = 0;White_COL_POS = 0;
    White_COL_Left = 0;White_COL_Right = 0;
    //左单边桥的识别
    if(Single_Bridge_Right == 0 && Single_Bridge_Left == 0){

        for(uint8 Xsite = LCDW-20;Xsite > 20;Xsite--){
            for(uint8 Ysite = 55;Ysite > 0;Ysite--){
                if(Pixel[Ysite][Xsite] == 0){
                    White_DOT2[Xsite] = 55 - Ysite;
                    break;
                }else if(Ysite == 1){
                    White_DOT2[Xsite] = 55;
                    break;
                }
            }
            //存放最长白列在那一列处
            if(White_COL_MAX <= White_DOT2[Xsite]){
                White_COL_POS = Xsite;
                White_COL_MAX = White_DOT2[Xsite];
                White_COL_MIN = White_DOT2[Xsite - 3];
            }
        }//计算每一行的最长白列数

    //统计左最长白列的值(右侧)
    for(uint8 Xsite = 20;Xsite < White_COL_POS; Xsite++){
        if(
             (MY_ABS((White_DOT2[White_COL_POS - 1] - White_DOT2[Xsite])) <= 1) && White_DOT2[Xsite]<= 45
           ){
            White_COL_Left ++;
        }
    }
    //统计左最长白列的值(右侧)
    for(uint8 Xsite = LCDW-20;Xsite >  White_COL_POS; Xsite--){
        if((MY_ABS(White_DOT2[Xsite] - White_DOT2[White_COL_POS]) <= 1) && White_DOT2[Xsite] <= 45)
        {
            White_COL_Right++;
        }
    }
    //判断左单边桥
    if(
          Straight_Right_Varance <= 1 && Straight_Left_Varance > 1
        &&Straight_Left_Varance1 > 1  && Straight_Right_Varance1  > 1
        &&White_DOT2[White_COL_POS] - White_DOT2[White_COL_POS - 3] >= 2 && White_COL_Left > 8 && White_COL_Right >= 4
        &&White_COL_Left - White_COL_Right < 10
        //无边行判断
        &&ImageStatus.Left_Line  < 3
        &&ImageStatus.Right_Line < 3
        //左边丢线
        &&LeftBoundary_Error  < 3
        &&RightBoundary_Error < 3
        &&TrackWidthChangeFlag
        //中线方差
        &&variance_acc < 20
        ){
        Single_Bridge_Left = 1;
        Buzzer_Count = 6;
    }
    else{
        Single_Bridge_Right = 0;Single_Bridge_Left = 0;
    }
   }
    Check_Single_SideBridge();
}


uint8 White_COL_MAX1 = 0, White_COL_POS1 = 0 , White_COL_MIN1 = 0;
uint8 White_COL_Left1 = 0 ,White_COL_Right1 = 0;//白列计数清零防止前面干扰
void Check_Bridge_Left(void){

    Check_TrackWidth();//用于检测赛道宽度的变化

   White_COL_MAX1 = 0;White_COL_POS1 = 0;
   White_COL_Left1 = 0;White_COL_Right1 = 0;
   for(uint8 i = 0;i < LCDW;i++){
       White_DOT1[i] = 0;
       White_DOT2[i] = 0;
   }//先清零后使用//先清零后使用//先清零后使用//先清零后使用

   if(Single_Bridge_Right == 0 && Single_Bridge_Left == 0 ){
       //左单边桥的识别
              for(uint8 Xsite = LCDW-20;Xsite > 20;Xsite--){
                  for(uint8 Ysite = 55;Ysite > 0;Ysite--){
                      if(Pixel[Ysite][Xsite] == 0){
                          White_DOT1[Xsite] = 55 - Ysite;
                          break;
                      }else if(Ysite == 1){
                          White_DOT1[Xsite] = 55;
                          break;
                      }
                  }
                  //存放最长白列在那一列处
                  if(White_COL_MAX1 <= White_DOT1[Xsite]){
                      White_COL_POS1 = Xsite;
                      White_COL_MAX1 = White_DOT1[Xsite];
                      White_COL_MIN1 = White_DOT1[Xsite - 3];
                  }
              }//计算每一行的最长白列数

          //统计左最长白列的值(左侧)
          for(uint8 Xsite = 20;Xsite < White_COL_POS1; Xsite++){
              if(
                   (MY_ABS((White_DOT1[CLAMP(White_COL_POS1 - 1)] - White_DOT1[CLAMP(Xsite)])) <= 1) && White_DOT1[CLAMP(Xsite)]< 45
                 ){
                  White_COL_Left1 ++;

                  if( White_COL_Left1 >= 10 && length_max > 35){
                      White_COL_Right1 = 3;
                  }
                  else{
                      White_COL_Right1 = 0;
                  }
              }
          }
          //统计左最长白列的值(右侧)
          for(uint8 Xsite = LCDW-20;Xsite >  White_COL_POS1; Xsite--){
              if((MY_ABS(White_DOT1[CLAMP(Xsite)] - White_DOT1[CLAMP(White_COL_POS1)]) <= 1) && White_DOT1[CLAMP(Xsite)] < 48)
              {
                  White_COL_Right1++;
              }
          }
          //判断左单边桥
          if(
                Straight_Right_Varance <= 1 && Straight_Left_Varance   > 1
              &&Straight_Left_Varance1 > 1 && Straight_Right_Varance1  > 1
              &&White_DOT1[CLAMP(White_COL_POS1)] - White_DOT1[CLAMP(White_COL_POS1 - 3)] >= 2 && White_COL_Left1 > 8 && White_COL_Right1 >= 5
//              &&White_COL_Left1 - White_COL_Right1 < 10
              //无边行判断
              &&ImageStatus.Left_Line  < 3
              &&ImageStatus.Right_Line < 3
              //左边丢线
              &&LeftBoundary_Error  < 3
              &&RightBoundary_Error < 3
              &&TrackWidthChangeFlag
              //中线方差
              &&variance_acc < 20
              &&length_max < 48
              &&Left_Straight_Flag < 5
              &&Right_Straight_Flag <5
              ){
              Single_Bridge_Left = 1;
              Obstacle_Encoder = 10;
              Buzzer_Count = 6;
          }
//          else{
//              Single_Bridge_Right = 0;Single_Bridge_Left = 0;
//          }
          Check_Single_SideBridge();
   }

}

uint8 White_COL_MAX2 = 0, White_COL_POS2 = 0 , White_COL_MIN2 = 0;
uint8 White_COL_Left2 = 0 , White_COL_Right2 = 0;
void Check_Bridge_Right(void){

    Check_TrackWidth();

    White_COL_Left2 = 0;  White_COL_Right2 = 0;
    White_COL_MAX2 = 0,   White_COL_POS2 = 0;

    //最长白列数组存放清零
    for(uint8 i = 0;i < LCDW;i++){
        White_DOT1[i] = 0;
        White_DOT2[i] = 0;
    }//先清零后使用//先清零后使用//先清零后使用//先清零后使用

    if(Single_Bridge_Right == 0 && Single_Bridge_Left == 0 ){
        //右单边桥的识别
           //统计左右最长白列的值(左侧)
           for(uint8 Xsite = 20;Xsite < LCDW-20;Xsite++){
               for(uint8 Ysite = 55;Ysite > 0;Ysite--){
                   if(Pixel[Ysite][Xsite] == 0){
                       White_DOT2[Xsite] = 55 - Ysite;
                       break;
                   }else if(Ysite == 1){
                       White_DOT2[Xsite] = 55;
                       break;
                   }
               }
               //存放最长白列在那一列处
               if(White_COL_MAX2 <= White_DOT2[Xsite]){
                   White_COL_POS2 = Xsite;
                   White_COL_MAX2 = White_DOT2[Xsite];
                   White_COL_MIN2 = White_DOT2[Xsite + 3];
               }
           }//计算每一行的最长白列数


           //统计左右最长白列的值(右侧)
           for(uint8 Xsite = LCDW-20;Xsite >  White_COL_POS2; Xsite--){
               if((MY_ABS(White_DOT2[CLAMP(Xsite)] - White_DOT2[CLAMP(White_COL_POS2 + 1)]) <= 1)&& White_DOT2[CLAMP(Xsite)] < 45)
               {
                   White_COL_Right2 ++;
               }

               if(White_COL_Right2 >= 10 && length_max > 35){
                   White_COL_Left2 = 3;
                }
                else {
                    White_COL_Left2 = 0;
               }
           }
           //统计左右最长白列的值(左侧)
           for(uint8 Xsite = 20;Xsite < White_COL_POS2; Xsite++){
               if(
                    MY_ABS((White_DOT2[CLAMP(White_COL_POS2)] - White_DOT2[CLAMP(Xsite)])) <= 1 && White_DOT2[CLAMP(Xsite)] < 48
                  ){
                   White_COL_Left2 ++;
               }
           }
           //判断右单边桥
           if(
                Straight_Left_Varance    <= 1&& Straight_Right_Varance  > 1
                &&Straight_Left_Varance1 > 1 && Straight_Right_Varance1 > 1
              &&White_DOT2[CLAMP(White_COL_POS2)] - White_DOT2[CLAMP(White_COL_POS2 + 3)] >= 2
              && White_COL_Left2 >= 5 && White_COL_Right2 > 8
//              &&White_COL_Right2 - White_COL_Left2    < 10
              //无边行判断
              &&ImageStatus.Left_Line  < 3
              &&ImageStatus.Right_Line < 3
              //左边丢线
              &&LeftBoundary_Error  < 3
              &&RightBoundary_Error < 3
              //中线方差
              &&variance_acc < 20
              &&TrackWidthChangeFlag
              &&length_max < 48

              &&Left_Straight_Flag < 5
              &&Right_Straight_Flag <5
           ){
              Single_Bridge_Right = 1;
              Obstacle_Encoder = 10;
              Buzzer_Count = 6;
           }
//           else{
//               Single_Bridge_Right = 0;Single_Bridge_Left = 0;
//           }
           Check_Single_SideBridge();
    }


}

void Check_Single_SideBridge(void){

    if(
         ImageStatus.Road_type == RightCirque
       ||ImageStatus.Road_type == LeftCirque
       ||Single_SideBridge_Flag == 1){
        return;
    }

    Check_TrackWidth2();

    if(Single_Bridge_Right == 1
     ||Single_Bridge_Left  == 1
||(length_max < 40 && TrackWidthChangeFlag2 == 1 && Right_Straight_Flag< 4 && Left_Straight_Flag < 3 && LeftBoundary_Error  < 3  && RightBoundary_Error < 3 && Road_Width[32] < 29  && Road_Width[33] < 29&&Road_Width[34]< 29 && Road_Width[35] < 30 && Road_Width[36] < 30 && Straight_Left_Varance > 1 && Straight_Right_Varance < 1)
||(length_max < 40 && TrackWidthChangeFlag2 == 1 && Right_Straight_Flag< 4 && Left_Straight_Flag < 3 && LeftBoundary_Error  < 3  && RightBoundary_Error < 3 && Road_Width[32] < 29  && Road_Width[33] < 29&&Road_Width[34] < 29 && Road_Width[35] < 30 && Road_Width[36] < 30 && Straight_Left_Varance < 1 && Straight_Right_Varance > 1)
    )
    {
        Single_SideBridge_Flag  = 1;
        Single_Bridge_Flag_Last = 1;
        Tow_Point = 40;
        Obstacle_Encoder = 10;
        Buzzer_Count = 6;
    }
}


void Check_Single_Bridge(void){
    if(Single_Bridge_Left == 0)
    Check_Bridge_Left();
    if(Single_Bridge_Right == 0)
    Check_Bridge_Right();
}


float Median_Deviation = 0;
int Single_3cmBridge = 1;


float Obstacle_Encoder_OUT = 2500;
float Width_Offest = 10;


void Run_Single_Bridge(void){

    if(Single_3cmBridge){
        int sum = 0;Median_Deviation = 0;
    //    用于判断识别到单边桥的寻线方式（左or右）
        for (Ysite = 55; Ysite >= ImageStatus.OFFLine + 1; Ysite--){
          sum += (ImageDeal[Ysite].Center - (LCDW / 2 - 1));
        }
        Median_Deviation = (float)sum / (54 - ImageStatus.OFFLine);

        if(Median_Deviation > 2){
            for (Ysite = 59; Ysite >= ImageStatus.OFFLine + 1; Ysite--){
              ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].RightBorder - Circle_HalfRoadWidth[Ysite]+ Width_Offest);
            }
            for(Ysite = ImageStatus.OFFLine + 1; Ysite > 0; Ysite--){
                ImageDeal[Ysite].Center = CLAMP(ImageDeal[CLIP_DATA(Ysite + 1)].Center);
            }
        }
        else if(Median_Deviation < -2){
            for (Ysite = 59; Ysite >= ImageStatus.OFFLine + 1; Ysite--){
                ImageDeal[Ysite].Center = CLAMP(ImageDeal[CLIP_DATA(Ysite)].LeftBorder + Circle_HalfRoadWidth[Ysite]-Width_Offest);
            }
            for(Ysite = ImageStatus.OFFLine + 1; Ysite > 0; Ysite--){
                ImageDeal[Ysite].Center = CLAMP(ImageDeal[ CLIP_DATA(Ysite + 1)].Center);
            }
        }
    }

    //单边桥状态退出
    if(Obstacle_Encoder >= Obstacle_Encoder_OUT){
        //清除单边桥标志位
        Single_SideBridge_Flag = 0;

        Single_Bridge_Right = 0;
        Single_Bridge_Left = 0;


        Obstacle_Encoder = 0;
        //打开舵机控制标志位
        Servo_Control_Flag = 1;
        //恢复前瞻值
        Tow_Point = 30;
        //开始滚转方向平衡控制//未使用
        STOP_FLAG = 0;
    }
}
//单边桥舵机增量
float p=0,p1=0;

void ROLL_Adapt(float stab_roll)
{
  float L_Height =0;
  float R_Height =0;

   L_Height = 75 + stab_roll;
   R_Height = 75 - stab_roll;

  if(L_Height<35)L_Height=35;
  if(L_Height>75)L_Height=75;

  if(R_Height<35)R_Height=35;
  if(R_Height>75)R_Height=75;


  IKParam.XLeft = 21;
  IKParam.XRight = 21;

  IKParam.YLeft  = L_Height;
  IKParam.YRight = R_Height;

  inverseKinematics();
}

//void Roll_control(void){
//    //单边桥伸缩
//    p = Roll_PID(&Roll,0,IMU660RA_Parameter.Roll);
//    //滤波
//    if(IMU660RA_Parameter.Roll <  - 0.5 && IMU660RA_Parameter.Roll > 0.5)  p=0;
//    p1=p1+p;
//    if(p1 > 45)  p1=45;
//    if(p1 <-45)  p1=-45;
//    ROLL_Adapt(p1);
//}



