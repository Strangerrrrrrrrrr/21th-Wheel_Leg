/*
*
*       Create on :2026/3/31
*       author: ii
*
*/

#include "segmentation.h"

//裁剪函数
int clip (int x, int lower, int upper){
    return x > upper ? upper : x < lower ? lower :x;
}
/*int fclip(float x, float lower, float upper)
{
}
*/
uint8_t img_line_data[MT9V03X_W][MT9V03X_H]
image_t capture_image = DEF_IMAGE(NULL,MT9V03X_W, MT9V03X_H); //定义一个图像结构体变量，指向摄像头图像数据
image_t draw_image = DEF_IMAGE((uint8_t *)img_line_data, MT9V03X_W, MT9V03X_H); //定义一个图像结构体变量，指向显示屏图像数据
/*
uint8_t Image_Use[LCDH][LCDW];
float Mh = MT9V03X_H;
float Lh = LCDH;
float Mw = MT9V03X_W;
float Lw = LCDW;
//float div_div  = 0;
void compressimage(void)
{
    //float div_div  = Mw /Lw ;  
    for (int i = 0; i < LCDH; i++)
    {
        for (int j = 0; j < LCDW; j++)
        {
            int col = (int)(j * Mw / Lw + 0.5);
            int row = (int)(i * Mh / Lh + 0.5);
            Image_Use[i][j] = AT_IMAGE(&capture_image, col, row);
        }
    }
    mt9v03x_finish_flag = 0 ;  //完成一帧图像DMA传输，标志位清零
}

为什么要进行图片压缩？？？？？？？？？*/
//到底是压缩显示分辨率还是图片分辨率 
//等倍率放缩还是一定比率采样？
//肯定计算量小一些 但是有必要吗？？



//拷贝复制图片
void clone_image(image_t *img0, image_t *img1) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    if (img0->width == img0->step && img1->width == img1->step) {
        memcpy(img1->data, img0->data, img0->width * img0->height);
    } else {
        for (int y = 0; y < img0->height; y++) {
            memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
        }
    }
}

void clear_image(image_t *img) {
    assert(img && img->data);
    if (img->width == img->step) {
        memset(img->data, 0, img->width * img->height);
    } else {
        for (int y = 0; y < img->height; y++) {
            memset(&AT(img, 0, y), 0, img->width);
        }
    }
}


/*定义前进方向*/
const int front[4][2] = {
    {0, -1}, //上 (x+0, y-1)
    {1, 0},  //右 (x+1, y+0)
    {0, 1},  //下 (x+0, y+1)
    {-1, 0}  //左 (x-1, y+0)
};
const int left_turn[4][2] = {
    {-1, -1}, //左-上 (x-1, y-1)
    {1, -1},  //右-上 (x+1, y-1)
    {1, 1},  //右-下 (x+1, y+1)
    {1, -1}   //左-下 (x-1, y+1)
};
const int right_turn[4][2] = {
    {1, -1}, //右-上 (x+1, y-1)
    {1, 1},  //右-下 (x+1, y+1)
    {-1, 1}, //左-下 (x-1, y+1)
    {-1, -1} //左-上 (x-1, y-1)
};

