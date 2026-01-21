/*
 * Menu.h
 *
 *  Created on: 2024年9月14日
 *      Author: 不懂
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_

#include "zf_common_headfile.h"

/**************************************************************************************************************************************/
/**************************************************************************************************************************************/
/**************************************************************************************************************************************/
/************************************使用修改*******************************************************************************************/
//使用说明
/*
 * menu.c
 *
 *  Created on: 2024年1月17日
 *      Author: 21335
 *先给所有PID之类的数值存储到
 *菜单修改方法：
 * 1.修改头文件宏定义row个数
 * 2.修改info_found
 * 3.修改二级菜单指针函数数组
 * 4.增加子菜单
 * 5.增加子菜单函数头文件
 * 6.子菜单内修改flash保存地址
 * 7.note:字菜单中可修改数据数目最多为4个，因为不想浪费太多flash_memory数组，可更改头文件MAX_MENU2_NUM
 */
#define MAX_MENU2_NUM 10 //二级菜单子函数总可修改数据最大数目
////一级菜单选项数量//必须改为对应使用的一级菜单数量（否则无法修改参数）
#define ROWS 5
//一级菜单显示入口//可更改一级菜单的显示数量
void info_found(unsigned char index);
//可更改刚进入菜单时显示的倍率//且这个数字目前最大不能超过4（可以更改）
extern float magindex_use;

//标志位声明
extern int image_flag[2]; //摄像头
extern int check_flag;    //发车检查
extern int out_flag;      //出界保护
extern int fan_flag;
extern uint32 time_image;

//使用时，必须调用的函数
void button_init(void);   //按键初始化  //可更改按键对应的IO口
void Key_entry(void);     //按键扫描入口//可更改按键对应的IO口
void menu_entry(void);    //菜单显示入口
void display_entry(void); //菜单显示入口
void paraflash_init(void);//读取flash当中数据//可更改对应扇区

//更改函数名时需改变对应的声明
char menu2_flag(char index);

char menu2_Control_B(char index);

char menu2_Control_T(char index);

char menu2_image(char index);

char menu2_Start(char index);

char menu2_ele_on_off(char index);

/**************************************************************************************************************************************/
/**************************************************************************************************************************************/
/**************************************************************************************************************************************/






#define data_num 100     //flash 存储数量
#define PARASET_F 0      //显示浮点型
#define PARASET_S 1      //显示短整型
#define BUTTONSW  2      //显示bool
#define FLASH_INIT 10    //flash 初始化

extern unsigned char menu_index;
extern unsigned char menu2_mode;
extern unsigned char menu2_limit;

extern int *intvalue[MAX_MENU2_NUM];
extern float *floatvalue[MAX_MENU2_NUM];
extern int *swflag[MAX_MENU2_NUM];//切换标志位 例如发车 显示图像等
extern unsigned char flash_index;
extern char (*amenu2_init_pfc[])(char);

void menu_select(unsigned char event);

void para_reset(char saveflag);

void para_saveall(void);

void menu2_select(unsigned char event);

void menu2_init(void);

float ex_int2float(long int value);

long int ex_float2int(float value);

void paraflash_read(void);

void flash_memory_write(void);

void menu_display(void);

void menu2_display(void);

#endif /* CODE_MENU_H_ */
