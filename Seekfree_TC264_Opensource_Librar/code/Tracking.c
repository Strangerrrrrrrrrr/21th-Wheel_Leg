/*
 * Tracking.c
 *
 *  Created on: 2025年3月21日
 *      Author: 不懂
 */
#include "Tracking.h"
//变量定义
ImageDealDatatypedef ImageDeal[60];                //记录单行的信息
ImageStatustypedef ImageStatus;                    //图像的全局变量

static int BottomBorderRight = 79,                 //59行右边界
BottomBorderLeft = 0,                              //59行左边界
BottomCenter = 39;                                  //59行中点

int ImageScanInterval = 2;                         //扫边范围    上一行的边界+-ImageScanInterval
int ImageScanInterval_Cross = 40;                       //270°的弯道后十字的扫线范围

static uint8* PicTemp;                             //保存单行图像
static int ytemp = 0;                              //存放行
static int TFSite = 0, FTSite = 0;                 //存放行
int Ysite = 0, Xsite = 0;                   //Y坐标=列
static int IntervalLow = 0, IntervalHigh = 0;      //定义高低扫描区间
static float DetR = 0, DetL = 0;                   //存放斜率

uint8 ExtenLFlag = 0;  //是否左延长标志
uint8 ExtenRFlag = 0;  //是否右延长标志

float Weighting[10] = {0.96, 0.92, 0.88, 0.83, 0.77,0.71, 0.65, 0.59, 0.53, 0.47};//10行权重参数，随意更改，基本不影响，大致按照正态分布即可


