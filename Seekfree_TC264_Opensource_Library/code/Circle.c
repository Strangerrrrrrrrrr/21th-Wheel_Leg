/*
 * Circle.c
 *
 *  Created on: 2025年3月22日
 *      Author: 不懂
 */
#include "Circle.h"
int Circle_Flag = 0;

int Left_RingsFlag_Point1_Ysite, Left_RingsFlag_Point2_Ysite;   //左圆环判断的两点纵坐标
int Right_RingsFlag_Point1_Ysite, Right_RingsFlag_Point2_Ysite; //右圆环判断的两点纵坐标

uint8 Ring_Help_Flag = 0;                      //进环辅助标志

ImageFlagtypedef ImageFlag;

int Point_Xsite,Point_Ysite;                   //拐点横纵坐标
int Repair_Point_Xsite,Repair_Point_Ysite;     //补线点横纵坐标


uint8 Half_Road_Wide[60] =                      //直道赛道半宽
{ 0, 0, 0, 5, 7, 8, 9, 10,10,10,
11,11,12,12,12,13,13,14,14,15,
15,15,15,15,15,16,16,17,17,17,
18,18,19,19,20,20,20,21,21,22,
23,23,23,24,24,25,25,25,30,30,
32,32,32,32,33,33,33,33,33,33,
};
//弯道赛道半宽
uint8 Half_Bend_Wide[60] =
{

        33,33,33,33,33,33,33,33,33,33,
        33,33,32,32,30,30,29,29,28,27,
        28,27,27,26,26,25,25,24,24,23,
        22,21,21,22,22,22,23,24,24,24,
        25,25,25,26,26,26,27,27,28,28,
        28,29,29,30,30,31,31,32,32,33,
};
//进环寻找单边数组（赛道宽度）
uint8 Circle_HalfRoadWidth[60]=
{
        10,10,10,10,10,10,10,10,10,10,
        9, 9, 9,10,10,10,11,12,12,12,
        13,13,14,15,15,16,17,17,18,19,
        20,20,20,21,21,22,23,24,24,25,
        25,26,26,29,29,30,30,30,30,30,
        31,31,31,31,31,32,32,32,32,32,
};
//环内单边巡线数组（赛道宽度）
uint8 Ciecle_BendWidth[60]={
        4, 4, 4, 4, 4, 5, 5, 6, 6, 6,
        7, 7, 8, 8,9, 10,10,10,10,10,
        11,11,11,11,11,15,15,16,17,17,
        17,18,18,19,20,20,20,21,21,22,
        22,22,22,22,23,23,24,24,25,25,
        26,26,26,26,26,27,27,29,29,29,
};

uint8 Left_Circle_Lost = 0, Right_Circle_Lost;
uint8 Left_line1 = 0,Left_line2 = 0;


