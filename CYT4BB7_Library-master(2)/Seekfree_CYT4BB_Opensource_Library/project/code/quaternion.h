#ifndef _QUATERNION_H_
#define _QUATERNION_H_
#include "zf_common_headfile.h"

#define INTEGRAL_LIMIT_X 0.1f   // X轴积分限幅，单位：弧度/秒
#define INTEGRAL_LIMIT_Y 0.1f   // Y轴积分限幅，单位：弧度/秒
#define INTEGRAL_LIMIT_Z 0.1f   // Z轴积分限幅，单位：弧度/秒
#define MAX_INTEGRAL_ERROR 0.2f // 积分总误差限幅
#define squa(Sq) (((float)Sq) * ((float)Sq))
#define DT              (0.001)             //微分时间，一般取0.005

#define absu16(Math_X) ((Math_X) < 0 ? -(Math_X) : (Math_X))
#define absFloat(Math_X) ((Math_X) < 0 ? -(Math_X) : (Math_X))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))



#define  ACC_DATA_X                               (imu660rb_acc_x)         //加速度计原始数据
#define  ACC_DATA_Y                               (-imu660rb_acc_y)        //加速度计原始数据（方向位置）
#define  ACC_DATA_Z                               (-imu660rb_acc_z)       //加速度计原始数据（方向位置）
#define  ACC_TRANSITION_FACTOR                    (4098.0f)             //加速度计数据转换系数（LSB/g）
#define  ACC_GRAVITY                              (9.80665f)             //重力加速度参考值（9.8m/2S）


#define  GYRO_DATA_X                              (imu660rb_gyro_x)         //陀螺仪原始数据
#define  GYRO_DATA_Y                              (-imu660rb_gyro_y)         //陀螺仪原始数（旋转方向位置）
#define  GYRO_DATA_Z                              (-imu660rb_gyro_z)         //陀螺仪原始数据（旋转方向位置）
#define  GYRO_TRANSITION_FACTOR                   (14.30f)                 //陀螺仪数据转换系数（LSB/g）
//原始数据转化之后的菜单sss
typedef struct _icm_data_t   //转换之后的物理量
{
    float accX;
    float accY;
    float accZ;
    float gyroX;
    float gyroY;
    float gyroZ;

} icm_data_t;
//向量菜单
typedef struct _vector_t
{
    float x;
    float y;
    float z;
} vector_t;

typedef struct _attitude_t
{
    float roll;
    float pitch;
    float yaw;
} attitude_t;


 extern float arcsinf1(float x);
 extern float arctanf(float x) ;
 extern icm_data_t IMU_TRAN;
 extern attitude_t g_attitude;
 extern void gyro_calibration(void);



float q_rsqrt(float number);
void imu_get(void);
void data_normalization(icm_data_t *p_icm, attitude_t *p_angle, float dt);

#endif /* _QUATERNION_H_ */