uint8 Image_Mid = 39;                                   //寻线基本变量
static uint8 Search_StartPoint(void){
    PicTemp = Pixel[59];                                     //原本59  改成了57
    if (*(PicTemp + LCDW/2) == 0)                       //如果底边图像中点为黑，异常情况
    {
      for(Xsite = 0; Xsite < LCDW/2; Xsite++)             //找左右边线
      {
        if (*(PicTemp + LCDW/2 - Xsite) != 0)             //一旦找到左或右赛道到中心距离，就break
          break;                                             //并且记录Xsite
        if (*(PicTemp + LCDW/2 + Xsite) != 0)
          break;
      }

      if (*(PicTemp + LCDW/2 - Xsite) != 0)                //赛道如果在左边的话
      {
        BottomBorderRight = LCDW/2 - Xsite + 1;            // 59行右边线有啦
        for (Xsite = BottomBorderRight; Xsite > 0; Xsite--)   //开始找59行左边线
        {
          if (*(PicTemp + Xsite) == 0 &&
              *(PicTemp + Xsite - 1) == 0)                    //连续两个黑点，滤波
          {
            BottomBorderLeft = Xsite;                         //左边线找到
            break;
          } else if (Xsite == 1) {
            BottomBorderLeft = 0;                             //搜索到最后了，看不到左边线，左边线认为是0
            break;
          }
        }
      } else if (*(PicTemp + LCDW/2 + Xsite) != 0)    //赛道如果在右边的话
      {
        BottomBorderLeft = LCDW/2 + Xsite - 1;        // 59行左边线有啦
        for (Xsite = BottomBorderLeft; Xsite < 79; Xsite++)   //开始找59行左边线
        {
          if (  *(PicTemp + Xsite) == 0
              &&*(PicTemp + Xsite + 1) == 0)                  //连续两个黑点，滤波
          {
            BottomBorderRight = Xsite;                        //右边线找到
            break;
          } else if (Xsite == 78) {
            BottomBorderRight = 79;                           //搜索到最后了，看不到右边线，左边线认为是79
            break;
          }
        }
      }
    }
    else                                                       //左边线中点是白的，比较正常的情况
    {
      for(Xsite = Image_Mid; Xsite < LCDW - 2; Xsite++)
      {   //一个点一个点地搜索右边线//找到白白黑即认为找到右边线
        if (
                *(PicTemp + Xsite) == 255
            &&*(PicTemp + Xsite - 1) == 255
            &&*(PicTemp + Xsite + 1) == 0
            )
        {
          BottomBorderRight = Xsite;                           //找到就记录
          break;
        } else if (Xsite == 78) {
          BottomBorderRight = 79;                              //找不到认为79
          break;
        }
      }
      for (Xsite = Image_Mid; Xsite > 1; Xsite--)              //一个点一个点地搜索左边线
      {//一个点一个点地搜索左边线//找到白白黑即认为找到左边线
        if (
                *(PicTemp + Xsite) == 255
            &&*(PicTemp + Xsite + 1) == 255
            &&*(PicTemp + Xsite - 1 ) == 0
            )
        {
          BottomBorderLeft = Xsite;                            //找到就记录
          break;
        } else if (Xsite == 1) {
          BottomBorderLeft = 0;                                //找不到认为0
          break;
        }
      }
    }
    BottomCenter =(BottomBorderLeft + BottomBorderRight) / 2;   // 59行中点直接取平均
    ImageDeal[59].LeftBorder = BottomBorderLeft;                //在数组里面记录一下信息，第一行特殊一点而已
    ImageDeal[59].RightBorder = BottomBorderRight;
    ImageDeal[59].Center = BottomCenter;                        //确定最底边
    ImageDeal[59].Wide = BottomBorderRight - BottomBorderLeft;  //存储宽度信息
    ImageDeal[59].IsLeftFind = 'T';
    ImageDeal[59].IsRightFind = 'T';

    //存放本帧图像最底行的中线，下一帧从该点开始向两边寻找
    Image_Mid = (uint8)BottomCenter;

    for (Ysite = 58; Ysite > 54; Ysite--)                       //由中间向两边确定底边五行
    {
      PicTemp = Pixel[Ysite];
      for (Xsite = ImageDeal[Ysite+1].Center; Xsite < LCDW-1;Xsite++)
      {//遍历寻找下面四行的右边线
        if (*(PicTemp + Xsite) == 255 && *(PicTemp + Xsite - 1) == 255&&*(PicTemp + Xsite + 1) == 0) {
          ImageDeal[Ysite].RightBorder = Xsite;
          break;
        }
        else if (Xsite == 78) {
          ImageDeal[Ysite].RightBorder = 79;
          break;
        }
      }
      for(Xsite = ImageDeal[Ysite+1].Center; Xsite > 0;Xsite--)                                             //和前面一样的搜索
      {
        if (*(PicTemp + Xsite) == 255 && *(PicTemp + Xsite + 1) == 255 &&*(PicTemp + Xsite - 1) == 0
        ) {
          ImageDeal[Ysite].LeftBorder = Xsite;
          break;
        }
        else if (Xsite == 1) {
          ImageDeal[Ysite].LeftBorder = 0;
          break;
        }
      }
      ImageDeal[Ysite].IsLeftFind = 'T';                                    //这些信息存储到数组里
      ImageDeal[Ysite].IsRightFind = 'T';
      ImageDeal[Ysite].Center =
          (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2; //存储中点
      ImageDeal[Ysite].Wide =
          ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;       //存储宽度
    }
    return 'T';
}


static uint8 DrawLinesFirst(void) {
  PicTemp = Pixel[59];                                     //原本59  改成了57
  if (*(PicTemp + ImageSensorMid) == 0)                 //如果底边图像中点为黑，异常情况
  {
    for (Xsite = 0; Xsite < ImageSensorMid; Xsite++)    //找左右边线
    {
      if (*(PicTemp + ImageSensorMid - Xsite) != 0)     //一旦找到左或右赛道到中心距离，就break
        break;                                          //并且记录Xsite
      if (*(PicTemp + ImageSensorMid + Xsite) != 0)
        break;
    }

    if (*(PicTemp + ImageSensorMid - Xsite) != 0)       //赛道如果在左边的话
    {
      BottomBorderRight = ImageSensorMid - Xsite + 1;   // 59行右边线有啦
      for (Xsite = BottomBorderRight; Xsite > 0; Xsite--)  //开始找59行左边线
      {
        if (*(PicTemp + Xsite) == 0 &&
            *(PicTemp + Xsite - 1) == 0)                //连续两个黑点，滤波
        {
          BottomBorderLeft = Xsite;                     //左边线找到
          break;
        } else if (Xsite == 1) {
          BottomBorderLeft = 0;                         //搜索到最后了，看不到左边线，左边线认为是0
          break;
        }
      }
    } else if (*(PicTemp + ImageSensorMid + Xsite) != 0)  //赛道如果在右边的话
    {
      BottomBorderLeft = ImageSensorMid + Xsite - 1;    // 59行左边线有啦
      for (Xsite = BottomBorderLeft; Xsite < 79; Xsite++)  //开始找59行左边线
      {
        if (  *(PicTemp + Xsite) == 0
            &&*(PicTemp + Xsite + 1) == 0)              //连续两个黑点，滤波
        {
          BottomBorderRight = Xsite;                    //右边线找到
          break;
        } else if (Xsite == 78) {
          BottomBorderRight = 79;                       //搜索到最后了，看不到右边线，左边线认为是79
          break;
        }
      }
    }
  }
  else                                                //左边线中点是白的，比较正常的情况
  {
    for (Xsite = 79; Xsite >ImageSensorMid; Xsite--)   //一个点一个点地搜索右边线
    {
      if (  *(PicTemp + Xsite) == 255
          &&*(PicTemp + Xsite - 1) == 255)                //连续两个黑点，滤波     //两个白点
      {
        BottomBorderRight = Xsite;                      //找到就记录
        break;
      } else if (Xsite == 40) {
        BottomBorderRight = 39;                         //找不到认为79
        break;
      }
    }
    for (Xsite = 0; Xsite < ImageSensorMid; Xsite++)    //一个点一个点地搜索左边线
    {
      if (  *(PicTemp + Xsite) == 255
          &&*(PicTemp + Xsite + 1) == 255)                //连续两个黑点，滤波
      {
        BottomBorderLeft = Xsite;                       //找到就记录
        break;
      } else if (Xsite == 38) {
        BottomBorderLeft = 39;                           //找不到认为0
        break;
      }
    }
  }
  BottomCenter =(BottomBorderLeft + BottomBorderRight) / 2;   // 59行中点直接取平均
  ImageDeal[59].LeftBorder = BottomBorderLeft;                //在数组里面记录一下信息，第一行特殊一点而已
  ImageDeal[59].RightBorder = BottomBorderRight;
  ImageDeal[59].Center = BottomCenter;                        //确定最底边
  ImageDeal[59].Wide = BottomBorderRight - BottomBorderLeft;  //存储宽度信息
  ImageDeal[59].IsLeftFind = 'T';
  ImageDeal[59].IsRightFind = 'T';
  for (Ysite = 58; Ysite > 54; Ysite--)                       //由中间向两边确定底边五行
  {
    PicTemp = Pixel[Ysite];
    for (Xsite = 79; Xsite > ImageDeal[Ysite + 1].Center;Xsite--)                                             //和前面一样的搜索
    {
      if (*(PicTemp + Xsite) == 255 && *(PicTemp + Xsite - 1) == 255) {
        ImageDeal[Ysite].RightBorder = Xsite;
        break;
      } else if (Xsite == (ImageDeal[Ysite + 1].Center+1)) {
        ImageDeal[Ysite].RightBorder = ImageDeal[Ysite + 1].Center;
        break;
      }
    }
    for (Xsite = 0; Xsite < ImageDeal[Ysite + 1].Center;Xsite++)                                             //和前面一样的搜索
    {
      if (*(PicTemp + Xsite) == 255 && *(PicTemp + Xsite + 1) == 255) {
        ImageDeal[Ysite].LeftBorder = Xsite;
        break;
      } else if (Xsite == (ImageDeal[Ysite + 1].Center-1)) {
        ImageDeal[Ysite].LeftBorder = ImageDeal[Ysite + 1].Center;
        break;
      }
    }
    ImageDeal[Ysite].IsLeftFind = 'T';                        //这些信息存储到数组里
    ImageDeal[Ysite].IsRightFind = 'T';
    ImageDeal[Ysite].Center =
        (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) /2; //存储中点
    ImageDeal[Ysite].Wide =
        ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;      //存储宽度
  }
  return 'T';
}  //最基本的要求，最近的五行首先不会受到干扰，这需要在安装的时候调整摄像头的视角

void GetJumpPointFromDet(uint8* p,uint8 type,int L,int H,JumpPointtypedef* Q)  //第一个参数是要查找的数组（80个点）
                                                                               //第二个扫左边线还是扫右边线
{                                                                              //三四是开始和结束点
  int i = 0;
  if (type == 'L')                              //扫描左边线
  {
    for (i = H; i >= L; i--) {
      if (*(p + i) == 255 && *(p + i - 1) != 255)   //由黑变白
      {
        Q->point = i;                           //记录左边线
        Q->type = 'T';                          //正确跳变
        break;
      } else if (i == (L + 1))                  //若果扫到最后也没找到
      {
        if (*(p + (L + H) / 2) != 0)            //如果中间是白的
        {
          Q->point = (L + H) / 2;               //认为左边线是中点
          Q->type = 'W';                        //非正确跳变且中间为白，认为没有边
          break;
        } else                                  //非正确跳变且中间为黑
        {
          Q->point = H;                         //如果中间是黑的
          Q->type = 'H';                        //左边线直接最大值，认为是大跳变
          break;
        }
      }
    }
  } else if (type == 'R')                       //扫描右边线
  {
    for (i = L; i <= H; i++)                    //从右往左扫
    {
      if (*(p + i) == 255 && *(p + i + 1) != 255)   //找由黑到白的跳变
      {
        Q->point = i;                           //记录
        Q->type = 'T';
        break;
      } else if (i == (H - 1))                  //若果扫到最后也没找到
      {
        if (*(p + (L + H) / 2) != 0)            //如果中间是白的
        {
          Q->point = (L + H) / 2;               //右边线是中点
          Q->type = 'W';
          break;
        } else                                  //如果中点是黑的
        {
          Q->point = L;                         //左边线直接最大值
          Q->type = 'H';
          break;
        }
      }
    }
  }
}

/*边线追逐大致得到全部边线*/
static void DrawLinesProcess(void)
{
  uint8 L_Found_T = 'F';  //确定无边斜率的基准有边行是否被找到的标志
  uint8 Get_L_line = 'F';  //找到这一帧图像的基准左斜率
  uint8 R_Found_T = 'F';  //确定无边斜率的基准有边行是否被找到的标志
  uint8 Get_R_line = 'F';  //找到这一帧图像的基准右斜率
  float D_L = 0;           //延长线左边线斜率
  float D_R = 0;           //延长线右边线斜率
  int ytemp_W_L;           //记住首次左丢边行
  int ytemp_W_R;           //记住首次右丢边行
  ExtenRFlag = 0;          //标志位清0
  ExtenLFlag = 0;
  ImageStatus.Left_Line = 0;
  ImageStatus.WhiteLine = 0;
  ImageStatus.Right_Line = 0;
  for (Ysite = 54 ; Ysite > ImageStatus.OFFLine; Ysite--)            //前5行处理过了，下面从55行到（设定的不处理的行OFFLine）
  {                        //太远的图像不稳定，OFFLine以后的不处理
    PicTemp = Pixel[Ysite];
    JumpPointtypedef JumpPoint[2];                                          // 0左1右
    if (ImageStatus.Road_type != Cross_ture) {
      IntervalLow =ImageDeal[Ysite + 1].RightBorder  - ImageScanInterval;           //从上一行右边线-Interval的点开始（确定扫描开始点）
      IntervalHigh =ImageDeal[Ysite + 1].RightBorder + ImageScanInterval;           //到上一行右边线+Interval的点结束（确定扫描结束点）
    }
    else {
      IntervalLow =ImageDeal[Ysite + 1].RightBorder -ImageScanInterval_Cross;       //从上一行右边线-Interval_Cross的点开始（确定扫描开始点）
      IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval_Cross;    //到上一行右边线+Interval_Cross的点开始（确定扫描开始点）
    }
    LimitL(IntervalLow);   //确定左扫描区间并进行限制
    LimitH(IntervalHigh);  //确定右扫描区间并进行限制
    GetJumpPointFromDet(PicTemp, 'R', IntervalLow, IntervalHigh,&JumpPoint[1]);     //扫右边线

    if (ImageStatus.Road_type != Cross_ture){
        IntervalLow =ImageDeal[Ysite + 1].LeftBorder  - ImageScanInterval;                //从上一行左边线-5的点开始（确定扫描开始点）
        IntervalHigh =ImageDeal[Ysite + 1].LeftBorder + ImageScanInterval;               //到上一行左边线+5的点结束（确定扫描结束点）
    }

    LimitL(IntervalLow);   //确定左扫描区间并进行限制
    LimitH(IntervalHigh);  //确定右扫描区间并进行限制
    GetJumpPointFromDet(PicTemp, 'L', IntervalLow, IntervalHigh,&JumpPoint[0]);

    if (JumpPoint[0].type =='W')                                                    //如果本行左边线不正常跳变，即这10个点都是白的
    {
      ImageDeal[Ysite].LeftBorder =ImageDeal[Ysite + 1].LeftBorder;                 //本行左边线用上一行的数值
    } else                                                                          //左边线正常
    {
      ImageDeal[Ysite].LeftBorder = JumpPoint[0].point;                             //记录下来啦
    }

    if (JumpPoint[1].type == 'W')                                                   //如果本行右边线不正常跳变
    {
      ImageDeal[Ysite].RightBorder =ImageDeal[Ysite + 1].RightBorder;               //本行右边线用上一行的数值
    } else                                                                          //右边线正常
    {
      ImageDeal[Ysite].RightBorder = JumpPoint[1].point;                            //记录下来啦
    }

    ImageDeal[Ysite].IsLeftFind =JumpPoint[0].type;                                 //记录本行是否找到边线，即边线类型
    ImageDeal[Ysite].IsRightFind = JumpPoint[1].type;

    //重新确定那些大跳变的边缘
    if (( ImageDeal[Ysite].IsLeftFind == 'H'
         ||ImageDeal[Ysite].IsRightFind == 'H')) {
      if (ImageDeal[Ysite].IsLeftFind == 'H')                                   //如果左边线大跳变
        for (Xsite = (ImageDeal[Ysite].LeftBorder + 1);
             Xsite <= (ImageDeal[Ysite].RightBorder - 1);
             Xsite++)                                                           //左右边线之间重新扫描
        {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite + 1) != 0)) {
            ImageDeal[Ysite].LeftBorder =Xsite;                                 //如果上一行左边线的右边有黑白跳变则为绝对边线直接取出
            ImageDeal[Ysite].IsLeftFind = 'T';
            break;
          } else if (*(PicTemp + Xsite) != 0)                                   //一旦出现白点则直接跳出
            break;
          else if (Xsite ==(ImageDeal[Ysite].RightBorder - 1))
          {
             ImageDeal[Ysite].IsLeftFind = 'T';
            break;
          }
        }
      if ((ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder) <=
          7)                              //图像宽度限定
      {
        ImageStatus.OFFLine = Ysite + 1;  //如果这行比7小了后面直接不要了
        break;
      }
      if (ImageDeal[Ysite].IsRightFind == 'H')
        for (Xsite = (ImageDeal[Ysite].RightBorder - 1);
             Xsite >= (ImageDeal[Ysite].LeftBorder + 1); Xsite--) {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite - 1) != 0)) {
            ImageDeal[Ysite].RightBorder =
                Xsite;                    //如果右边线的左边还有黑白跳变则为绝对边线直接取出
            ImageDeal[Ysite].IsRightFind = 'T';
            break;
          } else if (*(PicTemp + Xsite) != 0)
            break;
          else if (Xsite == (ImageDeal[Ysite].LeftBorder + 1))
          {
            ImageDeal[Ysite].RightBorder = Xsite;
            ImageDeal[Ysite].IsRightFind = 'T';
            break;
          }
        }
    }

 /***********重新确定无边行************/
    int ysite = 0;
    uint8 L_found_point = 0;
    uint8 R_found_point = 0;


    if(  ImageStatus.Road_type != Ramp)
    {
    if (    ImageDeal[Ysite].IsRightFind == 'W'
          &&Ysite > 10
          &&Ysite < 50
          &&ImageStatus.Road_type!=Barn_in
          )                     //最早出现的无边行
    {
      if (Get_R_line == 'F')    //这一帧图像没有跑过这个找基准线的代码段才运行
      {
        Get_R_line = 'T';       //找了  一帧图像只跑一次 置为T
        ytemp_W_R = Ysite + 2;
        for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++) {
          if (ImageDeal[CLIP_DATA(ysite)].IsRightFind =='T')  //往无边行下面搜索  一般都是有边的
            R_found_point++;
        }
        if (R_found_point >8)                      //找到基准斜率边  做延长线重新确定无边   当有边的点数大于8
        {
          D_R = ((float)(ImageDeal[CLIP_DATA(Ysite + R_found_point)].RightBorder - ImageDeal[CLIP_DATA(Ysite + 3)].RightBorder)) /((float)(R_found_point - 3));
                                                  //求下面这些点连起来的斜率
                                                  //好给无边行做延长线左个基准
          if (D_R > 0) {
            R_Found_T ='T';                       //如果斜率大于0  那么找到了这个基准行  因为梯形畸变
                                                  //所以一般情况都是斜率大于0  小于0的情况也不用延长 没必要
          } else {
            R_Found_T = 'F';                      //没有找到这个基准行
            if (D_R < 0)
            ExtenRFlag = 'F';                   //这个标志位用于十字角点补线  防止图像误补用的
          }
        }
      }
      if (R_Found_T == 'T')
        ImageDeal[Ysite].RightBorder =ImageDeal[ytemp_W_R].RightBorder -D_R * (ytemp_W_R - Ysite);  //如果找到了 那么以基准行做延长线

      LimitL(ImageDeal[Ysite].RightBorder);  //限幅
      LimitH(ImageDeal[Ysite].RightBorder);  //限幅
    }

    if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite > 10 && Ysite < 50 &&
        ImageStatus.Road_type != Barn_in)    //下面同理  左边界
    {
      if (Get_L_line == 'F') {
        Get_L_line = 'T';
        ytemp_W_L = Ysite + 2;
        for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++) {
          if (ImageDeal[ysite].IsLeftFind == 'T')
            L_found_point++;
        }
        if (L_found_point > 8)              //找到基准斜率边  做延长线重新确定无边
        {
          D_L = ((float)(ImageDeal[CLIP_DATA(Ysite + 3)].LeftBorder -ImageDeal[CLIP_DATA(Ysite + L_found_point)].LeftBorder)) /((float)(L_found_point - 3));
          if (D_L > 0) {
            L_Found_T = 'T';

          } else {
            L_Found_T = 'F';
            if (D_L < 0)
              ExtenLFlag = 'F';
          }
        }
      }

      if (L_Found_T == 'T')
        ImageDeal[Ysite].LeftBorder =ImageDeal[ytemp_W_L].LeftBorder + D_L * (ytemp_W_L - Ysite);

      LimitL(ImageDeal[Ysite].LeftBorder);  //限幅
      LimitH(ImageDeal[Ysite].LeftBorder);  //限幅
    }
}
    if (ImageDeal[Ysite].IsLeftFind == 'W'&&ImageDeal[Ysite].IsRightFind == 'W')
         {
             ImageStatus.WhiteLine++;  //要是左右都无边，丢边数+1
         }
        if (ImageDeal[Ysite].IsLeftFind == 'W'&&Ysite < 55 && Ysite > 5)
        {
             ImageStatus.Left_Line++;
        }
        if (ImageDeal[Ysite].IsRightFind == 'W'&&Ysite < 55 && Ysite > 5)
        {
             ImageStatus.Right_Line++;
        }

      LimitL(ImageDeal[Ysite].LeftBorder);   //限幅
      LimitH(ImageDeal[Ysite].LeftBorder);   //限幅
      LimitL(ImageDeal[Ysite].RightBorder);  //限幅
      LimitH(ImageDeal[Ysite].RightBorder);  //限幅

      ImageDeal[Ysite].Wide =ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
      ImageDeal[Ysite].Center =(ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

    if (ImageDeal[Ysite].Wide <= 7)         //重新确定可视距离
    {
      ImageStatus.OFFLine = Ysite + 1;
      break;
    }
    else if (  ImageDeal[Ysite].RightBorder <= 10
             ||ImageDeal[Ysite].LeftBorder >= 70) {
              ImageStatus.OFFLine = Ysite + 1;
              break;
    }                                        //当图像宽度小于0或者左右边达到一定的限制时，则终止巡边
  }
  return;
}