uint8 Circle_State = 0;
void Element_Judgment_Left_Rings()
{
    if(
      //左右边无边行
      ImageStatus.Left_Line < 13
    ||ImageStatus.Right_Line > 5
    //左边丢线
    || LeftBoundary_Error  < 20
    || RightBoundary_Error > 5

    ||variance_acc > 75

    ||(Right_Straight_Flag < 2)

    ||ImageStatus.OFFLine > 20        //截至行要求
    //两边无边行条件满足
    || ImageStatus.WhiteLine > 5       //两边丢线判断

    //底边行必须存在边线
    || ImageDeal[52].IsLeftFind == 'W'
    || ImageDeal[53].IsLeftFind == 'W'
    || ImageDeal[54].IsLeftFind == 'W'
    )return;

    int ring_ysite = 20;
    //四领域（八领域）辅助角点变量//先清零后使用
    Left_RingsFlag_Point1_Ysite = 0;Left_RingsFlag_Point2_Ysite = 0; Ring_Help_Flag = 0;
    //左圆环坐下角点判断//循环条件避免了数组越界，所以在这里不作限幅
    for (int Ysite = 59; Ysite > ring_ysite; Ysite--)
    {
        if(ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite - 4].LeftBoundary > 7)
        {
            Left_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }

    //有时候看不到边线跳跃，换一种找拐点的方式//此时可能边线是撕裂的导致连续点间的间距较小，满足不了上面边线撕裂条件//从而找不到拐点
    if(Left_RingsFlag_Point1_Ysite == 0){
        for(uint8 Ysite = 59 ; Ysite > ring_ysite;Ysite--){
            if(
                //连续四个点间距满足条件
                ImageDeal[Ysite].LeftBoundary   - ImageDeal[Ysite-1].LeftBoundary > 2
              &&ImageDeal[Ysite-1].LeftBoundary - ImageDeal[Ysite-2].LeftBoundary > 2
              &&ImageDeal[Ysite-2].LeftBoundary - ImageDeal[Ysite-3].LeftBoundary > 2
            ){
                Left_RingsFlag_Point1_Ysite = Ysite;
                break;
            }
        }
    }
    //边线断裂也认为是存在拐点
    if(Left_RingsFlag_Point1_Ysite == 0){
        for(uint8 Ysite = 59 ; Ysite > ring_ysite;Ysite--){
            if(
                //连续跳变满足要求
                ImageDeal[Ysite-3].LeftBoundary   <= ImageDeal[Ysite].LeftBoundary_First - 1
                &&ImageDeal[Ysite-4].LeftBoundary <= ImageDeal[Ysite].LeftBoundary_First - 1
                &&ImageDeal[Ysite-5].LeftBoundary <= ImageDeal[Ysite].LeftBoundary_First - 1
                &&ImageDeal[Ysite-6].LeftBoundary <= ImageDeal[Ysite].LeftBoundary_First - 1
                &&ImageDeal[Ysite-7].LeftBoundary <= ImageDeal[Ysite].LeftBoundary_First - 1
            ){
                Left_RingsFlag_Point1_Ysite = Ysite;
                break;
            }
        }
    }

    //右圆环  圆弧 判断
    for (int Ysite = Left_RingsFlag_Point1_Ysite; Ysite > ImageStatus.OFFLine; Ysite--)
    {
        if (   ImageDeal[CLIP_DATA(Ysite + 6)].LeftBorder < ImageDeal[CLIP_DATA(Ysite + 3)].LeftBorder
            && ImageDeal[CLIP_DATA(Ysite + 5)].LeftBorder < ImageDeal[CLIP_DATA(Ysite + 3)].LeftBorder
            && ImageDeal[CLIP_DATA(Ysite + 3)].LeftBorder > ImageDeal[CLIP_DATA(Ysite + 2)].LeftBorder
            && ImageDeal[CLIP_DATA(Ysite + 3)].LeftBorder > ImageDeal[CLIP_DATA(Ysite + 1)].LeftBorder
            )
        {
            Ring_Help_Flag = 1;
            break;
        }
    }

    //正常角点的上方有连续五行的无边行
    for(uint8 Ysite = (uint8)Left_RingsFlag_Point1_Ysite;Ysite > 10;Ysite--){
        if(
           ImageDeal[Left_RingsFlag_Point1_Ysite - 3].IsLeftFind == 'W'
         &&ImageDeal[Left_RingsFlag_Point1_Ysite - 4].IsLeftFind == 'W'
         &&ImageDeal[Left_RingsFlag_Point1_Ysite - 5].IsLeftFind == 'W'
         &&ImageDeal[Left_RingsFlag_Point1_Ysite - 6].IsLeftFind == 'W'
         &&ImageDeal[Left_RingsFlag_Point1_Ysite - 7].IsLeftFind == 'W'
        ){
            Ring_Help_Flag = 1;
            break;
        }
    }

    //如果没有正常找到无边行找丢线点数满足条件
    if(Left_RingsFlag_Point1_Ysite && ImageStatus.Left_Line > 25 && ImageStatus.WhiteLine < 8){
        Ring_Help_Flag = 1;
    }
    //满足左圆环要求
    if (ImageDeal[Left_RingsFlag_Point1_Ysite].LeftBoundary > 2  //角点坐标范围满足要求
       &&Left_RingsFlag_Point1_Ysite > 21                        //找到角点
       &&Ring_Help_Flag == 1                                     //圆环辅助标志位
       &&ImageStatus.Left_Line >= 15                             //环岛状态不识别环岛
       &&ImageFlag.image_element_rings_flag == 0
       &&Circle_State == 0)
    {
        ImageFlag.image_element_rings = 1;                       //圆环类型标志位
        ImageFlag.image_element_rings_flag = 1;                  //圆环阶段标志位
        ImageFlag.ring_big_small=1;//（没用）
        Circle_State = 1;
        Buzzer_Count = 6;
        ImageStatus.Road_type = LeftCirque;                      //圆环类型标志位


        if(g_attitude.yaw + 100 >= 180)
            IMU660RA_Parameter.Yaw = g_attitude.yaw + 100 -360.0;   //入环初始角度
        else
            IMU660RA_Parameter.Yaw = g_attitude.yaw + 100;
    }
}

