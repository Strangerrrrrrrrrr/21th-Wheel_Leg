#ifndef _PURE_NAV_H_                                   
#define _PURE_NAV_H_                                   

#include "zf_common_headfile.h"                        
#include "zf_driver_flash.h"                           
#include "math.h"                                      

// ================== 导航系统参数配置 ==================
#define MAX_WAYPOINTS              250                 
#define RECORD_INTERVAL_DISTANCE   1000.0f             // 打点间距脉冲数 (约5cm)
#define AUTO_PLAYBACK_SPEED        180.0f              // 自动回放发车时的基础速度
#define NAV_FLASH_PAGE             70                  
#define NAV_FLASH_MAGIC_NUM        0xAA55AA55          

// 一维坐标点结构体 (里程 + 航向)
typedef struct {                                       
    float distance;                                           
    float yaw;                                           
} PureWaypoint_t;                                      

// 导航状态机枚举
typedef enum {                                         
    PURE_NAV_IDLE = 0,                                 // 待机/正常遥控
    PURE_NAV_RECORDING,                                // 录制打点
    PURE_NAV_PLAYBACK                                  // 回放模式
} PureNavState_e;                              

extern PureNavState_e pure_nav_state;                  
extern float pure_nav_speed_out;                       
extern float pure_nav_yaw_out;                         
extern uint16 pure_waypoint_count;                     

void pure_nav_init(void);                                          
void pure_nav_set_state(PureNavState_e new_state);                 
void pure_nav_save_to_flash(void);                                 
void pure_nav_logic_tick(float enc_speed_tick, float current_yaw); 

#endif