//延长线绘制，理论上来说是很准确的
static void DrawExtensionLine(void)        //绘制延长线并重新确定中线 ，把补线补成斜线
{
  if ((
        ImageStatus.Road_type != Barn_in
        &&ImageStatus.Road_type != Ramp)
        &&ImageStatus.Road_type !=LeftCirque
        &&ImageStatus.Road_type !=RightCirque
        )                                  // g5.22  6.22调试注释  记得改回来
  {
    if (ImageStatus.WhiteLine >= ImageStatus.TowPoint_True - 15)
      TFSite = 55;
//    if (ImageStatus.CirqueOff == 'T' && ImageStatus.Road_type == LeftCirque)
//      TFSite = 55;
    if (ExtenLFlag != 'F')
      for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);
           Ysite--)                       //从第五行开始网上扫扫到顶边下面两行   多段补线
                                          //不仅仅只有一段
      {
        PicTemp = Pixel[Ysite];           //存当前行
        if (ImageDeal[Ysite].IsLeftFind =='W')                          //如果本行左边界没扫到但扫到的是白色，说明本行没有左边界点
        {
          //**************************************************//**************************************************
          if (ImageDeal[Ysite + 1].LeftBorder >= 70)                    //如果左边界实在是太右边
          {
            ImageStatus.OFFLine = Ysite + 1;
            break;                        //直接跳出（极端情况）
          }
          //************************************************//*************************************************

          while (Ysite >= (ImageStatus.OFFLine + 4))                    //此时还没扫到顶边
          {
            Ysite--;                      //继续往上扫
            if (  ImageDeal[Ysite].IsLeftFind == 'T'
                &&ImageDeal[Ysite - 1].IsLeftFind == 'T'
                &&ImageDeal[Ysite - 2].IsLeftFind == 'T'
                &&ImageDeal[Ysite - 2].LeftBorder > 10
                &&ImageDeal[Ysite - 2].LeftBorder <70
                )                                                       //如果扫到本行出现了并且本行以上连续三行都有左边界点（左边界在空白上方）
            {
              FTSite = Ysite - 2;          //把本行上面的第二行存入FTsite
              break;
            }
          }

          DetL =
              ((float)(ImageDeal[FTSite].LeftBorder -
                       ImageDeal[TFSite].LeftBorder)) /
              ((float)(FTSite - TFSite));  //左边界的斜率：列的坐标差/行的坐标差
          if (FTSite > ImageStatus.OFFLine)
            for (
                ytemp = TFSite; ytemp >= FTSite; ytemp--)               //从第一次扫到的左边界的下面第二行的坐标开始往上扫直到空白上方的左边界的行坐标值
            {
              ImageDeal[ytemp].LeftBorder =
                  (int)(DetL * ((float)(ytemp - TFSite))) +
                  ImageDeal[TFSite]
                      .LeftBorder;                                      //将这期间的空白处补线（补斜线），目的是方便图像处理
            }
        } else
          TFSite = Ysite + 2;                                           //如果扫到了本行的左边界，该行存在这里面，（算斜率）
      }

    if (ImageStatus.WhiteLine >= ImageStatus.TowPoint_True - 15)
      TFSite = 55;
    // g5.22
    if (ImageStatus.CirqueOff == 'T' && ImageStatus.Road_type == RightCirque)
      TFSite = 55;
    if (ExtenRFlag != 'F')
      for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);
           Ysite--)               //从第五行开始网上扫扫到顶边下面两行
      {
        PicTemp = Pixel[Ysite];  //存当前行

        if (ImageDeal[Ysite].IsRightFind =='W')                       //如果本行右边界没扫到但扫到的是白色，说明本行没有右边界点，但是处于赛道内的
        {
          if (ImageDeal[Ysite + 1].RightBorder <= 10)                 //如果右边界实在是太左边
          {
            ImageStatus.OFFLine =Ysite + 1;                           //直接跳出，说明这种情况赛道就尼玛离谱
            break;
          }
          while (Ysite >= (ImageStatus.OFFLine + 4))                  //此时还没扫到顶边下面两行
          {
            Ysite--;
            if (  ImageDeal[Ysite].IsRightFind == 'T'
                &&ImageDeal[Ysite - 1].IsRightFind == 'T'
                &&ImageDeal[Ysite - 2].IsRightFind == 'T'
                &&ImageDeal[Ysite - 2].RightBorder < 70
                &&ImageDeal[Ysite - 2].RightBorder > 10
                )                                                      //如果扫到本行出现了并且本行以上连续三行都有左边界点（左边界在空白上方）
            {
              FTSite = Ysite - 2;                                      // 把本行上面的第二行存入FTsite
              break;
            }
          }

          DetR =((float)(ImageDeal[FTSite].RightBorder -ImageDeal[TFSite].RightBorder)) /((float)(FTSite - TFSite));         //右边界的斜率：列的坐标差/行的坐标差
          if (FTSite > ImageStatus.OFFLine)
            for (ytemp = TFSite; ytemp >= FTSite;ytemp--)              //从第一次扫到的右边界的下面第二行的坐标开始往上扫直到空白上方的右边界的行坐标值
            {
              ImageDeal[ytemp].RightBorder =(int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder;          //将这期间的空白处补线（补斜线），目的是方便图像处理
            }
        } else
          TFSite =Ysite +2;                                           //如果本行的右边界找到了，则把该行下面第二行坐标送个TFsite
      }
  }
  for (Ysite = 59; Ysite >= ImageStatus.OFFLine; Ysite--) {
    ImageDeal[Ysite].Center =(ImageDeal[Ysite].LeftBorder + ImageDeal[Ysite].RightBorder) /2;                                //扫描结束，把这一块经优化之后的中间值存入
    ImageDeal[Ysite].Wide =-ImageDeal[Ysite].LeftBorder +ImageDeal[Ysite].RightBorder;                                       //把优化之后的宽度存入
  }
}