void Element_Judgment_Right_Rings()
{
    //不满足环岛基本特征直接跳出，不检测圆环
    if (
         //左右无边行满足条件
          ImageStatus.Left_Line > 5
        ||ImageStatus.Right_Line < 13
        //丢线满足条件
        ||RightBoundary_Error < 20
        ||LeftBoundary_Error > 5
        ||variance_acc > 70
        //截至行满足要求
        || ImageStatus.OFFLine > 20
        //左边长直道满足要求
        ||(Left_Straight_Flag < 3 )
        //两边丢线
        || ImageStatus.WhiteLine > 5
        //底边几行不能是无边行
        || ImageDeal[52].IsRightFind == 'W'
        || ImageDeal[53].IsRightFind == 'W'
        || ImageDeal[54].IsRightFind == 'W'
        ){
        return;
    }
    //四领域（八领域）辅助角点变量//先清零后使用
    int ring_ysite = 20;
    Right_RingsFlag_Point1_Ysite = 0;Right_RingsFlag_Point2_Ysite = 0;Ring_Help_Flag = 0;

    //角点判断，边线不连续//循环条件避免了数组越界，所以在这里不作限幅
    for (int Ysite = 59; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite - 4].RightBoundary - ImageDeal[Ysite].RightBoundary_First > 7)
        {
            Right_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }

    //有时候看不到边线跳跃，换一种找拐点的方式//此时可能边线是撕裂的导致连续点间的间距较小，满足不了上面边线撕裂条件//从而找不到拐点
    if(Right_RingsFlag_Point1_Ysite == 0){
        for(uint8 Ysite = 59 ; Ysite > ring_ysite;Ysite--){
            if(
                //连续四个点间距满足条件
                ImageDeal[Ysite - 1].RightBoundary - ImageDeal[Ysite].RightBoundary   > 2
              &&ImageDeal[Ysite - 2].RightBoundary - ImageDeal[Ysite-1].RightBoundary > 2
              &&ImageDeal[Ysite - 3].RightBoundary - ImageDeal[Ysite-2].RightBoundary > 2
            ){
                Right_RingsFlag_Point1_Ysite = Ysite;
                break;
            }
        }
    }
    //边线断裂也认为是存在拐点
    if(Right_RingsFlag_Point1_Ysite == 0){
        for(uint8 Ysite = 59 ; Ysite > ring_ysite;Ysite--){
            if(
                //连续跳变满足要求
                ImageDeal[Ysite-3].RightBoundary   >= ImageDeal[Ysite].RightBoundary_First + 1
                &&ImageDeal[Ysite-4].RightBoundary >= ImageDeal[Ysite].RightBoundary_First + 1
                &&ImageDeal[Ysite-5].RightBoundary >= ImageDeal[Ysite].RightBoundary_First + 1
                &&ImageDeal[Ysite-6].RightBoundary >= ImageDeal[Ysite].RightBoundary_First + 1
                &&ImageDeal[Ysite-7].RightBoundary >= ImageDeal[Ysite].RightBoundary_First + 1
            ){
                Left_RingsFlag_Point1_Ysite = Ysite;
                break;
            }
        }
    }

    //右圆环  圆弧 判断
    for (int Ysite = Right_RingsFlag_Point1_Ysite; Ysite > ImageStatus.OFFLine; Ysite--)
    {
        //对数组访问一定要加限制防止数组越界卡死
        if (   ImageDeal[CLIP_DATA(Ysite + 6)].RightBorder > ImageDeal[CLIP_DATA(Ysite + 3)].RightBorder
            && ImageDeal[CLIP_DATA(Ysite + 5)].RightBorder > ImageDeal[CLIP_DATA(Ysite + 3)].RightBorder
            && ImageDeal[CLIP_DATA(Ysite + 3)].RightBorder < ImageDeal[CLIP_DATA(Ysite + 2)].RightBorder
            && ImageDeal[CLIP_DATA(Ysite + 3)].RightBorder < ImageDeal[CLIP_DATA(Ysite + 1)].RightBorder
           )
        {
            Ring_Help_Flag = 1;
            break;
        }
    }

    //正常角点的上方有连续五行的无边行
    for(uint8 Ysite = (uint8)Right_RingsFlag_Point1_Ysite;Ysite > 10;Ysite--){
        if(
           ImageDeal[Right_RingsFlag_Point1_Ysite - 3].IsRightFind == 'W'
         &&ImageDeal[Right_RingsFlag_Point1_Ysite - 4].IsRightFind == 'W'
         &&ImageDeal[Right_RingsFlag_Point1_Ysite - 5].IsRightFind == 'W'
         &&ImageDeal[Right_RingsFlag_Point1_Ysite - 6].IsRightFind == 'W'
         &&ImageDeal[Right_RingsFlag_Point1_Ysite - 7].IsRightFind == 'W'
        ){
            Ring_Help_Flag = 1;
            break;
        }
    }
    //如果没有正常找到无边行找丢线点数满足条件
    if(Right_RingsFlag_Point1_Ysite && ImageStatus.WhiteLine < 8 && ImageStatus.Right_Line > 25){
        Ring_Help_Flag = 1;
    }

    if(
          ImageDeal[Right_RingsFlag_Point1_Ysite].RightBoundary < LCDW - 2
        &&Right_RingsFlag_Point1_Ysite > 21
        &&ImageStatus.Right_Line >= 15
        &&Ring_Help_Flag == 1
        &&ImageFlag.image_element_rings_flag == 0
        &&Circle_State == 0)
    {
        ImageFlag.image_element_rings = 2;
        ImageFlag.image_element_rings_flag = 1;
        ImageFlag.ring_big_small=1;

        Circle_State = 1;
        //入环初始角度
        if(g_attitude.yaw - 100 <= -180)
           IMU660RA_Parameter.Yaw = g_attitude.yaw - 100 + 360.0;
        else
           IMU660RA_Parameter.Yaw = g_attitude.yaw - 100;

        Buzzer_Count = 6;
        ImageStatus.Road_type = RightCirque;
    }
}

