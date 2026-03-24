#ifndef _NAVIGATION_H_ 
#define _NAVIGATION_H_ 

#include "zf_common_headfile.h" 
#include "quaternion.h"         
#include "zf_driver_flash.h"    

// ================== 导航系统参数配置 ==================
#define MAX_WAYPOINTS              250         // 最大路径点数量（250个点刚好存满一页Flash）
#define RECORD_INTERVAL_DISTANCE   1000.0f      // 【核心标定参数】：每隔多远记录一个点？(请根据实测5cm对应的脉冲数修改此值！)
#define NAV_FLASH_PAGE             70          // 使用单片机内部Flash的第70页来保存数据
#define NAV_FLASH_MAGIC_NUM        0xAA55AA55  // 魔法特征码

typedef struct {            
    float yaw;              // 记录绝对连续航向角
    float distance;         // 记录累计行驶的编码器物理距离
} waypoint_t;               

typedef enum {              
    NAV_STATE_IDLE = 0,     // 待机模式
    NAV_STATE_RECORDING,    // 手推录制模式 (电机失电)
    NAV_STATE_PLAYBACK      // 自动发车回放模式 (电机通电闭环)
} nav_state_e;              

extern nav_state_e current_nav_state; 
extern float nav_target_speed;        
extern float nav_target_yaw;          
extern uint16 waypoint_count;         
extern float total_accumulated_distance; // 暴露给外部，方便你在屏幕或串口查看以标定 5cm 脉冲数

void nav_init(void);                                            
void nav_logic_tick(float current_enc_speed, float current_yaw);
void nav_set_state(nav_state_e new_state);                      

#endif