/*上交大左右手法则扫线，作为处理圆环等判断元素的第二依据*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Bottom_Line_OTSU
//  @brief          获取底层左右边线
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        传入的图像数组
//  @param          Row                                     图像的Ysite
//  @param          Col                                     图像的Xsite
//  @return         Bottonline                              底边行选择
//  @time           2022年10月9日
//  @Author
//  Sample usage:   Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Bottom_Line_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{

    //寻找左边边界
    for (int Xsite = Col / 2-2; Xsite > 1; Xsite--)
    {
        if (imageInput[Bottonline][Xsite] == 255 && imageInput[Bottonline][Xsite - 1] == 0)
        {
            ImageDeal[Bottonline].LeftBoundary = Xsite;//获取底边左边线
            break;
        }
    }
    for (int Xsite = Col / 2+2; Xsite < LCDW-1; Xsite++)
    {
        if (imageInput[Bottonline][Xsite] == 255 && imageInput[Bottonline][Xsite + 1] == 0)
        {
            ImageDeal[Bottonline].RightBoundary = Xsite;//获取底边右边线
            break;
        }
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Left_and_Right_Lines
//  @brief          通过sobel提取左右边线
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        传入的图像数组
//  @param          Row                                     图像的Ysite
//  @param          Col                                     图像的Xsite
//  @param          Bottonline                              底边行选择
//  @return         无
//  @time           2022年10月7日
//  @Author
//  Sample usage:   Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Left_and_Right_Lines(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    //定义小人的当前行走状态位置为 上 左 下 右 一次要求 上：左边为黑色 左：上边为褐色 下：右边为色  右：下面有黑色
/*  前进方向定义：
                *   0
                * 3   1
                *   2
*/
/*寻左线坐标规则*/
    uint8 Left_Rule[2][8] = {
                                  {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},  (x,y )
                                  {-1,-1,1,-1,1,1,-1,1} //{-1,-1},{1,-1},{1,1},{-1,1}
    };
    /*寻右线坐标规则*/
    int Right_Rule[2][8] = {
                              {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},
                              {1,-1,1,1,-1,1,-1,-1} //{1,-1},{1,1},{-1,1},{-1,-1}
    };
      int num=0;
    uint8 Left_Ysite = Bottonline;
    uint8 Left_Xsite = (uint8)ImageDeal[Bottonline].LeftBoundary;
    uint8 Left_Rirection = 0;//左边方向
    uint8 Pixel_Left_Ysite = Bottonline;
    uint8 Pixel_Left_Xsite = 0;

    uint8 Right_Ysite = Bottonline;
    uint8 Right_Xsite = (uint8)ImageDeal[Bottonline].RightBoundary;
    uint8 Right_Rirection = 0;//右边方向
    uint8 Pixel_Right_Ysite = Bottonline;
    uint8 Pixel_Right_Xsite = 0;
    uint8 Ysite = Bottonline;
    ImageStatus.OFFLineBoundary = 5;
    while (1)
    {
            num++;
            if(num>300)
            {
                 ImageStatus.OFFLineBoundary = Ysite;
                break;
            }
        if(Ysite >= Pixel_Left_Ysite && Ysite >= Pixel_Right_Ysite)
        {
            if (Ysite < ImageStatus.OFFLineBoundary)
            {
                ImageStatus.OFFLineBoundary = Ysite;
                break;
            }
            else
            {
                Ysite--;
            }
        }
        /*********左边巡线*******/
        if ((Pixel_Left_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//右边扫线
        {
            /*计算前方坐标*/
            Pixel_Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
            Pixel_Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];

            if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//前方是黑色
            {
                //顺时针旋转90
                if (Left_Rirection == 3)
                    Left_Rirection = 0;
                else
                    Left_Rirection++;
            }
            else//前方是白色
            {
                /*计算左前方坐标*/
                Pixel_Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                Pixel_Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];

                if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//左前方为黑色
                {
                    //方向不变  Left_Rirection
                    Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0){
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                        ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                    }
                }
                else//左前方为白色
                {
                    // 方向发生改变 Left_Rirection  逆时针90度
                    Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0 )
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                    ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                    if (Left_Rirection == 0)
                        Left_Rirection = 3;
                    else
                        Left_Rirection--;
                }

            }
        }
        /*********右边巡线*******/
        if ((Pixel_Right_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//右边扫线
        {
            /*计算前方坐标*/
            Pixel_Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
            Pixel_Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];

            if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//前方是黑色
            {
                //逆时针旋转90
                if (Right_Rirection == 0)
                    Right_Rirection = 3;
                else
                    Right_Rirection--;
            }
            else//前方是白色
            {
                /*计算右前方坐标*/
                Pixel_Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                Pixel_Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];

                if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//左前方为黑色
                {
                    //方向不变  Right_Rirection
                    Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 79 )
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                }
                else//左前方为白色
                {
                    // 方向发生改变 Right_Rirection  逆时针90度
                    Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 79)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                    if (Right_Rirection == 3)
                        Right_Rirection = 0;
                    else
                        Right_Rirection++;
                }

            }
        }

        if (abs(Pixel_Right_Xsite - Pixel_Left_Xsite) < 3)//Ysite<80是为了放在底部是斑马线扫描结束  3 && Ysite < 30
        {

            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }

    }
}