uint8 Number = 0;
float Road_Width_L = 1,Road_Width_R = 5;

float Left_OUTNUM = 5,Right_OUTNUM = 5;
void Element_Handle_Left_Rings()
{
    /***************************************判断**************************************/
    //陀螺仪数值//判断状态出环
    if(Circle_State == 1){
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw)<10.0)
        {
            Circle_State = 2;
            if(g_attitude.yaw + 270 >=180)
                IMU660RA_Parameter.Yaw = g_attitude.yaw + 270 -360.0;
            else
                IMU660RA_Parameter.Yaw = g_attitude.yaw + 270;
        }
    }
    if(Circle_State == 2){
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw)<10.0){
            Circle_State = 3;
            if(g_attitude.yaw + 280 >=180)
                IMU660RA_Parameter.Yaw = g_attitude.yaw + 280 - 360.0;
            else
                IMU660RA_Parameter.Yaw = g_attitude.yaw + 280;
        }
    }
    if(Circle_State == 3){
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw) < 10){
            Circle_State = 4;
        }
    }

    int num = 0;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {//循环条件避免了数组越界，所以在这里不作限幅
        if(ImageDeal[Ysite].IsLeftFind == 'W')
            num++;
        if(    ImageDeal[Ysite+3].IsLeftFind == 'W' && ImageDeal[Ysite+2].IsLeftFind == 'W'
            && ImageDeal[Ysite+1].IsLeftFind == 'W' && ImageDeal[Ysite].IsLeftFind == 'T')
            break;
    }
    Number = (uint8)num;
    //准备进环
    if (ImageFlag.image_element_rings_flag == 1 && num > 13 && LeftBoundary_Error > 20)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num < 8 && LeftBoundary_Error < 16)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
    //进环
    if(ImageFlag.image_element_rings_flag == 5 &&ImageStatus.Right_Line > 5)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
    //进环（陀螺仪数值满足要求直接循内线入环）
    if(Circle_State == 2 && ImageFlag.image_element_rings_flag == 6){
        ImageFlag.image_element_rings_flag = 8;
    }
     //环内
    if(ImageFlag.image_element_rings_flag == 6 && ImageStatus.Right_Line < 4)
    {
        ImageFlag.image_element_rings_flag = 7;
    }
    //环内 大圆环判断
    if(ImageFlag.image_element_rings_flag == 7)
    {
        Point_Ysite = 0;
        Point_Xsite = 0;
        //这里访问数组不会出现越界，所以不加限幅
        for (int Ysite = 50; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (       ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 2].RightBorder
                    && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 2].RightBorder
                    && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 1].RightBorder
                    && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 1].RightBorder
                    && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 3].RightBorder
                    && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 3].RightBorder
               )
            {
                Point_Xsite = ImageDeal[Ysite].RightBorder;
                Point_Ysite = Ysite;
                break;
            }
        }
        if (Point_Ysite > 20)
        {
            ImageFlag.image_element_rings_flag = 8;
        }
        else if(ImageDeal[18].RightBoundary_First - ImageDeal[18].LeftBoundary_First > 70)
        {
            ImageFlag.image_element_rings_flag = 8;
        }
    }
    //准备出环
        if (ImageFlag.image_element_rings_flag == 8)
        {
             if (
                  //小车转过的角度满足要求
                  Circle_State == 4
                  //两边丢线数少
                  &&ImageStatus.WhiteLine < 5
                  //截止行（前瞻值）很小
                  &&ImageStatus.OFFLine < 17
                 )
               {
                 ImageFlag.image_element_rings_flag = 9;
               }
        }

        //结束圆环进程
        if (ImageFlag.image_element_rings_flag == 9)
        {
            int num=0;
            for (int Ysite = 45; Ysite > 8; Ysite--)
            {
                if(ImageDeal[Ysite].IsLeftFind == 'W' )
                    num++;
                wubianhang = num;
            }
            if(num < Left_OUTNUM)
            {
                //出环处理完道路类型清0
                ImageStatus.Road_type = 0;
                ImageFlag.image_element_rings_flag = 0;
                ImageFlag.image_element_rings = 0;
                ImageFlag.ring_big_small = 0;
                Circle_State = 0;
            }
    }
    /***************************************处理**************************************/
        //准备进环  半宽处理
    if (   ImageFlag.image_element_rings_flag == 1
        || ImageFlag.image_element_rings_flag == 2
        || ImageFlag.image_element_rings_flag == 3
        || ImageFlag.image_element_rings_flag == 4)
    {
        for (int Ysite = 57; Ysite > ImageStatus.OFFLine; Ysite--)
        {
//            ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].RightBorder - Half_Road_Wide[Ysite]);
            ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].RightBorder - Circle_HalfRoadWidth[Ysite] -7);

        }
    }
        //进环  补线
    if  ( ImageFlag.image_element_rings_flag == 5
        ||ImageFlag.image_element_rings_flag == 6
        )
    {
        int flag_Xsite_1=0;
        int flag_Ysite_1=0;
        float Slope_Rings=0;
        for(Ysite=55;Ysite>ImageStatus.OFFLine;Ysite--)
        {
            for(Xsite=ImageDeal[Ysite].LeftBorder + 1;Xsite<ImageDeal[Ysite].RightBorder - 1;Xsite++)
            {
                if(Pixel[Ysite][Xsite] == 255 && Pixel[Ysite][Xsite + 1] == 0)
                 {
                   flag_Ysite_1 = Ysite;
                   flag_Xsite_1 = Xsite;

                   //可以更改所补直线的斜率//更改与固定点的拉线（79，59）
                   Slope_Rings=(float)(79-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                   break;
                 }
            }
            if(flag_Ysite_1 != 0)
            {
                break;
            }
        }
        if(flag_Ysite_1 == 0)
        {
            for(Ysite=ImageStatus.OFFLine+1;Ysite<30;Ysite++)
            {
                if(ImageDeal[Ysite].IsLeftFind=='T'&&ImageDeal[Ysite+1].IsLeftFind=='T'&&ImageDeal[Ysite+2].IsLeftFind=='W'
                    &&abs(ImageDeal[Ysite].LeftBorder-ImageDeal[Ysite+2].LeftBorder)>10
                  )
                {
                    flag_Ysite_1=Ysite;
                    flag_Xsite_1=ImageDeal[flag_Ysite_1].LeftBorder;
                    ImageStatus.OFFLine=(uint8)Ysite;

                    //可以更改所补直线的斜率//更改与固定点的拉线（79，59）
                    Slope_Rings=(float)(79-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                    break;
                }

            }
        }
        //补线
        if(flag_Ysite_1 != 0)
        {
            //可以更改所补直线的需要改变边线的大小//for循环条件
            for(Ysite=flag_Ysite_1;Ysite < LCDH;Ysite++)
            {
                ImageDeal[Ysite].RightBorder=flag_Xsite_1+Slope_Rings*(Ysite-flag_Ysite_1);
                    ImageDeal[Ysite].Center = CLAMP(((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder)/2 ));
                if(ImageDeal[Ysite].Center<4)
                    ImageDeal[Ysite].Center = 4;
            }
            ImageDeal[flag_Ysite_1].RightBorder=flag_Xsite_1;
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A点上方进行扫线
            {
                for(Xsite=ImageDeal[Ysite+1].RightBorder-10;Xsite<ImageDeal[Ysite+1].RightBorder+2;Xsite++)
                {
                    if(Pixel[Ysite][Xsite]==255 && Pixel[Ysite][Xsite+1]==0)
                    {
                        ImageDeal[Ysite].RightBorder=Xsite;
                            ImageDeal[Ysite].Center = CLAMP(((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder)/2));
                        if(ImageDeal[Ysite].Center<4)
                            ImageDeal[Ysite].Center = 4;
                        ImageDeal[Ysite].Wide=ImageDeal[Ysite].RightBorder-ImageDeal[Ysite].LeftBorder;
                        break;
                    }
                }
                if(ImageDeal[Ysite].Wide > 8 &&ImageDeal[Ysite].RightBorder< ImageDeal[Ysite+2].RightBorder)
                {
                    continue;
                }
                else
                {
                    ImageStatus.OFFLine=Ysite+2;
                    break;
                }
            }
        }
    }
     //环内 小环弯道减半宽 大环不减
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
        //大圆环出环 补线
    if (ImageFlag.image_element_rings_flag == 8 && ImageFlag.ring_big_small == 1)    //大圆环
    {
        Repair_Point_Ysite = 7;
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //补线
        {
            ImageDeal[Ysite].RightBorder = CLAMP(ImageDeal[Ysite].LeftBorder + Ciecle_BendWidth[Ysite] + 25);
            if(ImageDeal[Ysite].RightBorder>79){ImageDeal[Ysite].RightBorder = 79;}
            ImageDeal[Ysite].Center = CLAMP(((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2));
        }
    }
    //已出环 半宽处理
    if (ImageFlag.image_element_rings_flag == 9 || ImageFlag.image_element_rings_flag == 10)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].RightBorder - Circle_HalfRoadWidth[Ysite] + 5);
        }
    }
}

