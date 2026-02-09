  //2026.2.8姿态解算更新
  //作者：花花草草猪小仙
  //重构代码计算逻辑



  #include "quaternion.h"
  float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
  float vecx_Z = 0 , vecy1_Z = 0 ,vecy2_Z = 0 , vecz1_Z = 0 , vecz2_Z = 0 ;
  float acc_normalize=0;
  icm_data_t IMU_TRAN;
  attitude_t g_attitude;

  //私有变量区

  const float MY_PI = 3.1415926535;
  const float RtA = 57.2957795f;   //弧度（radian）转角度（angle）的常数
  const float AtR = 0.0174532925f; //角度转弧度的常数
  const float Gyro_G = 0.03051756f * 2;
  const float Gyro_Gr = 0.001221730476f; ; // 角速度变成弧度    此参数对应陀螺2000度每秒0.0010653f
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
  float arctanf(float x) //  (-1 , +1)
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
  float arcsinf1(float x)
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

  //------------------------------------------//
  // 函数名: q_rsqrt
  // 函数简介: 快速计算1/sqrt(x)，使用Quake III平方根倒数算法通过位操作和牛顿迭代法，比标准库sqrtf函数快4倍左右
  // 参数说明: number - 输入的正浮点数，需要计算其平方根倒数
  // 返回参数: float - 返回1/sqrt(number)的近似值单次牛顿迭代后精度约0.2%
  // 使用示例: q_rsqrt(x);  
  //------------------------------------------//
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

 static float gyro_bias_x = 0, gyro_bias_y = 0, gyro_bias_z = 0;
static int calibration_samples = 0;
static const int CALIBRATION_COUNT = 1000;  // 校准采样次数