void Search_Border_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    ImageStatus.WhiteLine_L = 0;
    ImageStatus.WhiteLine_R = 0;
    //ImageStatus.OFFLine = 1;
    /*封上下边界处理*/
    for (int Xsite = 0; Xsite < LCDW; Xsite++)
    {
        imageInput[0][Xsite] = 0;
        imageInput[Bottonline + 1][Xsite] = 0;
    }
    /*封左右边界处理*/
    for (int Ysite = 0; Ysite < LCDH; Ysite++)
    {
            ImageDeal[Ysite].LeftBoundary_First = 0;
            ImageDeal[Ysite].RightBoundary_First = 79;

            imageInput[Ysite][0] = 0;
            imageInput[Ysite][LCDW - 1] = 0;
    }
    /********获取底部边线*********/
    Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
    /********获取左右边线*********/
    Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);

    for (int Ysite = Bottonline; Ysite > ImageStatus.OFFLineBoundary + 1; Ysite--)
    {
        if (ImageDeal[Ysite].LeftBoundary < 3)
        {
            ImageStatus.WhiteLine_L++;
        }
        if (ImageDeal[Ysite].RightBoundary >= LCDW - 3)
        {
            ImageStatus.WhiteLine_R++;
        }
    }
}
uint8 LeftBorder_Error      = 0,RightBorder_Error      = 0;
uint8 LeftBorder_Error_Flag = 0,RightBorder_Error_Flag = 0;