int wubianhang = 0;

void Element_Handle_Right_Rings()
{
    /****************判断*****************/
    if(Circle_State == 1){
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw)<10.0){
            Circle_State = 2;
            if(g_attitude.yaw - 280 <= -180)
               IMU660RA_Parameter.Yaw = g_attitude.yaw - 280 +360.0;
            else
               IMU660RA_Parameter.Yaw = g_attitude.yaw - 280;
        }
    }
    if(Circle_State == 2) {
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw) < 10.0){
            Circle_State = 3;

            if(g_attitude.yaw - 280 <= -180)
              IMU660RA_Parameter.Yaw = g_attitude.yaw - 280 + 360.0;
            else
              IMU660RA_Parameter.Yaw = g_attitude.yaw - 280;
        }
    }
    if(Circle_State == 3){
        if(MY_ABS(g_attitude.yaw - IMU660RA_Parameter.Yaw)< 10.0){
            Circle_State = 4;
        }
    }

    int num =0 ;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {
        if(ImageDeal[Ysite].IsRightFind == 'W')
        {
            num++;
        }
        if(    ImageDeal[Ysite+3].IsRightFind == 'W' && ImageDeal[Ysite+2].IsRightFind == 'W'
            && ImageDeal[Ysite+1].IsRightFind == 'W' && ImageDeal[Ysite].IsRightFind == 'T' )
            break;
    }

    Number = (uint8)num;
        //准备进环
    if (ImageFlag.image_element_rings_flag == 1 && num > 15 && RightBoundary_Error > 20)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num < 8 && RightBoundary_Error < 15)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
        //进环
    if(ImageFlag.image_element_rings_flag == 5 && ImageStatus.Left_Line > 5)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
    //进环（陀螺仪数值满足要求直接循内线入环）
    if(Circle_State == 2 && ImageFlag.image_element_rings_flag == 6){
        ImageFlag.image_element_rings_flag = 8;
    }
    //进环小圆环
    if(ImageFlag.image_element_rings_flag == 6 && ImageStatus.Left_Line<4)
    {
        ImageFlag.image_element_rings_flag = 7;
    }
    if (ImageFlag.image_element_rings_flag == 7)
    {
        Point_Xsite = 0;
        Point_Ysite = 0;
        for (int Ysite = 55; Ysite > ImageStatus.OFFLine + 4; Ysite--)
        {
            if (    ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 2].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 2].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 1].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 1].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 4].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 4].LeftBorder
                )
            {
                 Point_Xsite = ImageDeal[Ysite].LeftBorder;
                 Point_Ysite = Ysite;
                 break;
            }
        }
        if (Point_Ysite > 18)
        {
            ImageFlag.image_element_rings_flag = 8;
        }
        else if(ImageDeal[18].RightBoundary_First - ImageDeal[18].LeftBoundary_First > 70)
        {
            ImageFlag.image_element_rings_flag = 8;
        }
    }
    if(ImageFlag.image_element_rings_flag == 8)
    {
         if (
               //小车转过的角度满足要求
               Circle_State == 4
             &&ImageStatus.WhiteLine < 5
                 //截止行（前瞻值）很小
             &&ImageStatus.OFFLine < 20
             )
            {ImageFlag.image_element_rings_flag = 9;}
    }
    if(ImageFlag.image_element_rings_flag == 9)
    {
        int num=0;
        for (int Ysite = 45; Ysite > 10; Ysite--)
        {
            if(ImageDeal[Ysite].IsRightFind == 'W' )
            {
                num++;
                wubianhang = num;
            }
        }
        if(num < Right_OUTNUM)
        {
            //出环处理完道路类型清0
            ImageStatus.Road_type = 0;
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
            Circle_State = 0;
        }
    }
    /***************************************处理**************************************/
         //准备进环  半宽处理
    if (   ImageFlag.image_element_rings_flag == 1
        || ImageFlag.image_element_rings_flag == 2
        || ImageFlag.image_element_rings_flag == 3
        || ImageFlag.image_element_rings_flag == 4)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
