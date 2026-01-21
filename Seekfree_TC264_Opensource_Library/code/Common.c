/*
 * Common.c
 *
 *  Created on: 2025年3月18日
 *      Author: 不懂
 */
#include "Common.h"

uint16 Zebra_Time = 0;
uint8 Zebra_Time_Flag = 0;
uint8 num1 = 0;


int JUMP_START_FLAG = 1;

//蜂鸣器鸣叫时长，例子：若蜂鸣器计数为5 则蜂鸣器鸣叫 5 * 5  = 25 ms
uint16 Buzzer_Count = 0;

int Information_Flag = 0,Image_Flag = 1;

void Parameter_Init(void){

     Angle_KP = Angle_KP_Temp;//主循环恢复参数

        //计时清零
        if(TIME >= 900){

            Servo_Control_Flag = 1;//重新打开舵机控制标志位

            TIME = 0;         //计时变量
            JUMP_FLAG = 0;    //跳跃标志位

            //跳完障碍标志位清零
            BIG_Obstacle_Flag = 0;
        }
        if(Zebra_Flag_Last_TIME  > 3){
            Zebra_Flag_Last = 0;
            Zebra_Flag_Last_TIME = 0;
        }
}

float Angle_Calculation(int x1, int y1, int x2, int y2, int x3, int y3){
     float dx1 =(float)( x2-x1 );
     float dy1 =(float)( y2-y1 );
     float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
     float dx2 =(float)( x2-x3 );
     float dy2 =(float)( y2-y3 );
     float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
     float c1 = dx1 / dn1;
     float s1 = dy1 / dn1;
     float c2 = dx2 / dn2;
     float s2 = dy2 / dn2;
     float degree_angle;
     return degree_angle =fabs(atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1)/PI*180);
}

void  Stayguy_ADS(int x_start, int y_start, int x_end, int y_end){
    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;   //k
    float temp_b = 0;      //b

    do
    {
        if(x_start != x_end)
        {
            temp_rate = (float)(y_start - y_end) / (float)(x_start - x_end);
            temp_b = (float)y_start - (float)x_start * temp_rate;
        }
        else
        {
            while(y_start != y_end)
            {
                Pixel[y_start][x_start] = 0;
                y_start += y_dir;
            }
            break;
        }

        if(func_abs(y_start - y_end) > func_abs(x_start - x_end))
        {
            while(y_start != y_end)
            {
                Pixel[y_start][x_start] = 0;
                y_start += y_dir;
                x_start = (int16)(((float)y_start - temp_b) / temp_rate);
            }
        }
        else
        {
            while(x_start != x_end)
            {
                Pixel[y_start][x_start] = 0;
                x_start += x_dir;
                y_start = (int16)((float)x_start * temp_rate + temp_b);
            }
        }
    }while(0);
}


uint8 Left_Straight_Flag = 0,Right_Straight_Flag = 0;

uint8 Corner_LEFT = 0,Corner_RIGHT = 0;

float Curvature1[6] = {0},Curvature2[6] = {0};

