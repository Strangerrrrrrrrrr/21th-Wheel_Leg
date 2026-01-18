// quarternion.h
#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "zf_common_headfile.h"

#define squa(Sq) (((float)Sq) * ((float)Sq))
#define absu16(Math_X) ((Math_X) < 0 ? -(Math_X) : (Math_X))
#define absFloat(Math_X) ((Math_X) < 0 ? -(Math_X) : (Math_X))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define DT              (0.010)             //微分时间，一般取0.005


//数据结构声明


extern float W_gyro_z;

//Extern引用
extern const float MY_PI;
extern const float AtR;
extern const float RtA;
extern const float Gyro_G;
extern const float Gyro_Gr;
extern float Icm_return;

//extern float Motor_Icm[3];

//函数声明
extern float safe_asin(float v);
extern float arcsin(float x);
extern float arctan(float x);
extern float sine(float x);
extern float cosine(float x);
extern float q_rsqrt(float number);
extern float VariableParameter(float error);

/* 向量 */
typedef struct _vector_t
{
    float x;
    float y;
    float z;
} vector_t;

/* 姿态角 */
typedef struct _attitude_t
{
    float roll;
    float pitch;
    float yaw;
} attitude_t;

typedef struct _quaternion_t
{ //四元数
    float q0;
    float q1;
    float q2;
    float q3;
} quaternion_t;

typedef struct _icm_data_t
{
    int accX;
    int accY;
    int accZ;
    int gyroX;
    int gyroY;
    int gyroZ;

    int Offset[6];
} icm_data_t;

typedef struct
{
    float Tf; //!< Low pass filter time constant
    float y_prev; //!< filtered value in previous execution step
} LowPassFilter;



extern LowPassFilter  lpf_current_gyro_y;
extern LowPassFilter  lpf_current_Center;
//对陀螺仪角速度数据进行低通滤波//调用前使用
void lpf_init(void);
float lpf_operator(LowPassFilter* LPF,float x ,float dt);




extern attitude_t g_attitude;
extern icm_data_t g_icm20607;

//函数声明
void get_attitude_angle(icm_data_t *p_icm, attitude_t *p_angle, float dt);
void reset_quaternion(void);
void Icm_Data(void);                                        /*陀螺仪数据获取并且均值滤波*/
int Icm_Control(float next_ICM);


#endif
