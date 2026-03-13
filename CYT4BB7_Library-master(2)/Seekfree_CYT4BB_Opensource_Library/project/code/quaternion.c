#include "quaternion.h"                                         // 引入四元数运算的头文件定义
//花花草草猪小仙
// ================== 四元数与姿态解算全局变量 ==================
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;               // 初始化四元数，q0为实部，默认代表无旋转(1,0,0,0)
float vecx_Z = 0, vecy1_Z = 0, vecy2_Z = 0, vecz1_Z = 0, vecz2_Z = 0; // 定义计算欧拉角时所需的中间矩阵元素变量
float acc_normalize = 0;                                        // 定义用于加速度计向量归一化的中间变量
icm_data_t IMU_TRAN;                                            // 实例化IMU物理数据结构体，用于存储处理后的传感器数据
attitude_t g_attitude;                                          // 实例化欧拉角姿态结构体，用于存储最终输出的横滚、俯仰、偏航角

// ================== 陀螺仪零点偏移量变量 ==================
float gyro_offset_x = 0.0f;                                     // 存放X轴陀螺仪开机静止时的本底噪声偏差值
float gyro_offset_y = 0.0f;                                     // 存放Y轴陀螺仪开机静止时的本底噪声偏差值
float gyro_offset_z = 0.0f;                                     // 存放Z轴陀螺仪动态调整的零点偏差值

// ================== 偏航角(Yaw)连续化处理静态变量 ==================
static float last_yaw = 0.0f;                                   // 记录上一次解算出的原生偏航角，用于比对跳变
static float yaw_offset = 0.0f;                                 // 记录因为跨越180度边界而产生的累计补偿偏移量
static int first_yaw_flag = 1;                                  // 初始化标志位，1代表系统刚刚启动