/*********左手迷宫巡线*************/
void Lmaze_Adapt(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) 
    {
    //int pts[][2],     输出：存储巡线走过的每个点坐标
    //int *num          输入：pts 最大容量；输出：实际找到的点数

    assert(img && img->data);
    assert(num && *num >= 0);
    assert(block_size > 1 && block_size % 2 == 1);    //窗口大小必须为大于1的奇数
    int half  = block_size /2;                        //用于遍历领域
    int step = 0 ,dir = 0, turn = 0 ;                 //step为当前走过的点数，dir为当前前进方向，turn为连续右转次数
    while(step < *num 
        && half < x  && x<img ->width - half-1
        && half < y  && y<img ->height - half-1
        && turn <4)   //连续右转4次说明陷入死路
        {
        int local_thres  = 0 ;
        for (int dy = - half ; dy <= half ; dy++) {
            for (int dx = - half ; dx <= half ; dx++) {
                local_thres += IMAGE_Pixel(img, x + dx, y + dy);
            }
        }
        local_thres = local_thres / (block_size * block_size) - clip_value;  //计算局部平均灰度值并减去clip_value得到自适应阈值

        int  current_value = IMAGE_PIXEL(img, x, y);  //当前像素值
        int  front_value = IMAGE_PIXEL(img, x + front[dir][0], y + front[dir][1]);  //前方像素值
        int  frontleft_value = IMAGE_PIXEL(img, x + left_turn[dir][0], y + left_turn[dir][1]);  //前左像素值
        if(front_value < local_thres){                  //前方是黑色则右转
            dir  = (dir + 1) % 4;
            turn++;
        }
        else if(frontleft_value < local_thres){        //左前方是黑色则继续前进
            x += front[dir][0];
            y += front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
        else {                                         //前方和左前方都是白色则左转
            x += left_turn[dir][0];
            y += left_turn[dir][1];
            dir = (dir + 3) % 4 ;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0 ; 
        }
    *num = step;  //更新实际找到的点数
    }
    }

/*********右手迷宫巡线*************/
void Rmaze_Adapt(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {
    assert(img && img->data);
    assert(num && *num >= 0);
    assert(block_size > 1 && block_size % 2 == 1 );

    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while(step < *num
        && 0 < x  && x<img ->width -1
        && 0 < y  && y<img ->height -1
        && turn <4)   //连续右转4次说明陷入死路
    {
        int local_thres = 0;
        for (int dy = -half; dy <= half;dy++){
            for(int dx = -half; dx<= half;dx++){
                local_thres +=  IMAGE_PIXEL(img, x + dx, y + dy);
            }
        }
        local_thres = local_thres / (block_size * block_size) - clip_value;   //计算局部平均灰度值并减去clip_value得到自适应阈值

        int current_value = IMAGE_PIXEL(img, x, y);  //当前像素值
        int front_value = IMAGE_PIXEL(img, x + front[dir][0], y + front[dir][1]);  //前方像素值
        int frontright_value = IMAGE_PIXEL(img, x + right_turn[dir][0], y + right_turn[dir][1]);  //前右像素值
        if(front_value < local_thres){                  //前方是黑色则左转
            dir  = (dir + 3) % 4;
            turn++;
        }
        else if(frontright_value < local_thres){        //右前方是黑色则继续前进
            x += front[dir][0];
            y += front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
        else{
            x += right_turn[dir][0];
            y += right_turn[dir][1];
            dir = (dir + 1) % 4 ;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0 ;

        }
    }
    *num  = step;  //更新实际找到的点数
}



// /*********形态学处理 结构元************/
// // 图像逻辑与
// void image_and(image_t *img0, image_t *img1, image_t *img2) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img2 && img2->data);
//     assert(img0->width == img1->width && img0->height == img1->height);
//     assert(img0->width == img2->width && img0->height == img2->height);

//     // 先遍历y后遍历x比较cache-friendly
//     for (int y = 0; y < img0->height; y++) {
//         for (int x = 0; x < img0->width; x++) {
//                     AT(img2, x, y) = (AT(img0, x, y) == 0 || AT(img1, x, y) == 0) ? 0 : 255;
//         }
//     }
// }

// // 图像逻辑或
// void image_or(image_t *img0, image_t *img1, image_t *img2) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img2 && img2->data);
//     assert(img0->width == img1->width && img0->height == img1->height);
//     assert(img0->width == img2->width && img0->height == img2->height);

//     // 先遍历y后遍历x比较cache-friendly
//     for (int y = 0; y < img0->height; y++) {
//         for (int x = 0; x < img0->width; x++) {
//                     AT(img2, x, y) = (AT(img0, x, y) == 0 && AT(img1, x, y) == 0) ? 0 : 255;
//         }
//     }
// }

// // 2x2最小池化(赛道边界是黑色，最小池化可以较好保留赛道边界)
// void minpool2(image_t *img0, image_t *img1) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img0->width / 2 == img1->width && img0->height / 2 == img1->height);
//     assert(img0 != img1 && img0->data != img1->data);

//     uint8_t min_value;
//     // 先遍历y后遍历x比较cache-friendly
//     for (int y = 1; y < img0->height; y += 2) {
//         for (int x = 1; x < img0->width; x += 2) {
//             min_value = 255;
//             if (AT(img0, x, y) < min_value) min_value = AT(img0, x, y);
//             if (AT(img0, x - 1, y) < min_value) min_value = AT(img0, x - 1, y);
//             if (AT(img0, x, y - 1) < min_value) min_value = AT(img0, x, y - 1);
//             if (AT(img0, x - 1, y - 1) < min_value) min_value = AT(img0, x - 1, y - 1);
//                     AT(img1, x / 2, y / 2) = min_value;
//         }
//     }
// }


// // 3x3腐蚀
// void erode3(image_t *img0, image_t *img1) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img0->width == img1->width && img0->height == img1->height);
//     assert(img0 != img1 && img0->data != img1->data);

//     int min_value;
//     // 先遍历y后遍历x比较cache-friendly    
//     for (int y = 1; y < img0->height - 1; y++) {
//         for (int x = 1; x < img0->width - 1; x++) {
//             min_value = 255;
//             for (int dy = -1; dy <= 1; dy++) {
//                 for (int dx = -1; dx <= 1; dx++) {
//                     if (AT(img0, x + dx, y + dy) < min_value) min_value = AT(img0, x + dx, y + dy);
//                 }
//             }
//                     AT(img1, x, y) = min_value;
//         }
//     }
// }

// // 3x3膨胀
// void dilate3(image_t *img0, image_t *img1) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img0->width == img1->width && img0->height == img1->height);
//     assert(img0 != img1 && img0->data != img1->data);

//     int max_value;
//     // 先遍历y后遍历x比较cache-friendly    
//     for (int y = 1; y < img0->height - 1; y++) {
//         for (int x = 1; x < img0->width - 1; x++) {
//             max_value = 0;
//             for (int dy = -1; dy <= 1; dy++) {
//                 for (int dx = -1; dx <= 1; dx++) {
//                     if (AT(img0, x + dx, y + dy) > max_value) max_value = AT(img0, x + dx, y + dy);
//                 }
//             }
//                     AT(img1, x, y) = max_value;
//         }
//     }
// }


// // 图像滤波降噪
// void blur(image_t *img0, image_t *img1, uint32_t kernel) {
//     assert(img0 && img0->data);
//     assert(img1 && img1->data);
//     assert(img0->width == img1->width && img0->height == img1->height);
//     assert(img0 != img1 && img0->data != img1->data);

//     // 先遍历y后遍历x比较cache-friendly
//     for (int y = 1; y < img0->height - 1; y++) {
//         for (int x = 1; x < img0->width - 1; x++) {
//                     AT(img1, x, y) = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
//                                       2 * AT(img0, x - 1, y) + 4 * AT(img0, x, y) + 2 * AT(img0, x + 1, y) +
//                                       1 * AT(img0, x - 1, y + 1) + 2 * AT(img0, x, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 16;
//         }
//     }
// }

//点集合  三角滤波
void blur_points()
{

}