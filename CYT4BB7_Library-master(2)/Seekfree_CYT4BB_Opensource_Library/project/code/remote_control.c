#include "remote_control.h"
#include "pure_nav.h" 

static int last_ch5_state = -1; 
static int last_ch4_state = 1;  
static int last_ch6_state = 0;  

float turn_out_global = 0;

static float map_rc_channel(int raw_val, int min_val, int center_val, int max_val, int deadzone) 
{
    if (raw_val > max_val) raw_val = max_val;
    if (raw_val < min_val) raw_val = min_val;

    if (raw_val > (center_val - deadzone) && raw_val < (center_val + deadzone)) return 0.0f; 

    if (raw_val >= center_val + deadzone) {
        return (float)(raw_val - (center_val + deadzone)) / (float)(max_val - (center_val + deadzone));
    } else {
        return (float)(raw_val - (center_val - deadzone)) / (float)((center_val - deadzone) - min_val);
    }
}

void remote_control_process(void)
{
    if (1 == uart_receiver.finsh_flag) 
    {
        if (1 == uart_receiver.state) 
        {
            int ch1_raw = uart_receiver.channel[0]; 
            int ch2_raw = uart_receiver.channel[1]; 
            int ch4_raw = uart_receiver.channel[3]; 
            int ch5_raw = uart_receiver.channel[4]; 
            int ch6_raw = uart_receiver.channel[5]; 
            
            int current_ch5_state = (ch5_raw > CH5_THRESHOLD) ? 1 : 0; 
            if (last_ch5_state == -1) {
                last_ch5_state = current_ch5_state; 
            } else {
                if (current_ch5_state == 1 && last_ch5_state == 0) {
                    Run_Flag = !Run_Flag; 
                }
                last_ch5_state = current_ch5_state; 
            }
            
            int current_ch4_state = 1; 
            if (ch4_raw < 600) current_ch4_state = 1;        
            else if (ch4_raw < 1400) current_ch4_state = 2;  
            else current_ch4_state = 3;                      

            if (current_ch4_state != last_ch4_state) {
                if (current_ch4_state == 1) {
                    pure_nav_set_state(PURE_NAV_IDLE);
                } 
                else if (current_ch4_state == 2) {
                    pure_nav_set_state(PURE_NAV_RECORDING);
                } 
                else if (current_ch4_state == 3) {
                    pure_nav_set_state(PURE_NAV_IDLE); 
                    pure_nav_save_to_flash();          
                }
                last_ch4_state = current_ch4_state;
            }

            int current_ch6_state = (ch6_raw > 1000) ? 1 : 0;
            if (current_ch6_state != last_ch6_state) {
                if (current_ch6_state == 1 && current_ch4_state == 1) {
                    pure_nav_set_state(PURE_NAV_PLAYBACK);
                } else {
                    if (pure_nav_state == PURE_NAV_PLAYBACK) {
                        pure_nav_set_state(PURE_NAV_IDLE);
                    }
                }
                last_ch6_state = current_ch6_state;
            }

            if (Run_Flag == 1) 
            {
                float forward_ratio = map_rc_channel(ch2_raw, CH2_MIN, CH2_CENTER, CH2_MAX, CH2_DEADZONE);
                float turn_ratio    = map_rc_channel(ch1_raw, CH1_MIN, CH1_CENTER, CH1_MAX, CH1_DEADZONE);
                
                forward_ratio = forward_ratio * forward_ratio * forward_ratio;
                
                float target_speed = forward_ratio * MAX_SPEED_TARGET;  
                float target_turn  = turn_ratio * MAX_TURN_TARGET;      
                
                speed_Temp      = (speed_Temp * 0.90f) + (target_speed * 0.10f);
                turn_out_global = (turn_out_global * 0.70f) + (target_turn * 0.30f);      
            } else {
                speed_Temp = 0.0f;
                turn_out_global = 0.0f;
            }
        }
        else 
        {
            Run_Flag = 0;     
            speed_Temp = 0.0f;  
            turn_out_global = 0.0f;  
        }
        uart_receiver.finsh_flag = 0; 
    }
}