// ================== 数学常量定义 ==================
const float MY_PI = 3.1415926535f;                              // 定义圆周率PI的常数值
const float RtA = 57.2957795f;                                  // 弧度转角度常数 (180/PI)
const float AtR = 0.0174532925f;                                // 角度转弧度常数 (PI/180)
const float PI_2 = 1.570796f;                                   // 定义 PI/2 的常数值

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     陀螺仪静止零偏校准函数 
// 备注信息     上电时必须等待传感器苏醒，再连续采样500次求绝对零偏！
//-------------------------------------------------------------------------------------------------------------------
void imu_calibration(void)                                      
{                                                               
    // 防止单片机启动太快，读到全是无效的0导致校准失效
    system_delay_ms(500);                                       

    int32 sum_gx = 0, sum_gy = 0, sum_gz = 0;                   // 定义累加变量防溢出
    for(int i = 0; i < 500; i++)                                // 开启连续500次采样
    {                                                           
        imu660rb_get_gyro();                                    // 抓取底层陀螺仪数据
        sum_gx += GYRO_DATA_X;                                  
        sum_gy += GYRO_DATA_Y;                                  
        sum_gz += GYRO_DATA_Z;                                  
        system_delay_ms(2);                                     // 每次延时2毫秒，总共耗时1秒钟
    }                                                           
    
    gyro_offset_x = (float)sum_gx / 500.0f;                     // 计算真正的静止绝对零偏
    gyro_offset_y = (float)sum_gy / 500.0f;                     
    gyro_offset_z = (float)sum_gz / 500.0f;                     
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     偏航角连续化处理函数
//-------------------------------------------------------------------------------------------------------------------
static float apply_yaw_continuity(float raw_yaw)                
{                                                               
    if (first_yaw_flag)                                         
    {                                                           
        last_yaw = raw_yaw;                                     
        first_yaw_flag = 0;                                     
        return raw_yaw;                                         
    }                                                           
    float diff = raw_yaw - last_yaw;                            
    if (diff > 180.0f) yaw_offset -= 360.0f;                    
    else if (diff < -180.0f) yaw_offset += 360.0f;              
    last_yaw = raw_yaw;                                         
    return raw_yaw + yaw_offset;                                
}                                                               

void reset_yaw_continuity(void)                                 
{                                                               
    first_yaw_flag = 1; yaw_offset = 0.0f; last_yaw = 0.0f;     
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     泰勒展开法求反正弦函数 (arcsin)
//-------------------------------------------------------------------------------------------------------------------
float arcsinf1(float x)                                         
{                                                               
    float d = 1, t = x, result = 0, X2 = x * x;                 
    unsigned char cnt = 1;                                      
    if(x >= 1.0f) return PI_2;                                  
    if(x <= -1.0f) return -PI_2;                                
    do                                                          
    {                                                           
        result += t / (d * ((cnt << 1) - 1));                   
        t *= X2 * ((cnt << 1) - 1);                             
        d *= (cnt << 1);                                        
        cnt++;                                                  
    } while(cnt <= 6);                                          
    return result;                                              
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     快速平方根倒数算法 1/sqrt(x) 
//-------------------------------------------------------------------------------------------------------------------
float q_rsqrt(float number)                                     
{                                                               
    long i; float x2, y; const float threehalfs = 1.5F;         
    x2 = number * 0.5F; y = number;                             
    i = *(long *)&y;                                            
    i = 0x5f3759df - (i >> 1);                                  
    y = *(float *)&i;                                           
    y = y * (threehalfs - (x2 * y * y));                        
    return y;                                                   
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从四元数中提取欧拉角
//-------------------------------------------------------------------------------------------------------------------
static void get_angle(attitude_t *p_angle)                      
{                                                               
    vecx_Z = 2.0f * q1 * q3 - 2.0f * q0 * q2;                   
    vecy1_Z = 2.0f * q0 * q1 + 2.0f * q2 * q3;                  
    vecy2_Z = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3 ;           
    vecz1_Z = 2.0f * q1 * q2 + 2.0f * q0  * q3;                 
    vecz2_Z = q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3 ;           

    g_attitude.pitch = -arcsinf1(vecx_Z) * RtA ;                
    g_attitude.roll  = atan2f(vecy1_Z ,vecy2_Z) * RtA  ;        
    float raw_yaw = atan2f(vecz1_Z , vecz2_Z) * RtA;            
    g_attitude.yaw = apply_yaw_continuity(raw_yaw);             
}                                                               

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Mahony 四元数姿态解算核心算法
//-------------------------------------------------------------------------------------------------------------------
void data_normalization(icm_data_t *p_icm, attitude_t *p_angle, float dt) 
{                                                                         
    float HalfTime = dt * 0.5f;                                           
    float q0_t, q1_t, q2_t, q3_t;                                         
    static float KpDef = 1.2f;                                            
    static float KiDef = 0.001f;                                          
    vector_t Gravity, AccGravity, Gyro;                                   
    static vector_t GyroIntegError = {0};                                 
    float NormQuat;                                                       
    
    // 1. 加速度计向量归一化
    acc_normalize = q_rsqrt(IMU_TRAN.accX * IMU_TRAN.accX + IMU_TRAN.accY * IMU_TRAN.accY + IMU_TRAN.accZ * IMU_TRAN.accZ); 
    IMU_TRAN.accX *= acc_normalize;                                       
    IMU_TRAN.accY *= acc_normalize;                                       
    IMU_TRAN.accZ *= acc_normalize;                                       

    // 2. 根据四元数推算理论重力投影
    Gravity.x = 2 * (q1 * q3 - q0 * q2);                                  
    Gravity.y = 2 * (q0 * q1 + q2 * q3);                                  
    Gravity.z = 1 - 2 * (q1 * q1 + q2 * q2);                              
        
    // 3. 叉乘计算偏差角误差
    AccGravity.x = (IMU_TRAN.accY * Gravity.z - IMU_TRAN.accZ * Gravity.y); 
    AccGravity.y = (IMU_TRAN.accZ * Gravity.x - IMU_TRAN.accX * Gravity.z); 
    
    //切断Z轴加速度错误纠偏，防止线性运动强扭Yaw角
    AccGravity.z = 0;                                                       
    
    // 4. PI误差积分
    GyroIntegError.x += AccGravity.x * KiDef;                               
    GyroIntegError.y += AccGravity.y * KiDef;                               
    GyroIntegError.z = 0;                                                   // 同样绝不允许Z轴积分纠偏
     
    GyroIntegError.x = LIMIT(GyroIntegError.x, -MAX_INTEGRAL_ERROR, MAX_INTEGRAL_ERROR); 
    GyroIntegError.y = LIMIT(GyroIntegError.y, -MAX_INTEGRAL_ERROR, MAX_INTEGRAL_ERROR); 
    
    // 5. 数据融合得到最终角速度
    Gyro.x = p_icm->gyroX + KpDef * AccGravity.x + GyroIntegError.x;        
    Gyro.y = p_icm->gyroY + KpDef * AccGravity.y + GyroIntegError.y;        
    Gyro.z = p_icm->gyroZ;                                                  // 100%信任陀螺仪Z轴
      
    // 6. 龙格库塔法刷新四元数
    q0_t = (-q1 * Gyro.x - q2 * Gyro.y - q3 * Gyro.z) * HalfTime;           
    q1_t = (q0  * Gyro.x - q3 * Gyro.y + q2 * Gyro.z) * HalfTime;           
    q2_t = (q3  * Gyro.x + q0 * Gyro.y - q1 * Gyro.z) * HalfTime;           
    q3_t = (-q2 * Gyro.x + q1 * Gyro.y + q0 * Gyro.z) * HalfTime;           
      
    q0 += q0_t; q1 += q1_t; q2 += q2_t; q3 += q3_t;                         
      
    // 7. 四元数重新归一化防止变形
    NormQuat = q_rsqrt(squa(q0) + squa(q1) + squa(q2) + squa(q3));          
    q0 *= NormQuat; q1 *= NormQuat; q2 *= NormQuat; q3 *= NormQuat;         
      
    get_angle(p_angle);                                                     
}                                                                           

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU数据获取与极限镇压预处理
// 备注修改     已解除运动学锁死，并将死区调小至0.8度/秒，恢复极速手动响应！
//-------------------------------------------------------------------------------------------------------------------
void imu_get(void)                                                          
{                                                                           
    imu660rb_get_acc();                                                     
    imu660rb_get_gyro();                                                    
    
    // ================= 加速度计预处理 =================
    IMU_TRAN.accX = (float)ACC_DATA_X / ACC_TRANSITION_FACTOR;              
    IMU_TRAN.accY = (float)ACC_DATA_Y / ACC_TRANSITION_FACTOR;              
    IMU_TRAN.accZ = (float)ACC_DATA_Z / ACC_TRANSITION_FACTOR;              
    
    static float acc_alpha = 0.2f;                                          
    static float accX_filtered = 0, accY_filtered = 0, accZ_filtered = 0;   
    
    accX_filtered = acc_alpha * accX_filtered + (1 - acc_alpha) * IMU_TRAN.accX;  
    accY_filtered = acc_alpha * accY_filtered + (1 - acc_alpha) * IMU_TRAN.accY;  
    accZ_filtered = acc_alpha * accZ_filtered + (1 - acc_alpha) * IMU_TRAN.accZ;  
    
    IMU_TRAN.accX = accX_filtered;                                          
    IMU_TRAN.accY = accY_filtered;                                          
    IMU_TRAN.accZ = accZ_filtered;                                          

    // ================= 陀螺仪预处理 =================
    
    IMU_TRAN.gyroX = ((float)GYRO_DATA_X - gyro_offset_x) / GYRO_TRANSITION_FACTOR * 0.01745329f; 
    IMU_TRAN.gyroY = ((float)GYRO_DATA_Y - gyro_offset_y) / GYRO_TRANSITION_FACTOR * 0.01745329f; 
    
    // 直观提取Z轴的 度/秒 (dps)
    float raw_gyro_z_dps = ((float)GYRO_DATA_Z - gyro_offset_z) / GYRO_TRANSITION_FACTOR;

    static float gyro_filtered_x = 0, gyro_filtered_y = 0, gyro_filtered_z = 0; 
    static int first_run = 1;                                               
    static const float FILTER_ALPHA = 0.2f;  

    if(first_run)                                                           
    {                                                                       
        gyro_filtered_x = IMU_TRAN.gyroX;                                   
        gyro_filtered_y = IMU_TRAN.gyroY;                                   
        gyro_filtered_z = raw_gyro_z_dps * 0.01745329f;                                   
        first_run = 0;                                                      
    }

    //【动态零偏追踪】
    // 角速度在 1.5度/秒 以内（判定为未发生有效转向，芯片温漂发疯）
    if(raw_gyro_z_dps > -1.5f && raw_gyro_z_dps < 1.5f)
    {
        // 缓慢将漂移量当成新“零点”给吃掉！0.0002f是极慢追踪阻尼
     gyro_offset_z += ((float)GYRO_DATA_Z - gyro_offset_z) * 0.0002f;
    }

    // 【放宽死区至 0.8度/秒，恢复手动旋转灵敏度】
    if(raw_gyro_z_dps > -0.8f && raw_gyro_z_dps < 0.8f)
    {
        IMU_TRAN.gyroZ = 0.0f;                                              // 1. 物理读数强制拉成完美的 0
        gyro_filtered_z = 0.0f;                                             // 2. 斩断滤波器历史记忆，防止残留噪声飘移！
    }
    else
    {
        // 超过0.8度/秒，真实响应您的手动旋转！
        float gyro_z_rad = raw_gyro_z_dps * 0.01745329f;
        gyro_filtered_z = FILTER_ALPHA * gyro_filtered_z + (1.0f - FILTER_ALPHA) * gyro_z_rad;
        IMU_TRAN.gyroZ = gyro_filtered_z;
    }


    
    // 对 X 和 Y 轴照常进行平滑滤波
    gyro_filtered_x = FILTER_ALPHA * gyro_filtered_x + (1.0f - FILTER_ALPHA) * IMU_TRAN.gyroX;    
    gyro_filtered_y = FILTER_ALPHA * gyro_filtered_y + (1.0f - FILTER_ALPHA) * IMU_TRAN.gyroY;    
    
    IMU_TRAN.gyroX = gyro_filtered_x;                                       
    IMU_TRAN.gyroY = gyro_filtered_y;                                       

    // ================= 执行姿态解算 =================
    data_normalization(&IMU_TRAN, &g_attitude, DT);                         
}