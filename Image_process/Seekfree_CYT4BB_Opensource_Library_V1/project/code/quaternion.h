#ifndef _QUATERNION_H_                                          // 防止头文件被重复包含的宏定义开始
#define _QUATERNION_H_                                          // 定义头文件宏

#include "zf_common_headfile.h"                                 // 包含逐飞科技底层硬件驱动通用头文件
#include "small_driver_uart_control.h"                          // 引入无刷电机串口通信头文件

// ================== 算法限幅与常量定义 ==================
#define INTEGRAL_LIMIT_X          0.1f                          // X轴（Roll）陀螺仪积分误差限幅值，单位：弧度/秒
#define INTEGRAL_LIMIT_Y          0.1f                          // Y轴（Pitch）陀螺仪积分误差限幅值，单位：弧度/秒
#define INTEGRAL_LIMIT_Z          0.1f                          // Z轴（Yaw）陀螺仪积分误差限幅值，单位：弧度/秒
#define MAX_INTEGRAL_ERROR        0.2f                          // 积分总误差绝对值最高限幅，防止PI控制器积分饱和死锁
#define squa(Sq)                  (((float)Sq) * ((float)Sq))   // 定义求平方的快捷宏函数
#define DT                        (0.005f)                      // 定义姿态解算的执行周期（微分时间），5ms = 0.005s

// ================== 数学运算宏定义 ==================
#define absu16(Math_X)            ((Math_X) < 0 ? -(Math_X) : (Math_X))     // 定义16位整型绝对值求取宏
#define absFloat(Math_X)          ((Math_X) < 0 ? -(Math_X) : (Math_X))     // 定义浮点型绝对值求取宏
#define min(a, b)                 ((a) < (b) ? (a) : (b))                   // 定义求最小值的宏
#define max(a, b)                 ((a) > (b) ? (a) : (b))                   // 定义求最大值的宏
#define ABS(x)                    ((x) > 0 ? (x) : -(x))                    // 定义通用的绝对值求取宏
#define LIMIT(x, min, max)        ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x))) // 定义通用的数据区间限幅宏

// ================== 传感器原始数据读取宏 ==================
#define ACC_DATA_X                (imu660rb_acc_x)              // 映射底层IMU660RB的X轴加速度计原始整型数据
#define ACC_DATA_Y                (-imu660rb_acc_y)             // 映射Y轴加速度计数据（加负号为了调整传感器物理坐标系与车体一致）
#define ACC_DATA_Z                (-imu660rb_acc_z)             // 映射Z轴加速度计数据（加负号调整坐标系）
#define ACC_TRANSITION_FACTOR     (4098.0f)                     // 加速度计LSB转换系数：±8g量程下，除以4098得到标准重力加速度g

#define GYRO_DATA_X               (imu660rb_gyro_x)             // 映射底层IMU660RB的X轴陀螺仪原始整型数据
#define GYRO_DATA_Y               (-imu660rb_gyro_y)            // 映射Y轴陀螺仪数据（加负号调整旋转正方向）
#define GYRO_DATA_Z               (-imu660rb_gyro_z)            // 映射Z轴陀螺仪数据（加负号调整旋转正方向）
#define GYRO_TRANSITION_FACTOR    (14.30f)                      // 陀螺仪LSB转换系数：±2000dps量程下，除以14.30得到度每秒(°/s)

// ================== 核心数据结构体定义 ==================
typedef struct _icm_data_t                                      // 定义传感器物理量结构体
{                                                               // 结构体开始
    float accX;                                                 // 存放转换后的X轴加速度物理量（单位：g）
    float accY;                                                 // 存放转换后的Y轴加速度物理量（单位：g）
    float accZ;                                                 // 存放转换后的Z轴加速度物理量（单位：g）
    float gyroX;                                                // 存放转换后的X轴角速度物理量（单位：弧度/秒）
    float gyroY;                                                // 存放转换后的Y轴角速度物理量（单位：弧度/秒）
    float gyroZ;                                                // 存放转换后的Z轴角速度物理量（单位：弧度/秒）
} icm_data_t;                                                   // 命名该结构体类型为 icm_data_t

typedef struct _vector_t                                        // 定义空间三维向量结构体
{                                                               // 结构体开始
    float x;                                                    // 向量的X轴分量
    float y;                                                    // 向量的Y轴分量
    float z;                                                    // 向量的Z轴分量
} vector_t;                                                     // 命名该结构体类型为 vector_t

typedef struct _attitude_t                                      // 定义欧拉角姿态结构体
{                                                               // 结构体开始
    float roll;                                                 // 存放横滚角（左右倾斜角度）
    float pitch;                                                // 存放俯仰角（前后倾斜角度，平衡车核心控制角）
    float yaw;                                                  // 存放偏航角（车体水平旋转角度）
} attitude_t;                                                   // 命名该结构体类型为 attitude_t

// ================== 外部全局变量声明 ==================
extern icm_data_t IMU_TRAN;                                     // 声明外部结构体变量，用于在其他文件读取IMU物理数据
extern attitude_t g_attitude;                                   // 声明外部结构体变量，用于在其他文件读取最终计算出的姿态角

// ================== 外部核心函数声明 ==================
void imu_calibration(void);                                     // 声明陀螺仪静止零偏校准函数
void reset_yaw_continuity(void);                                // 声明重置偏航角连续化处理标志的函数
float q_rsqrt(float number);                                    // 声明快速平方根倒数算法函数（用于向量归一化）
void imu_get(void);                                             // 声明IMU数据获取与处理总函数（需要在5ms定时器中调用）
void data_normalization(icm_data_t *p_icm, attitude_t *p_angle, float dt); // 声明Mahony四元数姿态解算核心函数

#endif /* _QUATERNION_H_ */                                     // 结束头文件条件编译