

#include <iostream>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;


int main()
{
    // 读取图片
    Mat img = imread("image.png");
    if (img.empty()) {
        printf("无法读取图片！\n");
        return -1;
    }
    // 转换为 HSV 颜色空间
	Mat img_hsv;
	cvtColor(img, img_hsv, COLOR_BGR2HSV);
    
    // 分离 HSV 三个通道
	Mat img_h, img_s, img_v;
	vector<Mat> hsv_channels;
	split(img_hsv, hsv_channels);
	img_h = hsv_channels[0];
	img_s = hsv_channels[1];
	img_v = hsv_channels[2];

	// 阈值化处理， 提取提取高亮度
	Mat mask_h, mask_s, mask_v;
	inRange(img_h, 0, 255, mask_h); // H 通道不限制
	inRange(img_s, 0, 255, mask_s); // S 通道不限制
	inRange(img_v, 245, 255, mask_v); // V 通道高亮度

	// 合并掩码
	Mat mask;
	bitwise_and(mask_h, mask_s, mask);
	bitwise_and(mask, mask_v, mask);

	// 应用掩码提取高亮区域
	Mat img_out;
	bitwise_and(img, img, img_out, mask);

	// 提取灰度图像
	Mat gray;
	cvtColor(img_out, gray, COLOR_BGR2GRAY);

	// 去除噪点， 使轮廓更加完整
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat thresh;
	erode(gray, thresh, kernel);
	dilate(thresh, thresh, kernel);

	// 查找轮廓
	vector<vector<Point>> contours;
	findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// 遍历轮廓， 查找六边形
	for (int i = 0; i < contours.size(); i++) {
		// 过滤掉面积过小的轮廓
		if (contourArea(contours[i]) < 100) {
			continue;
		}
		// 多边形拟合
		vector<Point> approx;
		approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.02, true);
		// 判断是否为六边形
		if (approx.size() == 6) {
			// 计算六边形的外接矩形
			Rect rect = boundingRect(approx);
			// 绘制矩形
			rectangle(img, rect, Scalar(0, 255, 0), 2);
		}

	}
	// 显示结果
	imshow("Detected Hexagons", img);
    waitKey(0); // 等待按键
	return 0;

}