//出现丢边的时候  重新确定无边行的中线
static void RouteFilter(void) {
//下面的范围需要自己去调防止过十字时边线出错误
//*********************************************************************************************************************//
//    for(Ysite = 58; Ysite >= (ImageStatus.OFFLine + 3);Ysite--)   //原本58
//    {
//        LeftBorder_Error_Flag = 0;
//        if (ImageDeal[Ysite].LeftBorder <= 15) {
//            LeftBorder_Error++;
//            if(LeftBorder_Error >= 35){
//                LeftBorder_Error_Flag = 1;
//            }
//        }
//    }
//    for(Ysite = 58; Ysite >= (ImageStatus.OFFLine + 3);Ysite--)   //原本58
//    {
//        RightBorder_Error_Flag = 0;
//        if (ImageDeal[Ysite].RightBorder >= 65) {
//            RightBorder_Error++;
//            if(RightBorder_Error >= 35){
//                RightBorder_Error_Flag = 1;
//            }
//        }
//    }
  for (Ysite = 58; Ysite >= (ImageStatus.OFFLine + 5);   //原本58
       Ysite--)                                     //从开始位到停止位
  {
    if (   ImageDeal[Ysite].IsLeftFind == 'W'
         &&ImageDeal[Ysite].IsRightFind == 'W'
         &&Ysite <= 45
         &&ImageDeal[Ysite - 1].IsLeftFind == 'W'
         &&ImageDeal[Ysite - 1].IsRightFind =='W')  //当前行左右都无边，而且在前45行   滤波
    {
      ytemp = Ysite;
      while (ytemp >= (ImageStatus.OFFLine))     // 改改试试，-6效果好一些   原本+5
      {
        ytemp--;
        if (  ImageDeal[ytemp].IsLeftFind == 'T'
            &&ImageDeal[ytemp].IsRightFind == 'T'
        ) //寻找两边都正常的，找到离本行最近的就不找了
        {
          DetR = (float)(ImageDeal[ytemp - 1].Center - ImageDeal[Ysite + 2].Center) /(float)(ytemp - 1 - Ysite - 2);          //算斜率
          int CenterTemp = ImageDeal[Ysite + 2].Center;
          int LineTemp = Ysite + 2;
          while (Ysite >= ytemp) {
                  ImageDeal[Ysite].Center =(int)(CenterTemp +DetR * (float)(Ysite - LineTemp));                                     //用斜率补
                  Ysite--;
          }
          break;
        }
      }
    }
    ImageDeal[Ysite].Center =(ImageDeal[Ysite - 1].Center + 2 * ImageDeal[Ysite].Center) /3;                                  //求平均，应该会比较滑  本来是上下两点平均
  }
}