/* 陀螺仪零偏校准函数 */
void gyro_calibration(void)
{
    static float sum_x = 0, sum_y = 0, sum_z = 0;
    
    imu660rb_get_gyro();
    
    if (calibration_samples < CALIBRATION_COUNT) {
        sum_x += (float)GYRO_DATA_X / GYRO_TRANSITION_FACTOR * AtR;
        sum_y += (float)GYRO_DATA_Y / GYRO_TRANSITION_FACTOR * AtR;
        sum_z += (float)GYRO_DATA_Z / GYRO_TRANSITION_FACTOR * AtR;
        calibration_samples++;
        
        if (calibration_samples == CALIBRATION_COUNT) {
            gyro_bias_x = sum_x / CALIBRATION_COUNT;
            gyro_bias_y = sum_y / CALIBRATION_COUNT;
            gyro_bias_z = sum_z / CALIBRATION_COUNT;
          
        }
    }
}
 


  
  /* 四元素融合角度 */
  static void get_angle(attitude_t *p_angle)
  {
     vecx_Z = 2.0f * q1 * q3 - 2.0f * q0 * q2;             

     vecy1_Z = 2.0f * q0 * q1 + 2.0f * q2 * q3; 
     vecy2_Z = q0 * q0 -q1 *q1 -q2 *q2 +q3 *q3 ;

     vecz1_Z = 2.0f * q1 * q2 + 2.0f * q0  * q3; 
     vecz2_Z = q0 * q0 +q1 *q1 -q2 *q2 -q3 *q3 ;


     g_attitude.pitch = -arcsinf1(vecx_Z) * RtA ;             //俯仰角
     g_attitude.roll  = atan2f(vecy1_Z ,vecy2_Z) * RtA  ;     //横滚角
     g_attitude.yaw   = atan2f(vecz1_Z , vecz2_Z) * RtA  ;
      
      
     
  }
  //------------------------------------------//
  //函数简介:四元素获取  dt：5MS左右 */
  //返回参数：void
  //使用历程：data_normalization()；
  //备注    ：数据归一化/加速度计归一化，b坐标系下的重力分量
  void data_normalization(icm_data_t *p_icm, attitude_t *p_angle, float dt)
  {
    
    
     float HalfTime = dt * 0.5f;
     float q0_t, q1_t, q2_t, q3_t;
     static float KpDef = 0.7f;
     static float KiDef = 0.0003f;
     vector_t Gravity,AccGravity,Gyro;
     static vector_t GyroIntegError = {0};
     float NormQuat;
    //加速度计归一化，但是转化了数据后期需要验证是否考虑去除
     acc_normalize=q_rsqrt(IMU_TRAN.accX * IMU_TRAN.accX+IMU_TRAN.accY * IMU_TRAN.accY+IMU_TRAN.accZ * IMU_TRAN.accZ);
     IMU_TRAN.accX*=acc_normalize;
     IMU_TRAN.accY*=acc_normalize;
     IMU_TRAN.accZ*=acc_normalize;

    //提取姿态矩阵中的重力分量
     Gravity.x = 2 * (q1 * q3 - q0 * q2);
     Gravity.y = 2 * (q0 * q1 + q2 * q3);
     Gravity.z = 1 - 2 * (q1 * q1 + q2 * q2);
        
       //向量叉乘得出的值，叉乘后可以得到旋转矩阵的重力分量在新的加速度分量上的偏差
     AccGravity.x = (IMU_TRAN.accY * Gravity.z - IMU_TRAN.accZ * Gravity.y);
     AccGravity.y = (IMU_TRAN.accZ * Gravity.x - IMU_TRAN.accX * Gravity.z);
     AccGravity.z = (IMU_TRAN.accX * Gravity.y - IMU_TRAN.accY * Gravity.x);
      //对误差进行积分，从而消除误差
     GyroIntegError.x += AccGravity.x * KiDef;
     GyroIntegError.y += AccGravity.y * KiDef;
     GyroIntegError.z += AccGravity.z * KiDef*0.1;
     
     
     // ========== 积分限幅策略 ==========

    GyroIntegError.x = LIMIT(GyroIntegError.x, -MAX_INTEGRAL_ERROR, MAX_INTEGRAL_ERROR);
    GyroIntegError.y = LIMIT(GyroIntegError.y, -MAX_INTEGRAL_ERROR, MAX_INTEGRAL_ERROR);
    GyroIntegError.z = LIMIT(GyroIntegError.z, -MAX_INTEGRAL_ERROR, MAX_INTEGRAL_ERROR);
      
      //角速度融合加速度比例补偿值，与上面三句共同形成了PI补偿，得到矫正后的角速度值!!!转换的因子已经计算过了
     Gyro.x = p_icm->gyroX  + KpDef * AccGravity.x + GyroIntegError.x; //弧度制，此处补偿的是角速度的漂移
     Gyro.y = p_icm->gyroY  + KpDef * AccGravity.y + GyroIntegError.y;
     Gyro.z = p_icm->gyroZ  + (KpDef-0.6) * AccGravity.z + GyroIntegError.z;
      
          // 一阶龙格库塔法, 更新四元数
      //矫正后的角速度值积分，得到两次姿态解算中四元数一个实部Q0，三个虚部Q1~3的值的变化
     q0_t = (-q1 * Gyro.x - q2 * Gyro.y - q3 * Gyro.z) * HalfTime;
     q1_t = (q0  * Gyro.x - q3 * Gyro.y + q2 * Gyro.z) * HalfTime;
     q2_t = (q3  * Gyro.x + q0 * Gyro.y - q1 * Gyro.z) * HalfTime;
     q3_t = (-q2 * Gyro.x + q1 * Gyro.y + q0 * Gyro.z) * HalfTime;
      
      //积分后的值累加到上次的四元数中，即新的四元数
     q0+= q0_t;
     q1+= q1_t;
     q2+= q2_t;
     q3+= q3_t;
      
      // 重新四元数归一化，得到单位向量下
      
     NormQuat = q_rsqrt(squa(q0) + squa(q1) + squa(q2) + squa(q3)); //得到四元数的模长
     q0 *= NormQuat;                                                               //模长更新四元数值
     q1 *= NormQuat;
     q2 *= NormQuat;
     q3 *= NormQuat;
      
      
      
     /* 计算姿态角 */
     get_angle(p_angle);
      
    
  }

  //------------------------------------------------------//
  //函数简介:读取IMU660RB的原是数据并转化
  //返回参数：void
  //备注    ：读取加速度计值、角速度值，将陀螺仪值转为弧度//


  void imu_get(void)
  {
    imu660rb_get_acc();
    imu660rb_get_gyro();
        //加速度计转换：原始数据->g为单位 （1g=9.8m/s）
    IMU_TRAN.accX =(float)ACC_DATA_X;//ACC_GRAVITY;
    IMU_TRAN.accY =(float)ACC_DATA_Y;//ACC_GRAVITY;
    IMU_TRAN.accZ =(float)ACC_DATA_Z;//ACC_GRAVITY;
    
    // 在imu_get函数中对加速度计数据进行低通滤波
    static float alpha = 0.2f;  // 滤波系数
    static float accX_filtered = 0, accY_filtered = 0, accZ_filtered = 0;

    accX_filtered = alpha * accX_filtered + (1-alpha) * IMU_TRAN.accX;
    accY_filtered = alpha * accY_filtered + (1-alpha) * IMU_TRAN.accY;
    accZ_filtered = alpha * accZ_filtered + (1-alpha) * IMU_TRAN.accZ;

    IMU_TRAN.accX = accX_filtered;
    IMU_TRAN.accY = accY_filtered;
    IMU_TRAN.accZ = accZ_filtered;
    //陀螺仪原始数据转换->(。/s)->(弧度/秒)（简单的滤波10/10）
    
    static float gyro_filtered_x = 0, gyro_filtered_y = 0, gyro_filtered_z = 0;
    static int first_run = 1;
    static const float FILTER_ALPHA = 0.4f;
    
    
    IMU_TRAN.gyroX=((float)(GYRO_DATA_X)/GYRO_TRANSITION_FACTOR * 0.01745329f);//0.01745329为转弧度系数
    IMU_TRAN.gyroY=((float)(GYRO_DATA_Y)/GYRO_TRANSITION_FACTOR * 0.01745329f);
    IMU_TRAN.gyroZ=((float)(GYRO_DATA_Z)/GYRO_TRANSITION_FACTOR * 0.01745329f);
    

     if(first_run) {
        gyro_filtered_x = IMU_TRAN.gyroX;
        gyro_filtered_y = IMU_TRAN.gyroY;
        gyro_filtered_z = IMU_TRAN.gyroZ;
        first_run = 0;
        }
     // 一阶低通滤波: y[n] = α * y[n-1] + (1-α) * x[n]
    gyro_filtered_x = FILTER_ALPHA * gyro_filtered_x + (1.0f - FILTER_ALPHA) * IMU_TRAN.gyroX;
    gyro_filtered_y = FILTER_ALPHA * gyro_filtered_y + (1.0f - FILTER_ALPHA) * IMU_TRAN.gyroY;
    gyro_filtered_z = FILTER_ALPHA * gyro_filtered_z + (1.0f - FILTER_ALPHA) * IMU_TRAN.gyroZ;
    
    // 存储滤波后的陀螺仪数据
    IMU_TRAN.gyroX = gyro_filtered_x;
    IMU_TRAN.gyroY = gyro_filtered_y;
    IMU_TRAN.gyroZ = gyro_filtered_z;



    
    

     data_normalization(&IMU_TRAN, &g_attitude, DT); // 四元素算法
  }
















