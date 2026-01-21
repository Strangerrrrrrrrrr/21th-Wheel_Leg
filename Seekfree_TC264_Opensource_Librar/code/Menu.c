#include "Menu.h"

uint32 time_image=0;
//开关状态变量
bool Key1_result;
bool Key2_result;
bool Key3_result;
bool Key4_result;

//开关状态变量
bool key1_status = 1;
bool key2_status = 1;
bool key3_status = 1;
bool key4_status = 1;

//上一次开关状态变量
bool key1_last_status;
bool key2_last_status;
bool key3_last_status;
bool key4_last_status;

bool clear_flag = 0;
//一级菜单
bool menu_level = 0;//是否进入二级菜单
unsigned char menu_index = 0;//光标x坐标
char menustr[20];  //存放字节的数组

//二级菜单
unsigned char menu2_index = 0;//光标x坐标
unsigned char menu2_mode = 0;//二级菜单所处形式 整形 浮点型
unsigned char menu2_limit = 0;//子菜单内选项个数
bool menu2_level = 0; //调整参数的标志位

int image_flag[2] = {0};//摄像头
int check_flag = 0;//发车检查
int fan_flag=0;  //风扇
int key_run_flag = 0;//遥控发车
int out_flag = 0 ;//出界
//  修改倍数相关
unsigned char magindex = 0;  //给下面数组使用
float magindex_use = 3;
float mag[] = {0.01,0.1, 1, 10 ,100};

//数值修改相关
int32 *intvalue[MAX_MENU2_NUM];
float *floatvalue[MAX_MENU2_NUM];
int *swflag[MAX_MENU2_NUM];//切换标志位 例如发车 显示图像等
//flash保存
unsigned char flash_index = 0;
uint32 flash_memory[data_num];  //flash存放的数组