//            ImageDeal[Ysite].Center = ImageDeal[Ysite].LeftBorder + Half_Road_Wide[Ysite];
            ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].LeftBorder + Circle_HalfRoadWidth[Ysite] -4);//右边圆环dai'n
        }
    }
        //进环  补线
    if (   ImageFlag.image_element_rings_flag == 5
        || ImageFlag.image_element_rings_flag == 6
       )
    {
        int flag_Xsite_1=0;
        int flag_Ysite_1=0;
        float Slope_Right_Rings = 0;
        for(Ysite=55;Ysite>ImageStatus.OFFLine;Ysite--)
        {
            for(Xsite=ImageDeal[Ysite].LeftBorder + 1;Xsite<ImageDeal[Ysite].RightBorder - 1;Xsite++)
            {
                if(Pixel[Ysite][Xsite]== 255 && Pixel[Ysite][Xsite+1]==0)
                {
                    flag_Ysite_1=Ysite;
                    flag_Xsite_1=Xsite;
                    Slope_Right_Rings=(float)(0-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                    break;
                }
            }
            if(flag_Ysite_1!=0)
            {
              break;
            }
        }
        if(flag_Ysite_1==0)
        {
        for(Ysite=ImageStatus.OFFLine+5;Ysite<30;Ysite++)
        {
         if(ImageDeal[Ysite].IsRightFind=='T'&&ImageDeal[Ysite+1].IsRightFind=='T'&&ImageDeal[Ysite+2].IsRightFind=='W'
               &&abs(ImageDeal[Ysite].RightBorder-ImageDeal[Ysite+2].RightBorder)>10
         )
         {
             flag_Ysite_1=Ysite;
             flag_Xsite_1=ImageDeal[flag_Ysite_1].RightBorder;
             ImageStatus.OFFLine=(uint8)Ysite;
             Slope_Right_Rings=(float)(0-flag_Xsite_1)/(float)(59-flag_Ysite_1);
             break;
         }

        }

        }
        //补线
        if(flag_Ysite_1!=0)
        {
            for(Ysite=flag_Ysite_1;Ysite<58;Ysite++)
            {
                ImageDeal[Ysite].LeftBorder=flag_Xsite_1+Slope_Right_Rings*(Ysite-flag_Ysite_1);
//                if(ImageFlag.ring_big_small==2)//小圆环加半宽
//                    ImageDeal[Ysite].Center=ImageDeal[Ysite].LeftBorder+Half_Bend_Wide[Ysite];//板块
//                else//大圆环不加半宽
                    ImageDeal[Ysite].Center= CLAMP((ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2 -5);//板块
                if(ImageDeal[Ysite].Center>79)
                    ImageDeal[Ysite].Center=79;
            }
            ImageDeal[flag_Ysite_1].LeftBorder=flag_Xsite_1;
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A点上方进行扫线
            {
                for(Xsite=ImageDeal[Ysite+1].LeftBorder+8;Xsite>ImageDeal[Ysite+1].LeftBorder-4;Xsite--)
                {
                    if(Pixel[Ysite][Xsite]==255 && Pixel[Ysite][Xsite-1]==0)
                    {
                     ImageDeal[Ysite].LeftBorder=Xsite;
                     ImageDeal[Ysite].Wide=ImageDeal[Ysite].RightBorder-ImageDeal[Ysite].LeftBorder;
//                     if(ImageFlag.ring_big_small==2)//小圆环加半宽
//                         ImageDeal[Ysite].Center=ImageDeal[Ysite].LeftBorder+Half_Bend_Wide[Ysite];//板块
//                     else//大圆环不加半宽
                         ImageDeal[Ysite].Center=CLAMP((ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2);
                     if(ImageDeal[Ysite].Center>79)
                         ImageDeal[Ysite].Center=79;
                     if(ImageDeal[Ysite].Center<5)
                         ImageDeal[Ysite].Center=5;
                     break;
                    }
                }
                if(ImageDeal[Ysite].Wide>8 && ImageDeal[Ysite].LeftBorder >  ImageDeal[Ysite+2].LeftBorder)
                {
                    continue;
                }
                else
                {
                    ImageStatus.OFFLine=Ysite+2;
                    break;
                }
            }
        }
    }
        //环内不处理
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
        //大圆环出环 补线
    if (ImageFlag.image_element_rings_flag == 8)  //大圆环
    {
        Repair_Point_Ysite = 7;
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)
        {
            ImageDeal[Ysite].LeftBorder = CLAMP(ImageDeal[Ysite].RightBorder - Ciecle_BendWidth[Ysite] - 16) ;
            if(ImageDeal[Ysite].LeftBorder < 3){ImageDeal[Ysite].LeftBorder = 3;}
            ImageDeal[Ysite].Center = CLAMP((ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2);
        }
    }
        //已出环 半宽处理
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = CLAMP(ImageDeal[Ysite].LeftBorder + Half_Road_Wide[Ysite] - 5);
        }
    }
}

//检测圆环
void Check_Circle(void){
    if (
        ImageStatus.Road_type != RightCirque
      &&ImageStatus.Road_type != LeftCirque
       )
    {
          if(Circle_Flag == 0)
          {
              Element_Judgment_Left_Rings();           //左圆环检测
              Element_Judgment_Right_Rings();          //右圆环检测
          }
          else if(Circle_Flag == 1)
          {
              Element_Judgment_Left_Rings();           //左圆环检测
          }
          else if(Circle_Flag == 2)
          {
              Element_Judgment_Right_Rings();          //右圆环检测
          }
      }
}

//处理圆环
void Run_Circle(void){

    //左圆环处理
    if (ImageFlag.image_element_rings == 1)
        Element_Handle_Left_Rings();
    //右圆环处理
    else if(ImageFlag.image_element_rings == 2)
        Element_Handle_Right_Rings();

}

