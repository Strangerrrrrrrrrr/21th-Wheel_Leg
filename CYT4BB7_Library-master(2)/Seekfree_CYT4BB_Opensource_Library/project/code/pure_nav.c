#include "pure_nav.h"                                  

PureWaypoint_t pure_path_buffer[MAX_WAYPOINTS];             
PureNavState_e pure_nav_state = PURE_NAV_IDLE;         
uint16 pure_waypoint_count = 0;                        

float pure_total_distance = 0.0f;                           
static float last_recorded_distance = 0.0f;                   

uint16 playback_index = 0;                             
float pure_nav_speed_out = 0.0f;                       
float pure_nav_yaw_out = 0.0f;                         

static float base_yaw = 0.0f;                 // 发车基准角
static float filtered_target_yaw = 0.0f;      // 平滑后的目标航向 (防打架核心)

void pure_nav_save_to_flash(void) {                 
    flash_buffer_clear();                              
    for(int i = 0; i < pure_waypoint_count; i++) {     
        flash_union_buffer[i * 2].float_type = pure_path_buffer[i].distance;      
        flash_union_buffer[i * 2 + 1].float_type = pure_path_buffer[i].yaw;  
    }                                                  
    flash_union_buffer[510].int32_type = pure_waypoint_count;         
    flash_union_buffer[511].uint32_type = NAV_FLASH_MAGIC_NUM;        

    if(flash_check(0, NAV_FLASH_PAGE)) {
        flash_erase_page(0, NAV_FLASH_PAGE);
    }
    flash_write_page_from_buffer(0, NAV_FLASH_PAGE, 512);             
    printf(">> 存点成功! 共记录了 %d 个 1D 坐标点\r\n", pure_waypoint_count); 
}                                                      

void pure_nav_init(void) {                             
    flash_read_page_to_buffer(0, NAV_FLASH_PAGE, 512); 
    if(flash_union_buffer[511].uint32_type == NAV_FLASH_MAGIC_NUM) {  
        pure_waypoint_count = flash_union_buffer[510].int32_type;     
        if(pure_waypoint_count > MAX_WAYPOINTS) pure_waypoint_count = MAX_WAYPOINTS; 
        for(int i = 0; i < pure_waypoint_count; i++) {                
            pure_path_buffer[i].distance = flash_union_buffer[i * 2].float_type;  
            pure_path_buffer[i].yaw = flash_union_buffer[i * 2 + 1].float_type;
        }                                                             
        printf(">> 开机加载成功! 记忆中有 %d 个坐标点。\r\n", pure_waypoint_count); 
    } else {                                                          
        pure_waypoint_count = 0;                                      
    }                                                                 
}                                                      

void pure_nav_set_state(PureNavState_e new_state) {    
    if (pure_nav_state == new_state) return;           

    if (new_state == PURE_NAV_RECORDING) {             
        pure_waypoint_count = 0;                       
        pure_total_distance = 0.0f;                         
        last_recorded_distance = 0.0f; 
        base_yaw = g_attitude.yaw;                       
        printf(">> 启动一维坐标系，开始记录...\r\n"); 
    }                                                  

    if (new_state == PURE_NAV_PLAYBACK) {              
        if (pure_waypoint_count == 0) {                
            printf(">> 警告: 没有存点，无法发车！\r\n"); 
            return;                                    
        }                                              
        playback_index = 0;                            
        pure_total_distance = 0.0f;                         
        pure_nav_speed_out = 0.0f; 
        base_yaw = g_attitude.yaw;   
        
        // 发车瞬间，重置平滑滤波器，防止起步瞬间猛烈甩头
        filtered_target_yaw = g_attitude.yaw; 
        
        printf(">> 自动发车！开始 1D 轨迹重现！\r\n"); 
    }                                                  

    pure_nav_state = new_state;                        
}                                                      

void pure_nav_logic_tick(float enc_speed_tick, float current_yaw) {
    if (pure_nav_state == PURE_NAV_IDLE) return;

    pure_total_distance += enc_speed_tick;

    float relative_yaw = current_yaw - base_yaw;
    while(relative_yaw > 180.0f) relative_yaw -= 360.0f;
    while(relative_yaw < -180.0f) relative_yaw += 360.0f;

    // ================= 录制打点模式 =================
    if (pure_nav_state == PURE_NAV_RECORDING) {        
        if (ABS(pure_total_distance - last_recorded_distance) >= RECORD_INTERVAL_DISTANCE) { 
            if (pure_waypoint_count < MAX_WAYPOINTS) {        
                pure_path_buffer[pure_waypoint_count].distance = pure_total_distance; 
                pure_path_buffer[pure_waypoint_count].yaw = relative_yaw; 
                pure_waypoint_count++;                               
                last_recorded_distance = pure_total_distance;                    
            } else {                                          
                printf(">> 内存已满，自动结束存点！\r\n");       
                pure_nav_set_state(PURE_NAV_IDLE);            
            }                                                 
        }                                                     
    }                                                         
    
    // ================= 自动一维寻迹发车模式 =================
    else if (pure_nav_state == PURE_NAV_PLAYBACK) {           
        
        while (playback_index < pure_waypoint_count && 
               pure_total_distance >= pure_path_buffer[playback_index].distance) 
        {
            playback_index++; 
        }
        
        if (playback_index < pure_waypoint_count) {               
            
            // 提取目标相对角度，还原为底层认得的绝对角度
            float raw_target_yaw = pure_path_buffer[playback_index].yaw + base_yaw;

            // 【防打架核心】：使用低通滤波，熨平坐标点之间产生的阶梯跃变，消灭方向盘突抽
            float yaw_diff = raw_target_yaw - filtered_target_yaw;
            while(yaw_diff > 180.0f) yaw_diff -= 360.0f;
            while(yaw_diff < -180.0f) yaw_diff += 360.0f;
            
            filtered_target_yaw += yaw_diff * 0.1f; // 0.1为平滑系数，值越小打方向盘越柔和
            pure_nav_yaw_out = filtered_target_yaw;

            // 柔性起步
            if (pure_nav_speed_out < AUTO_PLAYBACK_SPEED) {
                pure_nav_speed_out += 2.0f; 
            } else {
                pure_nav_speed_out = AUTO_PLAYBACK_SPEED;
            }
        } else {
            pure_nav_speed_out = 0.0f;                    
            pure_nav_yaw_out = current_yaw;               
            printf(">> 抵达终点，任务完成！\r\n");     
            pure_nav_set_state(PURE_NAV_IDLE);            
            return;                                       
        }                                                     
    }                                                         
}