//float aaaaa = 0;
SystemDatatypdef SystemData;
/*****************误差按权重重新整定**********************/
uint8 Tow_Point = 30;
void GetDet() {
  float DetTemp = 0;
  int TowPoint = 0;
//  float SpeedGain = 0;
  float UnitAll = 0;

  ImageStatus.TowPoint = Tow_Point;

//  SpeedGain=(Encoder_pre - ( Target_Speed )) * 0.1 + 0.5 ;//根据速度调整前瞻的因子   速度高于80  增益为负数  前瞻加长

//    if (SpeedGain >= 2)
//      SpeedGain = 2;
//    else if (SpeedGain <= -1)
//      SpeedGain = -1;

    //直道前瞻
//    else if(ImageStatus.Road_type == Straight)
//    TowPoint = SystemData.straighet_towpoint;
    //环岛前瞻
//    else if(ImageStatus.Road_type == LeftCirque || ImageStatus.Road_type == RightCirque)
  if(ImageStatus.Road_type == LeftCirque || ImageStatus.Road_type == RightCirque)
    {
      TowPoint = 18;
    }
    //没有元素根据速度调前瞻
    else //速度越快前瞻越长//动态前瞻
        TowPoint = ImageStatus.TowPoint;
//    TowPoint = ImageStatus.TowPoint - SpeedGain;

    //对前瞻值进行限幅
    if(TowPoint < ImageStatus.OFFLine)
    TowPoint = ImageStatus.OFFLine + 3;

    //当前方存在路障时截止行过大，防止小车过早转弯//或者说弯道时截至行极大
    if(ImageStatus.OFFLine > TowPoint-5)
        TowPoint = ImageStatus.OFFLine + 10;

    //前瞻值最大值限幅
    if (TowPoint >= 49)
    TowPoint = 49;

//    aaaaa = TowPoint;

    //对所取的中线上的点进行权重处理
    if((TowPoint - 5) >= ImageStatus.OFFLine) {                                          //前瞻取设定前瞻还是可视距离  需要分情况讨论
    for(int Ysite = (TowPoint - 5); Ysite < TowPoint; Ysite++) {
      DetTemp = DetTemp + Weighting[CLAMP_0_9(TowPoint - Ysite - 1)] * (ImageDeal[Ysite].Center);
      UnitAll = UnitAll + Weighting[CLAMP_0_9(TowPoint - Ysite - 1)];
    }
    for (Ysite = (TowPoint + 5); Ysite > TowPoint; Ysite--) {
      DetTemp += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)] * (ImageDeal[Ysite].Center);
      UnitAll += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)];
    }
    DetTemp = (ImageDeal[TowPoint].Center + DetTemp) / (UnitAll + 1);

  } else if (TowPoint > ImageStatus.OFFLine) {
    for (Ysite = ImageStatus.OFFLine; Ysite < TowPoint; Ysite++) {
      DetTemp += Weighting[CLAMP_0_9(TowPoint - Ysite - 1)] * (ImageDeal[Ysite].Center);
      UnitAll += Weighting[CLAMP_0_9(TowPoint - Ysite - 1)];
    }
    for (Ysite = (TowPoint + TowPoint - ImageStatus.OFFLine); Ysite > TowPoint;
         Ysite--) {
      DetTemp += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)] * (ImageDeal[Ysite].Center);
      UnitAll += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)];
    }
    DetTemp = (ImageDeal[Ysite].Center + DetTemp) / (UnitAll + 1);
  } else if (ImageStatus.OFFLine < 49) {
    for (Ysite = (ImageStatus.OFFLine + 3); Ysite > ImageStatus.OFFLine;
         Ysite--) {
      DetTemp += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)] * (ImageDeal[Ysite].Center);
      UnitAll += Weighting[CLAMP_0_9(-TowPoint + Ysite - 1)];
    }
    DetTemp = (ImageDeal[ImageStatus.OFFLine].Center + DetTemp) / (UnitAll + 1);

  } else
    DetTemp = (float)ImageStatus.Det_True;                                             //如果是出现OFFLine>50情况，保持上一次的偏差值

