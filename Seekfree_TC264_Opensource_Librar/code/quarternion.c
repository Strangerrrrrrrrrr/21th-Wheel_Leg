
#include "quarternion.h"
static quaternion_t NumQ = {1, 0, 0, 0};
float vecx_Z = 0 , vecy1_Z = 0 ,vecy2_Z = 0 , vecz1_Z = 0 , vecz2_Z = 0 ;
float wz_acc_tmp[2] = {0} ;
float Icm_return = 0 ;

float Motor_Icm[3];

float W_gyro_z = 0 ;
attitude_t g_attitude;
icm_data_t g_icm20607;

//私有变量区

const float MY_PI = 3.1415926535;
const float RtA = 57.2957795f;   //弧度（radian）转角度（angle）的常数
const float AtR = 0.0174532925f; //角度转弧度的常数
const float Gyro_G = 0.03051756f * 2;
const float Gyro_Gr = 0.0005326f * 2; // 角速度变成弧度    此参数对应陀螺2000度每秒0.0010653f
const float PI_2 = 1.570796f;         //PI/2

/* 返回正弦值，x为角度值 */
float sine(float x) // (-M_PI , M_PI) ???? 0.0005
{
    float Q = 0.775f;
    float P = 0.225f;
    float B = 1.273239544;
    float tmp_c = -0.405284f;
    float y = B * x + tmp_c * x * fabs(x);
    return (Q * y + P * y * fabs(y));
}

/* 计算余弦值，x为角度*/
float cosine(float x)
{
    return sine(x + MY_PI / 2);
}

/* 返回反正切值，输入反正切数据 */
float arctan(float x) //  (-1 , +1)
{
    float t = x;
    float result = 0;
    float X2 = x * x;
    unsigned char cnt = 1;
    do
    {
        result += t / ((cnt << 1) - 1);
        t = -t;
        t *= X2;
        cnt++;
    } while(cnt <= 6);

    return result;
}

/* 反正弦数据 */
float arcsin(float x)
{
    float d = 1;
    float t = x;
    unsigned char cnt = 1;
    float result = 0;
    float X2 = x * x;

    if(x >= 1.0f)
    {
        return PI_2;
    }
    if(x <= -1.0f)
    {
        return -PI_2;
    }
    do
    {
        result += t / (d * ((cnt << 1) - 1));
        t *= X2 * ((cnt << 1) - 1); //
        d *= (cnt << 1);            //2 4 6 8 10 ...
        cnt++;
    } while(cnt <= 6);

    return result;
}

/* 计算1/sqrt(x) */
float q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}

/* 数据限幅 */
float data_limit(float data, float toplimit, float lowerlimit)
{
    if(data > toplimit)
        data = toplimit;
    else if(data < lowerlimit)
        data = lowerlimit;
    return data;
}

/* 四元素融合角度 */
static void get_angle(attitude_t *p_angle)
{
    vecx_Z = 2.0f * NumQ.q0 * NumQ.q2 - 2.0f * NumQ.q1 * NumQ.q3; /*矩阵(3,1)项*/                   //地理坐标系下的roll轴的重力分量

    vecy1_Z = 2.0f * NumQ.q2 * NumQ.q3 + 2.0f * NumQ.q0 * NumQ.q1; /*矩阵(3,2)项*/                  //地理坐标系下的pitch轴的重力分量
    vecy2_Z =   - 2.0f * NumQ.q1 * NumQ.q1 - 2.0f * NumQ.q2 * NumQ.q2 + 1.0f;

    vecz1_Z = 2.0f * NumQ.q1 * NumQ.q2 + 2.0f * NumQ.q0 * NumQ.q3; /*矩阵(3,3)项*/                  //地理坐标系下的Z轴的重力分量
    vecz2_Z = -2.0f * NumQ.q3 * NumQ.q3 - 2.0f * NumQ.q2 * NumQ.q2 + 1.0f;

//    p_angle->pitch = asin(vecx_Z) * RtA;             //俯仰角
//    p_angle->roll = atan2f(vecy1_Z , vecy2_Z) * RtA;     //横滚角
//    p_angle->yaw = atan2f(vecz1_Z , vecz2_Z) * RtA;

    g_attitude.pitch = ( asin(vecx_Z) * RtA );             //俯仰角
    g_attitude.roll  = ( atan2f(vecy1_Z , vecy2_Z) * RtA )  ;     //横滚角
    g_attitude.yaw   = ( atan2f(vecz1_Z , vecz2_Z) * RtA ) ;

}

/* 重置四元素 */
void reset_quaternion(void)
{
    NumQ.q0 = 1.0;
    NumQ.q1 = 0.0;
    NumQ.q2 = 0.0;
    NumQ.q3 = 0.0;
}