// 一级菜单指针函数
void (*menu_pfc[])(unsigned char) = {menu_select, menu2_select};
// 二级菜单指针函数
char (*amenu2_init_pfc[])(char) =
{
menu2_flag,
menu2_Control_B,
menu2_Control_T,
menu2_image,
menu2_Start,
};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     button_init 按键初始化
// 参数说明     无
// 返回参数
// 使用示例     button_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void button_init(void)
{
    //按键开关
    gpio_init(P20_6, GPI, 1, GPI_PULL_UP);
    gpio_init(P20_7, GPI, 1, GPI_PULL_UP);
    gpio_init(P11_2, GPI, 1, GPI_PULL_UP);
    gpio_init(P11_3, GPI, 1, GPI_PULL_UP);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Key_entry 获取按键状态
// 参数说明
// 返回参数     无
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Key_entry(void)
{
    key1_last_status = key1_status;
    key2_last_status = key2_status;
    key3_last_status = key3_status;
    key4_last_status = key4_status;

    //读取当前按键状态
    key1_status = gpio_get_level(P20_6);
    key2_status = gpio_get_level(P20_7);
    key3_status = gpio_get_level(P11_2);
    key4_status = gpio_get_level(P11_3);

     if      (key1_status && !key1_last_status) Key1_result = 1;
     else if (key2_status && !key2_last_status) Key2_result = 1;
     else if (key3_status && !key3_last_status) Key3_result = 1;
     else if (key4_status && !key4_last_status) Key4_result = 1;

}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     menu_entry  一级菜单按键
// 参数说明
// 返回参数
// 使用示例     menu_entry();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu_entry(void)
{
    // 按下key1
    if (Key1_result) {
        Key1_result = 0;
        menu_pfc[menu_level](1);
    }
    //按下key2
    else if (Key2_result) {
        Key2_result = 0;
        menu_pfc[menu_level](2);
    }
    //按下key3
    else  if (Key3_result) {
        Key3_result = 0;
        menu_pfc[menu_level](3);
    }
    //按下key4
    else if (Key4_result) {
        Key4_result = 0;
        menu_pfc[menu_level](4);
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     一级菜单按键功能
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu_select(unsigned char event)
{
    if (!menu_level) {
        switch (event) {
            case 1://选项上移
                if (menu_index > 0) menu_index--;
                else menu_index = ROWS - 1;
                break;
            case 2://选项下移
                if (menu_index < ROWS - 1) menu_index++;
                else menu_index = 0;
                break;
            case 3://进入二级菜单
                menu2_init();
                menu_level = 1;
                return;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     menu2_init  二级菜单初始化
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu2_init()
{
    //  变量定义及初始化
    magindex = magindex_use;
    menu2_level = 0;
    menu2_index = 0;
    menu2_limit = 0;
    clear_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       modify 数值修改模块
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
static void modify(unsigned char event)
{
    switch (menu2_mode)
    {
        case PARASET_F:
            if (event) *floatvalue[menu2_index] += mag[magindex];//数值增加
            else *floatvalue[menu2_index] -= mag[magindex];//数值减少
            break;
        case PARASET_S:
            if (event) *intvalue[menu2_index] += mag[magindex];
            else *intvalue[menu2_index] -= mag[magindex];
            break;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介         ex_int2float 使用uinon联合体 将 long int 类型的整数值转换为与之等价的 float 类型的浮点数值。
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float ex_int2float(long int value)
{
//  定义联合体
    union {
        float float_value;
        unsigned long int_value;
    } c;
//  存储数据转换
    c.int_value = value;
    return c.float_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      ex_float2int 使用uinon联合体 将 float 类型的整数值转换为与之等价的 long int 类型的浮点数值。
// 参数说明
// 返回参数      long int 型变量
// 使用示例      para_reset(aaa);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
long int ex_float2int(float value)
{
//  定义联合体
    union
    {
        float float_value;
        unsigned long int_value;
    } c;
//  存储数据转换
    c.float_value = value;
    return c.int_value;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     para_reset 初次上电 保存数据
// 参数说明     saveflag 是否保存
// 返回参数     无
// 使用示例     para_reset(1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void para_reset(char saveflag)
{
    //saveflag: 是否存储
    register short i, j;
    for(i = 0; i < ROWS; i++)
    {
        amenu2_init_pfc[i](FLASH_INIT);//菜单初始化
        if(menu2_mode == PARASET_F || menu2_mode == PARASET_S)
        {
            amenu2_init_pfc[i](FLASH_INIT);//初始化参数值
        //  将参数写入数组
            switch(menu2_mode){
                case PARASET_F:
                    for(j = 0; j < menu2_limit; j++)
                        flash_memory[flash_index*MAX_MENU2_NUM+j] = ex_float2int(*floatvalue[j]);//相应变量的数值存到flash里面
                    break;
                case PARASET_S:
                    for(j = 0; j < menu2_limit; j++)
//                        flash_memory[flash_index*5+j] = ex_float2int(*shortvalue[j]);
                        flash_memory[flash_index*MAX_MENU2_NUM+j] =*intvalue[j];
                    break;
            }
        }
    }
    if(saveflag)
    {
            flash_write_page(0, 0, flash_memory, data_num);//存入数据
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     paraflash_init flash 初始化 第一次上电存储数据，之后上电从flash中提取数据
// 参数说明
// 返回参数     无
// 使用示例
// 备注信息     flash_index*5  5个一组 防止数据冲突
//-------------------------------------------------------------------------------------------------------------------
void paraflash_init(void)
{
//  变量定义
    register short i, j;
   if( !flash_check(0, 0))//  确认是否有提前保存的参数
   // if(1)
         para_reset(1);
    else{//读取、启用参数
        flash_read_page(0,0,flash_memory, data_num);//读取0 页到flash_memory数组中
        for(i = 0; i < ROWS; i++) {
            amenu2_init_pfc[i](0);//菜单初始化
            if(menu2_mode == PARASET_F || menu2_mode == PARASET_S){
            //  单个写入参数
                switch(menu2_mode){
                    case PARASET_F:
                        for(j = 0; j < menu2_limit; j++)
                            *floatvalue[j] = ex_int2float(flash_memory[flash_index*MAX_MENU2_NUM+j]);//这个一步骤就是将flash里面的数值给到相应变量 每5个是一个函数里面的变量
                        break;
                    case PARASET_S:
                        for(j = 0; j < menu2_limit; j++)
                            *intvalue[j] = flash_memory[flash_index*MAX_MENU2_NUM+j];
                        break;
                }
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     flash_memory_write 每次数据更改后按下确认键 就会将数据保存到flash里
// 参数说明
// 返回参数     无
// 使用示例     flash_memory_write();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void flash_memory_write(void)
{
//  变量定义
    register short i;
//  将参数写入数组
    switch(menu2_mode){
        case PARASET_F:
            for(i = 0; i < menu2_limit; i++)
                flash_memory[flash_index*MAX_MENU2_NUM+i] = ex_float2int(*floatvalue[i]);
            break;
        case PARASET_S:
            for(i = 0; i < menu2_limit; i++)
                flash_memory[flash_index*MAX_MENU2_NUM+i] = *intvalue[i];
            break;
        case BUTTONSW:
            *swflag[menu2_index] = !*swflag[menu2_index];
            break;
    }
    flash_erase_page(0,0);//清除扇区
    flash_write_page(0, 0, flash_memory, data_num);//存入数据
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu2_select(unsigned char event)
{
    if (!menu2_level){
        switch (event){
            case 1:
                if (menu2_index > 0) menu2_index--;
                break;
            case 2:
                if (menu2_index < menu2_limit) menu2_index++;
                break;
            case 3:
                menu2_level = 1;
                break;
        }
    }
    else{
        switch (event){
            case 1:
                modify(1);
                break; //数值增
            case 2:
                modify(0);
                break; //数值减
            case 3: //确定键
                clear_flag = 1;
                menu2_level = 0;
                flash_memory_write();

                break;
            case 4: //增值倍数修改
                magindex++;
                if (magindex > 4) magindex = 0;
                break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu_display(void)
{
    register char i;
    if (!menu_level) {
        for (i = 0; i < ROWS; i++) {
            if (i == menu_index)
                ips200_set_color(RGB565_RED,RGB565_BLUE);

            else
                ips200_set_color(RGB565_RED,RGB565_YELLOW);

            info_found(i);
            ips200_show_string(0, i*16, menustr);
            ips200_set_color(RGB565_RED,RGB565_YELLOW);

        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void menu2_display(void)
{
    for (unsigned char i = 0; i <= menu2_limit; i++){
        amenu2_init_pfc[menu_index](i);
        if (!menu2_level){
            if (i == menu2_index)
                ips200_set_color(RGB565_RED,RGB565_BLUE);

            else
                ips200_set_color(RGB565_RED,RGB565_YELLOW);

                ips200_show_string(0, i*16, menustr);
                ips200_set_color(RGB565_RED,RGB565_YELLOW);

        }
        else{
           if (menu2_mode == PARASET_S)
                ips200_show_int(75, menu2_index, *intvalue[menu2_index], 4);
           else if (menu2_mode == PARASET_F)
                ips200_show_float(75, menu2_index, *floatvalue[menu2_index], 5, 5);
           else if(menu2_mode == BUTTONSW)
                ips200_show_int(70, menu2_index, *swflag[menu2_index], 4);
        }
    }
    ips200_show_float(120,0, mag[magindex], 3,3); //显示倍数

     //返回一级菜单(放在二级菜单选择函数中有bug)
    if (menu2_index == menu2_limit){ //如果当前x轴坐标在最大值时候 就是会退出
        menu_level = 0;
        clear_flag = 1;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     一级菜单选项显示
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void info_found(unsigned char index)
{
    memset(menustr, 0, sizeof menustr);          //清空数组
    switch (index){
        case 0:
            strcpy(menustr, "Flag");
            break;
        case 1:
            strcpy(menustr, "Control_B");
            break;
        case 2:
            strcpy(menustr, "Control_T");
            break;
        case 3:
            strcpy(menustr, "image");
            break;
        case 4:
            strcpy(menustr, "menu2_Start");
            break;
//        case 5:
//            strcpy(menustr, "ele_on_off");
//            break;
    }
}




//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
char menu2_flag(char index)// 按钮切换模板
{
    menu2_mode = BUTTONSW;
    menu2_limit = 7;
    // 标志位数组
    swflag[0] = &Run_Flag;           //发车标志位
    swflag[1] = &Circle_Flag;        //环岛元素检测标志位
    swflag[2] = &Information_Flag;   //屏幕信息显示标志位
    swflag[3] = &Image_Flag;         //屏幕图像显示标志位
    swflag[4] = &Single_3cmBridge;         //屏幕图像显示标志位
    swflag[5] = &JUMP_START_FLAG;         //屏幕图像显示标志位
    swflag[6] = &Zebra_STOP_STOP;         //屏幕图像显示标志位
    switch(index){
    //  菜单属性初始化
        case 0://延时发车
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Run_Flag");
            return 0;
        case 1:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Circle_Flag");
            return 0;
        case 2:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Information_Flag");
            return 0;
        case 3:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Image_Flag");
            return 0;
        case 4:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Single_3cmBridge");
            return 0;
        case 5:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "JUMP_START_FLAG");
            return 0;
        case 6:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Zebra_STOP_STOP");
            return 0;
        case 7:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "exit");
            return 0;
        case FLASH_INIT:
            menu2_mode = BUTTONSW;
            return menu2_limit;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
char menu2_Control_B(char index)
{
    //菜单属性
    menu2_mode = PARASET_F;
    menu2_limit = 9;//显示和存储的数据的个数
    //  修改数值的地址
    floatvalue[0] = &speedring_KP;          //速度环
    floatvalue[1] = &speedring_KI;
    floatvalue[2] = &Angle_KP;             //角度环
    floatvalue[3] = &Angle_KD;
    floatvalue[4] = &Gyro_kP;              //角速度环
    floatvalue[5] = &GYRO_KD;
    floatvalue[6] = &SPEED_TO_ANGLE_GAIN;   //速度误差放大因子
    floatvalue[7] = &SERVO_ANGLE_INCREMENT; //舵机缓慢打角步进值
    floatvalue[8] = &Machine_Mid;
    //flash地址
    flash_index = 0;
    switch (index){
        case 0:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "speedring_KP");
            return 0;
        case 1:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "speedring_KI");
            return 0;
        case 2:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Angle_KP");
            return 0;
        case 3:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Angle_KD");
            return 0;
        case 4:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Gyro_kP");
            return 0;
        case 5:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "GYRO_KD");
            return 0;
        case 6:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Gain");
            return 0;
        case 7:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Step");
            return 0;
        case 8:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Machine_Mid");
            return 0;
        case 9:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "exit");
            return 0;
        case FLASH_INIT:
            menu2_mode = PARASET_F;
            return menu2_limit;
    }
    return 0;
}

char menu2_Control_T(char index)
{
    //菜单属性
    menu2_mode = PARASET_F;
    menu2_limit = 9;//显示和存储的数据的个数
    //  修改数值的地址
    floatvalue[0] = &speed_limit;
    floatvalue[1] = &Turn_KP;
    floatvalue[2] = &Turn_KP2;
    floatvalue[3] = &Turn_KD;
    floatvalue[4] = &Turn_KD2;
    floatvalue[5] = &TurnzZ_KP;
    floatvalue[6] = &TurnzZ_KP2;
    floatvalue[7] = &Turn_KDz2;
    floatvalue[8] = &Width_Offest;
    //flash地址
    flash_index = 1;//第几个二级菜单就是几
    switch (index){
        case 0:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "speed_limit");
            return 0;
        case 1:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Turn_KP");
            return 0;
        case 2:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Turn_KP2");
            return 0;
        case 3:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Turn_KD");
            return 0;
        case 4:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Turn_KD2");
            return 0;
        case 5:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "TurnzZ_KP");
            return 0;
        case 6:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "TurnzZ_KP2");
            return 0;
        case 7:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Turn_KDz2");
            return 0;
        case 8:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Width_Offest");
            return 0;
        case 9:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "exit");
            return 0;
        case FLASH_INIT:
            menu2_mode = PARASET_F;
            return menu2_limit;

    }
    return 0;
}

char menu2_image(char index)
{
    //菜单属性
    menu2_mode = PARASET_F;
    menu2_limit = 9;//显示和存储的数据的个数
    //  修改数值的地址
    floatvalue[0] = &compensation_value;
    floatvalue[1] = &Thresold_MIN;
    floatvalue[2] = &Thresold_MAX;
    floatvalue[3] = &Single_SideBridge_Velocity;
    floatvalue[4] = &OFFLine;
    floatvalue[5] = &Col_MAX;
    floatvalue[6] = &Obstacle_Encoder_OUT;
    floatvalue[7] = &S_V_Reduce;
    floatvalue[8] = &Exposure;

    //flash地址
    flash_index = 2;//第几个二级菜单就是几
    switch (index){
        case 0:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "compensation");
            return 0;
        case 1:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Thresold_MIN");
            return 0;
        case 2:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Thresold_MAX");
            return 0;
        case 3:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Single_SideBridge_Velocity");
            return 0;
        case 4:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "OFFLine");
            return 0;
        case 5:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Col_MAX");
            return 0;
        case 6:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "S_Encoder_OUT");
            return 0;
        case 7:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "S_V_Reduce");
            return 0;
        case 8:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Exposure");
            return 0;
        case 9:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "exit");
            return 0;
        case FLASH_INIT:
            menu2_mode = PARASET_F;
            return menu2_limit;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
char menu2_Start(char index)
{
    //菜单属性
    menu2_mode = PARASET_F;
    menu2_limit = 3;//显示和存储的数据的个数
      //修改数值的地址
    floatvalue[0] = &SPEED_INCREMENT;
    floatvalue[1] = &Left_OUTNUM;
    floatvalue[2] = &Right_OUTNUM;
    //flash地址
    flash_index = 3;//第几个二级菜单就是几
    switch (index){
        case 0:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "SPEED_INCREMENT");
            return 0;
        case 1:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Left_OUTNUM");
            return 0;
        case 2:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "Right_OUTNUM");
            return 0;
        case 3:
            memset(menustr, 0, sizeof menustr);          //清空数组
            strcpy(menustr, "exit");
            return 0;
        case FLASH_INIT:
            menu2_mode = PARASET_F;
            return menu2_limit;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介
// 参数说明
// 返回参数
// 使用示例
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void display_entry(void)
{
    if (clear_flag) {
        clear_flag = 0;
        ips200_clear();
    }
    if (!menu_level) menu_display();
    else menu2_display();
}