//记录前瞻的值
  ImageStatus.Det_True = DetTemp;                                                      //此时的解算出来的平均图像偏差

  ImageStatus.TowPoint_True = TowPoint;                                                //此时的前瞻
}



uint8 LeftBoundary_Error = 0,RightBoundary_Error = 0;
uint16 Straight_Left_Varance = 0,Straight_Right_Varance = 0;
uint16 Straight_Left_Varance1 = 0,Straight_Right_Varance1 = 0;

void Tracking(void){
    ImageStatus.OFFLine = 2;          //这个值根据真实距离得到，必须进行限制
    ImageStatus.WhiteLine = 0;
  for (Ysite = 59; Ysite >= ImageStatus.OFFLine; Ysite--) {
    //边界与标志位初始化
    ImageDeal[Ysite].IsLeftFind = 'F';
    ImageDeal[Ysite].IsRightFind = 'F';

    ImageDeal[Ysite].LeftBorder = 0;
    ImageDeal[Ysite].RightBorder = 79;

    ImageDeal[Ysite].LeftTemp = 0;
    ImageDeal[Ysite].RightTemp = 79;

    ImageDeal[Ysite].close_LeftBorder = 0;
    ImageDeal[Ysite].close_RightBorder = 79;

    LeftBoundary_Error = 0;
    RightBoundary_Error = 0;
  }

//寻找初始边线//斑马线改变扫描方式
  if(Zebra_Flag || Zebra_Time <= 5 || Zebra_Flag_Last == 1){
      DrawLinesFirst();}
  else{
      Search_StartPoint();}
//得到所有基本边线   8us
  DrawLinesProcess();
//八领域扫线
  Search_Border_OTSU(Pixel, LCDH, LCDW, LCDH - 2);

//丢线判断（采用八领域爬出来的边线进行辅助丢线判断）
  for(Ysite = 50; Ysite >= ImageStatus.OFFLine; Ysite--){
      if(ImageDeal[Ysite].LeftBoundary <= 5){
          LeftBoundary_Error++;
      }
      if(ImageDeal[Ysite].RightBoundary >= LCDW-6){
          RightBoundary_Error++;
      }
  }
  //直线
  Check_Straight();
//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环//圆环
  Check_Circle();
//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥//单边桥
//  if(Single_SideBridge_Flag == 0 && BIG_Obstacle_Flag ==0 && Zebra_Stop_Flag == 0 &&ImageFlag.image_element_rings==0 /* Single_Bridge_Flag_Last == 0;*/){
//      Check_Single_Bridge();
//  }
//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//横断路障//&& JUMP_START_FLAG==1
  if(BIG_Obstacle_Flag == 0 && Single_SideBridge_Flag == 0 && Zebra_Flag == 0 && Zebra_Stop_Flag==0 &&ImageFlag.image_element_rings ==0 && JUMP_START_FLAG==1 ){
      Check_BIG_Obstacle();
  }

  DrawExtensionLine();
  RouteFilter();

  //圆环处理
  Run_Circle();
  //单边桥处理
  if(Single_SideBridge_Flag)
  Run_Single_Bridge();

  //中线误差计算
  GetDet();
}
