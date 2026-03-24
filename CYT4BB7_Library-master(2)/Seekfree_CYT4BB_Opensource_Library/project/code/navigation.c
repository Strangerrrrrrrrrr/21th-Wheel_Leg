#include "navigation.h" 

waypoint_t path_buffer[MAX_WAYPOINTS];          
nav_state_e current_nav_state = NAV_STATE_IDLE; 
uint16 waypoint_count = 0;                      
float total_accumulated_distance = 0;           
float last_recorded_distance = 0;               // 记录上一次打点时的里程

uint16 playback_index = 0;                      
float nav_target_speed = 0.0f;                  
float nav_target_yaw = 0.0f;                    
float auto_playback_speed_setpoint =170.0f;    // 自动巡航发车速度

// 把内存里的点全部打包存进内部Flash里
static void simple_save_to_flash(void)
{
    flash_buffer_clear(); 

    for(int i = 0; i < waypoint_count; i++) 
    {
        flash_union_buffer[i * 2].float_type = path_buffer[i].yaw;            
        flash_union_buffer[i * 2 + 1].float_type = path_buffer[i].distance;  
    }
    
    flash_union_buffer[510].int32_type = waypoint_count; 
    flash_union_buffer[511].uint32_type = NAV_FLASH_MAGIC_NUM; 

    flash_write_page_from_buffer(0, NAV_FLASH_PAGE, 512); 
    printf("存点成功! 共记录了 %d 个点\r\n", waypoint_count); 
}

// 开机时自动从Flash读取上次没跑完的路径
void nav_init(void)
{
    flash_read_page_to_buffer(0, NAV_FLASH_PAGE, 512); 
    
    if(flash_union_buffer[511].uint32_type == NAV_FLASH_MAGIC_NUM) 
    {
        waypoint_count = flash_union_buffer[510].int32_type; 
        if(waypoint_count > MAX_WAYPOINTS) waypoint_count = MAX_WAYPOINTS; 
        
        for(int i = 0; i < waypoint_count; i++) 
        {
            path_buffer[i].yaw = flash_union_buffer[i * 2].float_type;          
            path_buffer[i].distance = flash_union_buffer[i * 2 + 1].float_type; 
        }
        printf("开机加载成功! 记忆中有 %d 个坐标点。\r\n", waypoint_count); 
    } 
    else 
    {
        waypoint_count = 0; 
    }
}

// 状态机切换函数
void nav_set_state(nav_state_e new_state)
{
    if (current_nav_state == new_state) return; 

    if (current_nav_state == NAV_STATE_RECORDING && new_state == NAV_STATE_IDLE) 
    {
        simple_save_to_flash(); 
    }

    if (new_state == NAV_STATE_RECORDING) 
    {
        waypoint_count = 0;             
        total_accumulated_distance = 0; 
        last_recorded_distance = 0;     // 重新录制时归零
        reset_yaw_continuity();         
        printf("--- 开始手推定距存点，请推车! ---\r\n"); 
    }

    if (new_state == NAV_STATE_PLAYBACK) 
    {
        if (waypoint_count == 0) 
        {
            printf("警告: 还没有存点，无法发车！\r\n"); 
            return; 
        }
        playback_index = 0;             
        total_accumulated_distance = 0; 
        reset_yaw_continuity();         
        printf("--- 自动发车！重现路径 ---\r\n"); 
    }

    current_nav_state = new_state; 
}

// 导航大脑心跳函数 (每20ms执行)
void nav_logic_tick(float current_enc_speed, float current_yaw)
{
    if (current_nav_state != NAV_STATE_IDLE) {
        total_accumulated_distance += current_enc_speed; 
    }

    // ================= 手推录制模式 (定距打点) =================
    if (current_nav_state == NAV_STATE_RECORDING) 
    {
        // 核心改造：当前里程和上次打点里程相差达到阈值，才丢下一颗“石子”
        if (ABS(total_accumulated_distance - last_recorded_distance) >= RECORD_INTERVAL_DISTANCE) 
        {
            if (waypoint_count < MAX_WAYPOINTS) 
            {
                path_buffer[waypoint_count].yaw = current_yaw;                      
                path_buffer[waypoint_count].distance = total_accumulated_distance;  
                waypoint_count++; 
                
                last_recorded_distance = total_accumulated_distance; // 更新打点基准位置
            }
            else 
            {
                printf("内存已满，自动结束存点！\r\n"); 
                nav_set_state(NAV_STATE_IDLE); 
            }
        }
    }
    // ================= 自动发车模式 =================
    else if (current_nav_state == NAV_STATE_PLAYBACK) 
    {
        while (playback_index < waypoint_count && 
               total_accumulated_distance >= path_buffer[playback_index].distance) 
        {
            playback_index++; 
        }

        if (playback_index < waypoint_count) 
        {
            nav_target_yaw = path_buffer[playback_index].yaw; 
            nav_target_speed = auto_playback_speed_setpoint;  
        } 
        else 
        {
            nav_target_speed = 0.0f; 
            nav_target_yaw = path_buffer[waypoint_count - 1].yaw; 
            printf("--- 到达终点，自动停车 ---\r\n"); 
            nav_set_state(NAV_STATE_IDLE); 
        }
    }
}