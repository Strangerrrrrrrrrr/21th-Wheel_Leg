/*  +++++++++++++++
 * Zebra_Crossingg.c
 *
 *  Created on: 2025年4月11日
 *      Author: 榴星宇
 */
#include "Zebra_Crossing.h"

uint8 Zebra_Flag = 0;
uint8 Zebra_Flag_Last = 0;
uint8 Zebra_Flag_Last_TIME = 0;
uint8 Zebra_Stop_Flag = 0;

//利用原图判断斑马线
void Check_Zebra_Crossing(void){
    uint8 zebra_jump = 0; Zebra_Flag = 0;
        for(int j = 5;j <= 70;j++){
            for(int i = 50;i < 140;i++){
                if(mt9v03x_image[j][i]   > ThreShold_Value + 10&& mt9v03x_image[j][i+1] > ThreShold_Value + 10
                && mt9v03x_image[j][i+2] < ThreShold_Value - 10&& mt9v03x_image[j][i+3] < ThreShold_Value - 10){
                    zebra_jump++;
                }
            }
            if(zebra_jump >= 5){
                Zebra_Flag = 1;
                Zebra_Flag_Last = 1;

                //识别到斑马线蜂鸣器开始鸣叫
                Buzzer_Count = 6;

                if(Zebra_Time_Flag == 1 ){
                    Zebra_Stop_Flag = 1;
                    Zebra_Encoder = 10;

                    //满足后直接清零
                    zebra_jump = 0;
                    break;
                }
            }
            else{
                zebra_jump = 0;
                Zebra_Flag = 0;
            }
        }
}

//利用二值化后的图像判断斑马线
//void check_zebra_crossing(void){
//    static uint8 zebra_jump = 0;
//    for (int Ysite = 20; Ysite < 33; Ysite++){
//        for (int Xsite =ImageDeal[Ysite].LeftBoundary + 2; Xsite < ImageDeal[Ysite].RightBoundary - 2; Xsite++){
//
//            if (Pixel[Ysite][Xsite] == 0 && Pixel[Ysite][Xsite + 1] != 0){
//                zebra_jump++;
//            }
//        }
//        if(zebra_jump >= 7){
//           Zebra_Flag = 1;
//
//           //识别到斑马线蜂鸣器开始鸣叫
//           Buzzer_Count = 6;
//
//           if(Zebra_Time_Flag == 1 ){
//              Zebra_Stop_Flag = 1;
//              Zebra_Encoder = 10;

                  //满足后直接清零
//              zebra_jump = 0;
//              break;
//             }
//        }
//        else{
//            zebra_jump = 0;
//       }
//    }
//}