/* 四元素获取  dt：5MS左右 */
void get_attitude_angle(icm_data_t *p_icm, attitude_t *p_angle, float dt)
{
    vector_t Gravity, Acc, Gyro, AccGravity;
    static vector_t GyroIntegError = {0};
    static float KpDef = 0.8f;
    static float KiDef = 0.0003f;
    float q0_t, q1_t, q2_t, q3_t;
    float NormQuat;
    float HalfTime = dt * 0.5f;

    Gravity.x = 2 * (NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);
    Gravity.y = 2 * (NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);
    Gravity.z = 1 - 2 * (NumQ.q1 * NumQ.q1 + NumQ.q2 * NumQ.q2);
    // 加速度归一化，
    NormQuat = q_rsqrt(squa(p_icm->accX)+ squa(p_icm->accY) +squa(p_icm->accZ));

    //归一后可化为单位向量下方向分量
    Acc.x = p_icm->accX * NormQuat;
    Acc.y = p_icm->accY * NormQuat;
    Acc.z = p_icm->accZ * NormQuat;

    //向量叉乘得出的值，叉乘后可以得到旋转矩阵的重力分量在新的加速度分量上的偏差
    AccGravity.x = (Acc.y * Gravity.z - Acc.z * Gravity.y);
    AccGravity.y = (Acc.z * Gravity.x - Acc.x * Gravity.z);
    AccGravity.z = (Acc.x * Gravity.y - Acc.y * Gravity.x);

    GyroIntegError.x += AccGravity.x * KiDef;
    GyroIntegError.y += AccGravity.y * KiDef;
    GyroIntegError.z += AccGravity.z * KiDef;

    //角速度融合加速度比例补偿值，与上面三句共同形成了PI补偿，得到矫正后的角速度值
    Gyro.x = p_icm->gyroX * Gyro_Gr + KpDef * AccGravity.x + GyroIntegError.x; //弧度制，此处补偿的是角速度的漂移
    Gyro.y = p_icm->gyroY * Gyro_Gr + KpDef * AccGravity.y + GyroIntegError.y;
    Gyro.z = p_icm->gyroZ * Gyro_Gr + KpDef * AccGravity.z + GyroIntegError.z;
    // 一阶龙格库塔法, 更新四元数
    //矫正后的角速度值积分，得到两次姿态解算中四元数一个实部Q0，三个虚部Q1~3的值的变化
    q0_t = (-NumQ.q1 * Gyro.x - NumQ.q2 * Gyro.y - NumQ.q3 * Gyro.z) * HalfTime;
    q1_t = (NumQ.q0 * Gyro.x - NumQ.q3 * Gyro.y + NumQ.q2 * Gyro.z) * HalfTime;
    q2_t = (NumQ.q3 * Gyro.x + NumQ.q0 * Gyro.y - NumQ.q1 * Gyro.z) * HalfTime;
    q3_t = (-NumQ.q2 * Gyro.x + NumQ.q1 * Gyro.y + NumQ.q0 * Gyro.z) * HalfTime;

    //积分后的值累加到上次的四元数中，即新的四元数
    NumQ.q0 += q0_t;
    NumQ.q1 += q1_t;
    NumQ.q2 += q2_t;
    NumQ.q3 += q3_t;

    // 重新四元数归一化，得到单位向量下
    NormQuat = q_rsqrt(squa(NumQ.q0) + squa(NumQ.q1) + squa(NumQ.q2) + squa(NumQ.q3)); //得到四元数的模长
    NumQ.q0 *= NormQuat;                                                               //模长更新四元数值
    NumQ.q1 *= NormQuat;
    NumQ.q2 *= NormQuat;
    NumQ.q3 *= NormQuat;

    /* 计算姿态角 */
    get_angle(p_angle);
}
/***********陀螺仪6数据获取*****************/
void Icm_Data(void)
{
    //获取加速度数据
    imu660ra_get_acc();

    //在角速度环已经获取，所以使用时进行了屏蔽  //逐飞的滤波
//     imu660ra_get_gyro();
//     imu660ra_gyro_y= imu660ra_gyro_y+2;
//
//    if(func_abs( imu660ra_gyro_y)<=5)
//    {
//        imu660ra_gyro_y=0;
//
//    }
//
//    if(func_abs( imu660ra_gyro_z)<=5)
//    {
//        imu660ra_gyro_z=0;
//
//    }
//    if(func_abs( imu660ra_gyro_x)<=5)
//    {
//        imu660ra_gyro_x=0;
//
//    }

    g_icm20607.accX=imu660ra_acc_x;
    g_icm20607.accY=imu660ra_acc_y;
    g_icm20607.accZ=imu660ra_acc_z;

    g_icm20607.gyroX=imu660ra_gyro_x;
    g_icm20607.gyroY=imu660ra_gyro_y;
    g_icm20607.gyroZ=imu660ra_gyro_z;

    get_attitude_angle(&g_icm20607, &g_attitude, DT); // 四元素算法


}

int Icm_Control(float next_ICM)
{
    static float P , D , Set_Icm_x=0 , Icm_x_error , Derror_ICM,errorlast_ICM;


    Icm_x_error = Set_Icm_x - next_ICM;
    P=Motor_Icm[1];
    D=Motor_Icm[2];
    Icm_return = P * Icm_x_error + D * (Derror_ICM - errorlast_ICM);
    errorlast_ICM = Derror_ICM;
    Derror_ICM = Icm_x_error;
    return Icm_return;
}
LowPassFilter  lpf_current_gyro_y;
LowPassFilter  lpf_current_gyro_z;
LowPassFilter  lpf_current_Center;

//对陀螺仪角速度数据进行低通滤波//调用前使用
void lpf_init(void)
{
    lpf_current_gyro_y.Tf = 0.08f;
    lpf_current_gyro_y.y_prev = 0;
}

void lpf_init_z(void)
{
    lpf_current_gyro_z.Tf = 0.08f;
    lpf_current_gyro_z.y_prev = 0;
}

float lpf_operator(LowPassFilter* LPF,float x ,float dt)
{
    float alpha = 0, y = 0;
    alpha = LPF->Tf / (LPF->Tf + dt);
    y = alpha * LPF->y_prev + (1.0f - alpha) * x;
    LPF->y_prev = y;
    return y;
}
































