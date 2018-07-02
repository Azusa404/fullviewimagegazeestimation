#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
Mat LUT(Mat srcimg, Size size, int flag);

const int toushix = 150;//定义输出透视图像的尺寸
const int toushiy = 150;


//////主程序
int main()
{
	VideoCapture capture;
	capture.open("theta1.MP4");											//输入视频
	//capture.open(2);														//摄像头


	//Mat img = imread("111.jpg", 1);									//定义全天周原图
	//imshow("1", img);
	if (!capture.isOpened())											//判断视频读取是否成功
	{
		cout << "Read video failed" << endl;
		return 0;
	}

	Mat frame;	
	VideoWriter output_cap("toushi1.avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), 15, Size(toushix, toushiy), 1);// capture.get(CAP_PROP_FOURCC),capture.get(CAP_PROP_FPS)
	if (!output_cap.isOpened()) { return -1; }
	//cout << "total frame number is " << frame_num << endl;				//显示帧数

	//读取视频///////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	//int frame_num = capture.get(CAP_PROP_FRAME_COUNT);
	//for (int i = 0; i < frame_num; i++) 
	//{
	//	capture >> frame; 
	//	imshow("theta", frame);
	//	Mat perseptiveimg = LUT(frame, Size(toushix, toushiy), 1);				//操作每一帧
	//	imshow("toushi",perseptiveimg);								//播放视频
	//	output_cap << perseptiveimg;									//写入视频
	//	waitKey(30);													
	//}
	//////////////////////////////////////////////////////////////////////////

	//读取摄像头/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	while (1)
	{
		capture >> frame;
		imshow("theta", frame);
		Mat perseptiveimg = LUT(frame, Size(toushix, toushiy), 1);
		imshow("toushi", perseptiveimg);
		output_cap << perseptiveimg;
		if (waitKey(30) >= 0) break;
	}
	
	//Mat perseptiveimg = LUT(img, Size(150, 150), 1);					//操作单张透视图像
	//imshow("111", perseptiveimg);
	//imwrite("222.jpg", perseptiveimg);								
	waitKey(1);
	return 0;
}



////调用查找表
Mat LUT(Mat srcimg, Size size, int flag)//flag=0 gray, flag=1 color
{

	const int toushipixel = toushix * toushiy;
	int a[toushipixel] = { 0 }, b[toushipixel] = { 0 };								//创建两个一维数组

	ifstream lut1("coordinate.txt");									 //定义输入查找表
	string s;
	int i = 0, j = 0;													//初始化
	while (getline(lut1, s))											//按行读取 
	{

		stringstream strs(s);											//把行装到另一个流中 
		strs >> a[i] >> b[j];											//把流按分隔符实现分割输入 
		//cout << a[i] << "," << b[j] << endl;							//显示读取查找表过程
		i++; j++;
	}


	if (flag == 1) {
		Mat himg = srcimg.clone(); //原图
		Mat pimg;					//透视图像
		pimg = Mat_<Vec3b>(size, 255);	//定义透视图像
		int i = 0, j = 0;
		for (int x = 0; x < pimg.cols; x++) {
			for (int y = 0; y < pimg.rows; y++) {



				int pyy = b[i];
				int pxx = a[j];

			/*	if (pxx < 0) { pxx = himg.cols + pxx; }
				else if (pxx >= himg.cols) { pxx = pxx - himg.cols; }
				if (pyy < 0) { pyy = himg.rows + pyy; }
				else if (pyy >= himg.rows) { pyy = pyy - himg.rows; }*/
				pimg.at<Vec3b>(y, x) = himg.at<Vec3b>(pyy, pxx);	//在透视图像里填入圆柱图像的对应像素
				i++; j++;
			}
		}
		pimg = pimg.t();
		//////把透视图像旋转90度
		int width = pimg.cols;
		int length = pimg.rows;
		int angle = 90;	//旋转角度

		Mat rotation = getRotationMatrix2D(Point2f(length / 2, width / 2), angle, 1.0);	//算旋转
		warpAffine(pimg, pimg, rotation, Size(length, width));							//仿射变换
		return(pimg);
		
	}
	lut1.close();		//关闭TXT

	
}



