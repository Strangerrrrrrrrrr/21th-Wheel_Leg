/*********************************************************************************************************************
* 平衡车双模按键控制系统 - 中断驱动版
* 说明：按键状态机在10ms中断处理，平衡控制在1ms中断执行，完全分离高频/低频逻辑
********************************************************************************************************************/
#include "key_start_system.h"

static system_state_t current_state = STATE_DOWN;
static uint32_t press_timer = 0;
static bool key_pressed = false;

// 短按状态切换
static void switch_state(void) {
    switch(current_state) {
        case STATE_DOWN:
            current_state = STATE_BALANCE;
            speed_limit = 0;
            printf("[BALANCE] Standing up...\r\n");
            break;
        case STATE_BALANCE:
            current_state = STATE_RUNNING;
            speed_limit = 0;
            printf("[RUNNING] Moving forward at %.0f\r\n", TARGET_SPEED);
            break;
        case STATE_RUNNING:
            current_state = STATE_BALANCE;
            speed_limit = 0;
            printf("[BALANCE] Stopped moving, holding balance\r\n");
            break;
    }
}

// 长按处理：强制关机
static void emergency_shutdown(void) {
    if(current_state != STATE_DOWN) {
        current_state = STATE_DOWN;
        speed_limit = 0;
        printf("[SHUTDOWN] Balance disabled\r\n");
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键启动系统初始化
// 参数说明     void
// 返回参数     void
// 使用示例     key_start_system_init();
// 备注信息     初始化按键系统（10ms扫描周期），重置系统状态，打印操作提示信息
//-------------------------------------------------------------------------------------------------------------------
void key_start_system_init(void) {
    key_init(10);  // 初始化按键扫描基础（实际扫描由中断触发）
    current_state = STATE_DOWN;
    printf("Balance Vehicle Ready\r\n");
    printf("SHORT PRESS: Down→Balance→Run→Balance...\r\n");
    printf("LONG PRESS (2s): Emergency shutdown\r\n");
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键状态机处理（10ms中断调用）
// 参数说明     void
// 返回参数     void
// 使用示例     在10ms定时器中断服务函数中调用
// 备注信息     扫描按键状态，处理短按/长按事件，更新系统状态和目标速度；专用于低频按键逻辑
//-------------------------------------------------------------------------------------------------------------------
void key_state_machine_10ms(void) {
    key_scanner();  // 扫描按键（10ms周期调用，符合按键去抖需求）
    key_state_enum state = key_get_state(CONTROL_KEY_INDEX);
    
    if(state == KEY_LONG_PRESS) {
        if(!key_pressed) {
            key_pressed = true;
            press_timer = 0;
        }
        press_timer += 10;  // 每次中断累加10ms
        
        if(press_timer >= LONG_PRESS_TIME) {
            emergency_shutdown();
            key_pressed = false;
        }
    } 
    else if(key_pressed && state == KEY_RELEASE) {
        if(press_timer < LONG_PRESS_TIME) {
            switch_state();  // 短按触发状态切换
        }
        key_pressed = false;
        press_timer = 0;
    }
    else if(state == KEY_RELEASE) {
        key_pressed = false;
        press_timer = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     平衡控制执行（1ms中断调用）
// 参数说明     void
// 返回参数     void
// 使用示例     在1ms定时器中断服务函数中调用
// 备注信息     根据当前系统状态执行平衡控制或强制停机；高频执行确保平衡稳定性
//-------------------------------------------------------------------------------------------------------------------
void balance_control_1ms(void) {
    switch(current_state) {
        case STATE_DOWN:  // 倒地状态：强制停机
            speed_Temp = 0;
            small_driver_set_duty(0, 0);
            break;
        case STATE_BALANCE:  // 原地平衡：速度=0
        case STATE_RUNNING:  // 行进状态：按预设速度运行
            callback();  // 执行平衡控制核心算法（含角度环/速度环）
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取系统当前运行状态
// 参数说明     void
// 返回参数     system_state_t - 当前系统状态（STATE_DOWN/STATE_BALANCE/STATE_RUNNING）
// 使用示例     if(get_system_state() == STATE_RUNNING) { ... }
// 备注信息     返回系统当前状态，可用于外部逻辑判断或状态显示
//-------------------------------------------------------------------------------------------------------------------
system_state_t get_system_state(void) {
    return current_state;
}