void Curvature_Calculation(void)
{
    Left_Straight_Flag  = 0;  Right_Straight_Flag = 0;
    Corner_LEFT = 0;          Corner_RIGHT = 0;

    for(uint8 i = 0; i < 6 ;i++){
        Curvature1[i] = 0;
        Curvature2[i] = 0;
    }

    int x1[3]={0},y1[3]={0};  int x2[3]={0},y2[3]={0};
//八领域边线几乎长直//八领域边线计算直线//大长直
    if(ImageStatus.OFFLine < 20){
    x1[0]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 5)].LeftBoundary;
    x1[1]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 15)].LeftBoundary;
    x1[2]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 35)].LeftBoundary;

    y1[0] = CLIP_DATA(ImageStatus.OFFLine + 5);
    y1[1] = CLIP_DATA(ImageStatus.OFFLine + 15);
    y1[2] = CLIP_DATA(ImageStatus.OFFLine + 35);
    Curvature1[0]=Angle_Calculation(x1[0],y1[0],x1[1],y1[1],x1[2],y1[2]);

    x2[0]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 5)].RightBoundary;
    x2[1]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 15)].RightBoundary;
    x2[2]=ImageDeal[CLIP_DATA(ImageStatus.OFFLine + 30)].RightBoundary;

    y2[0] = CLIP_DATA(ImageStatus.OFFLine + 5);
    y2[1] = CLIP_DATA(ImageStatus.OFFLine + 15);
    y2[2] = CLIP_DATA(ImageStatus.OFFLine + 30);
    Curvature2[0]=Angle_Calculation(x2[0],y2[0],x2[1],y2[1],x2[2],y2[2]);
    }

    if(Curvature1[0] > 168){
        Left_Straight_Flag = 4;
    }
    else{
        Corner_LEFT = 1;
        Left_Straight_Flag = 0;
    }

    if(Curvature2[0] > 168 ){
        Right_Straight_Flag = 4;
    }
    else{
        Corner_RIGHT = 1;
        Right_Straight_Flag = 0;
    }

    int x3[3]={0},y3[3]={0};  int x4[3]={0},y4[3]={0};

    if(Left_Straight_Flag == 4 && Right_Straight_Flag == 4){
    x3[0]=ImageDeal[8].LeftBoundary;
    x3[1]=ImageDeal[25].LeftBoundary;
    x3[2]=ImageDeal[50].LeftBoundary;

    y3[0] = 8;
    y3[1] = 25;
    y3[2] = 50;
    Curvature1[1]=Angle_Calculation(x3[0],y3[0],x3[1],y3[1],x3[2],y3[2]);

    x4[0]=ImageDeal[8].RightBoundary;
    x4[1]=ImageDeal[25].RightBoundary;
    x4[2]=ImageDeal[50].RightBoundary;

    y4[0] = 8;
    y4[1] = 25;
    y4[2] = 50;
    Curvature2[1]=Angle_Calculation(x4[0],y4[0],x4[1],y4[1],x4[2],y4[2]);

    if(Curvature1[1] > 168){
        Left_Straight_Flag = 5;
    }

    if(Curvature2[1] > 168 ){
        Right_Straight_Flag = 5;
    }
   }
}


//压缩图像//把几个像素点融合成为一个像素点
float Mh = MT9V03X_H;
float Lh = LCDH;
float Mw = MT9V03X_W;
float Lw = LCDW;

float div_div = 0;

void compressimage(void) {
  int i, j, row, line;
  const float div_h = Mh / Lh, div_w = Mw / Lw;
  div_div   = div_w;
  for (i = 0; i < LCDH; i++) {
    row = i * div_h + 0.5;
    for (j = 0; j < LCDW; j++) {
      line = j * div_w + 0.5;
      Image_Use[i][j] = mt9v03x_image[row][line];
    }
  }
  mt9v03x_finish_flag = 0;  //使用完一帧DMA传输的图像图像  可以开始传输下一帧
}

uint8 length_max = 0;

//计算一列的白列长度
uint8 len(uint8 x)
{
    unsigned char line,y;
    for(line=55;line>0;line--)
    {
        if(!Pixel[line][x])
        {
            y=55-line;
            break;
        }
        else
            y=55;
    }
    return y;
}
//计算20~60列之间的最长白列长度
uint8 len_maxget(void)
{
    uint8 x,y,max=0;
    for(x=20;x<=60;x++)
    {
        y=len(x);
        if(max<y)
        {
            max=y;
        }
    }
    return max;
}

//用于加速的直道检测//用于加速的直道检测
float variance, variance_acc;  //方差
void Straightacc_Test(void) {
  int sum = 0;
  for (Ysite = 55; Ysite > ImageStatus.OFFLine + 1; Ysite--) {
    sum += (ImageDeal[Ysite].Center - (LCDW / 2 - 1)) *(ImageDeal[Ysite].Center - (LCDW / 2 - 1));
  }
  //计算中线的方差
  variance_acc = (float)sum / (54 - ImageStatus.OFFLine);
  //计算最长白列
  length_max = len_maxget();

  if( length_max >= 45             //最长白列，通过判断赛道最大的宽度是否满足规定的区间 来判断是否为直道
    && ImageStatus.OFFLine <= 25   // 直道的前瞻一定很小//防止横断路障检测直线加速
    && abs((ImageStatus.Det_True - 39)) < 6
    && variance_acc < 10
       //左右长直道路
    ) {
      ImageStatus.straight_acc = 1;

      if(Left_Straight_Flag  > 3 && Right_Straight_Flag > 3){
          ImageStatus.straight_acc = 2;
          if(Right_Straight_Flag == 5 && Left_Straight_Flag == 5){
              ImageStatus.straight_acc = 3;
          }
      }
  } else
      ImageStatus.straight_acc = 0;
}
/*****************直线判断******************///(用于判断传统扫线的边线是否是直线)
float Straight_Judge(uint8 dir, uint8 start, uint8 end)     //返回结果小于1即为直线
{
    int i;
    float S = 0, Sum = 0, Err = 0, k = 0;
    switch (dir)
    {
    case 1:k = (float)(ImageDeal[start].LeftBorder - ImageDeal[end].LeftBorder) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder) * (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    case 2:k = (float)(ImageDeal[start].RightBorder - ImageDeal[end].RightBorder) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder) * (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    }
    return S;
}

/*****************直线判断******************///(用于判断八领域爬出的边线是否是直线)
float Straight_Judge_2(uint8 dir, uint8 start, uint8 end)     //返回结果小于1即为直线
{
    int i;
    float S = 0, Sum = 0, Err = 0, k = 0;
    switch (dir)
    {
    case 1:k = (float)(ImageDeal[start].LeftBoundary - ImageDeal[end].LeftBoundary) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].LeftBoundary + k * i - ImageDeal[i + start].LeftBoundary) * (ImageDeal[start].LeftBoundary + k * i - ImageDeal[i + start].LeftBoundary);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    case 2:k = (float)(ImageDeal[start].RightBoundary - ImageDeal[end].RightBoundary) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].RightBoundary + k * i - ImageDeal[i + start].RightBoundary) * (ImageDeal[start].RightBoundary + k * i - ImageDeal[i + start].RightBoundary);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    }
    return S;
}

void Check_Straight(void){
    Curvature_Calculation();
    //直线检测
    Straight_Left_Varance  = Straight_Judge(1,CLIP_DATA(ImageStatus.OFFLine), 50);
    Straight_Right_Varance = Straight_Judge(2,CLIP_DATA(ImageStatus.OFFLine), 50);

    Straight_Left_Varance1  = Straight_Judge(1,10, 50);
    Straight_Right_Varance1 = Straight_Judge(2,10, 50);

    //没有完全写好用于直线加速的代码//下面这个函数没什么作用//用到的部分东西用于检测十字中线方差和最长白列
    if(//用于检测直线加速的函数 //圆环状态内不加速
          ImageStatus.Road_type != LeftCirque
        &&ImageStatus.Road_type != RightCirque
        )
    {
      Straightacc_Test();
    }
}

void Pixel_Filter(void) {
  int nr;  //行
  int nc;  //列

  for (nr = 10; nr < 40; nr++) {
    for (nc = 10; nc < 70; nc = nc + 1) {
      if ((Pixel[nr][nc] == 0) && (Pixel[nr - 1][nc] + Pixel[nr + 1][nc] +
          Pixel[nr][nc + 1] + Pixel[nr][nc - 1] >= 3 * 255
                                   )) {
        Pixel[nr][nc] = 255;
      }
    }
  }
}

void Buzzer_Init(void){
    gpio_init(P33_10,GPO,0, GPO_PUSH_PULL);
}

void Buzzer(void){
    if(Buzzer_Count!=0){
        gpio_set_level(P33_10, 1);
        Buzzer_Count--;
    }
    else{
        gpio_set_level(P33_10, 0);
    }
}

void TOF_Init(void){
    //TOF模块初始化
//    dl1a_init();
    //下面这个不是TOF模块初始化
//    adc_init(ADC0_CH1_A1,ADC_8BIT);
    adc_init(ADC0_CH1_A1,ADC_12BIT);
}
