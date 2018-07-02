#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

double PI = CV_PI;
double error_x_0 = 5, error_y_0 = 0,error_x_1 = 0,error_y_1 = 0, error_x = 0, error_y= 0;

void onMouse(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		std::cout << "**********************" << std::endl;
		std::cout << "注視対象点をクリックしてください" << std::endl;
		std::cout << "**********************" << std::endl;

		int hx = x;
		int hy = y;
		std::cout << "(hx,hy)=(" << hx << "," << hy << ")" << std::endl;
		int MouseFlag = 1;
	}
}


cv::Mat PIMG(cv::Mat srcimg, cv::Point point, int Forcus, cv::Size size, int flag)//flag=0 gray, flag=1 coloar
{

	if (flag == 0) {
		cv::Mat himg = srcimg.clone();
		cv::Mat pimg;
		pimg = cv::Mat_<unsigned char>(size, 255);	//透視画像の定義(H,W,(初期画素値))
		double rot_fai = 360 * point.x / himg.cols*PI / 180;
		double rot_sita = 180 * point.y / himg.rows*PI / 180;

		cv::Mat rotY = (cv::Mat_<double>(3, 3) << cos(rot_sita), 0, sin(rot_sita), 0, 1, 0, -sin(rot_sita), 0, cos(rot_sita));
		cv::Mat rotZ = (cv::Mat_<double>(3, 3) << cos(rot_fai), -sin(rot_fai), 0, sin(rot_fai), cos(rot_fai), 0, 0, 0, 1);
		cv::Mat R = rotZ*rotY;

		double R1 = R.at<double>(0, 0);
		double R2 = R.at<double>(0, 1);
		double R3 = R.at<double>(0, 2);
		double R4 = R.at<double>(1, 0);
		double R5 = R.at<double>(1, 1);
		double R6 = R.at<double>(1, 2);
		double R7 = R.at<double>(2, 0);
		double R8 = R.at<double>(2, 1);
		double R9 = R.at<double>(2, 2);
		std::cout << "R=" << R << std::endl;

		for (int x = 0; x < pimg.cols; x++) {		//透視画像座標をまわす　透視画像座標→対応する元画像座標を探す→元画像画素値を透視画像座標へマッピング
			for (int y = 0; y < pimg.rows; y++) {
				cv::Point3d p = cv::Point3d(x - pimg.cols / 2, y - pimg.rows / 2, Forcus);		//透視画像座標の３次元座標値
				cv::Mat pmat = (cv::Mat_<double>(3, 1) << p.x, p.y, p.z);
				cv::Mat pp = R*pmat;
				p.x = pp.at<double>(0, 0);
				p.y = pp.at<double>(1, 0);
				p.z = pp.at<double>(2, 0);

				double scale_p = sqrt(p.x*p.x + p.y*p.y + p.z*p.z*1.0);
				cv::Point3d p_vec = cv::Point3d(p.x / scale_p, p.y / scale_p, p.z / scale_p);		//３次元座標の原点からの単位ベクトルを求める

				double fai, sita;

				if (p.x < 0 && p.y <= 0) {												//透視画像点への原点からのベクトルp_vecより、球座標系におけるθとφを求める　直交座標系→球座標系
					fai = 180 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if (p.x >= 0 && p.y <= 0) {
					fai = 360 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if (p.x >= 0 && p.y >= 0) {
					fai = atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if ((p.x <= 0 && p.y >= 0)) {
					fai = 180 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}

				if (p.x == 0 && p.y == 0) {												//Z軸上の点に対するエラー処理
					fai = 0;
					sita = 0;
				}


				if (sita < 0) { sita = sita + 180; }


				double px = himg.cols*fai / 360;										//φ,θを用いて、全天周画像(theta出力、正距円筒図法)の対応点を求める
				double py = himg.rows*sita / 180;



				if (px<0 || py<0 || px>himg.cols || py>himg.rows) {				//エラー回避
					std::cout << "(θ,Φ)=(" << sita << "," << fai << ")" << std::endl;

					//std::cout << "error" << std::endl;
					continue;
				}

				pimg.at<unsigned char>(y, x) = himg.at<unsigned char>(int(py + 0.5), int(px + 0.5));	//透視画像座標に対応する入力画像座標の画素値を入れる

			}
		}
		return(pimg);
	}


	if (flag == 1) {
		cv::Mat himg = srcimg.clone();
		cv::Mat pimg;
		pimg = cv::Mat_<cv::Vec3b>(size, 255);	//透視画像の定義(H,W,(初期画素値))
		double rot_fai = 360 * point.x / himg.cols*PI / 180;
		double rot_sita = 180 * point.y / himg.rows*PI / 180;

		cv::Mat rotY = (cv::Mat_<double>(3, 3) << cos(rot_sita), 0, sin(rot_sita), 0, 1, 0, -sin(rot_sita), 0, cos(rot_sita));
		cv::Mat rotZ = (cv::Mat_<double>(3, 3) << cos(rot_fai), -sin(rot_fai), 0, sin(rot_fai), cos(rot_fai), 0, 0, 0, 1);
		cv::Mat R = rotZ*rotY;

		double R1 = R.at<double>(0, 0);
		double R2 = R.at<double>(0, 1);
		double R3 = R.at<double>(0, 2);
		double R4 = R.at<double>(1, 0);
		double R5 = R.at<double>(1, 1);
		double R6 = R.at<double>(1, 2);
		double R7 = R.at<double>(2, 0);
		double R8 = R.at<double>(2, 1);
		double R9 = R.at<double>(2, 2);
		std::cout << "R=" << R << std::endl;

		for (int x = 0; x < pimg.cols; x++) {		//透視画像座標をまわす　透視画像座標→対応する元画像座標を探す→元画像画素値を透視画像座標へマッピング
			for (int y = 0; y < pimg.rows; y++) {
				cv::Point3d p = cv::Point3d(x - pimg.cols / 2, y - pimg.rows / 2, Forcus);		//透視画像座標の３次元座標値
				cv::Mat pmat = (cv::Mat_<double>(3, 1) << p.x, p.y, p.z);
				cv::Mat pp = R*pmat;
				p.x = pp.at<double>(0, 0);
				p.y = pp.at<double>(1, 0);
				p.z = pp.at<double>(2, 0);

				double scale_p = sqrt(p.x*p.x + p.y*p.y + p.z*p.z*1.0);
				cv::Point3d p_vec = cv::Point3d(p.x / scale_p, p.y / scale_p, p.z / scale_p);		//３次元座標の原点からの単位ベクトルを求める

				double fai, sita;

				if (p.x < 0 && p.y <= 0) {												//透視画像点への原点からのベクトルp_vecより、球座標系におけるθとφを求める　直交座標系→球座標系
					fai = 180 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if (p.x >= 0 && p.y <= 0) {
					fai = 360 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if (p.x >= 0 && p.y >= 0) {
					fai = atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}
				else if ((p.x <= 0 && p.y >= 0)) {
					fai = 180 + atan(p_vec.y / p_vec.x)*(180 / PI);
					sita = atan(sqrt(p_vec.x*p_vec.x + p_vec.y*p_vec.y) / p_vec.z)*(180 / PI);
				}

				if (p.x == 0 && p.y == 0) {												//Z軸上の点に対するエラー処理
					fai = 0;
					sita = 0;
				}


				if (sita < 0) { sita = sita + 180; }
				if (sita > 180) { sita = sita - 180; }


				double px = himg.cols*fai / 360;										//φ,θを用いて、全天周画像(theta出力、正距円筒図法)の対応点を求める
				double py = himg.rows*sita / 180;



				if (px<0 || py<0 || px>himg.cols || py>himg.rows) {				//エラー回避
					std::cout << "(θ,Φ)=(" << sita << "," << fai << ")" << std::endl;

					//std::cout << "error" << std::endl;
					continue;
				}



				double pyy = py + 0.5;
				double pxx = px + 0.5;

				if (pxx < 0) { pxx = himg.cols + pxx; }
				else if (pxx >= himg.cols) { pxx = pxx - himg.cols; }
				if (pyy < 0) { pyy = himg.rows + pyy; }
				else if (pyy >= himg.rows) { pyy = pyy - himg.rows; }
				//std::cout << pxx << "," << pyy << std::endl;
				pimg.at<cv::Vec3b>(y, x) = himg.at<cv::Vec3b>(pyy, pxx);	//透視画像座標に対応する入力画像座標の画素値を入れる

			}
		}
		pimg = pimg.t();
		return(pimg);
	}
}


int main()
{	//
	char write_str[256];
	char write_str2[256];
	//
	VideoCapture capture;
	capture.open("C:\\Users\\zhaox\\Desktop\\2018.1.5\\1_lut\\EJYR3359.MP4");
	const int txt_lenth = 3000;
	double a[txt_lenth] = { 0 }, b[txt_lenth] = { 0 }, c[txt_lenth] = { 0 };
	double g0theta[txt_lenth] = { 0 }, g0fai[txt_lenth] = { 0 }, g1theta[txt_lenth] = { 0 }, g1fai[txt_lenth] = { 0 }, g5theta[txt_lenth] = { 0 }, g5fai[txt_lenth] = { 0 };
	double gx, gy, gz;
	ifstream g0_txt;
	g0_txt.open("gaze_0_thetafai.txt");
	if (!g0_txt.is_open())
	{
		cout << "Error opening gmat file"; exit(-1);
	}
	string s0;
	int i=0;
	while (getline(g0_txt, s0))											//按行读取 
	{

		stringstream strs(s0);											//把行装到另一个流中 
		strs >> g0fai[i] >> g0theta[i];											//把流按分隔符实现分割输入 
																		//cout << a[i] << "," << b[j] << endl;							//显示读取查找表过程
		i++; 
	}
	//////
	ifstream g1_txt;
	g1_txt.open("gaze_1_thetafai.txt");
	if (!g1_txt.is_open())
	{
		cout << "Error opening gmat file"; exit(-2);
	}
	string s1;
	int j = 0;
	while (getline(g1_txt, s1))											//按行读取 
	{

		stringstream strs(s1);											//把行装到另一个流中 
		strs >> g1fai[j] >> g1theta[j];											//把流按分隔符实现分割输入 
																				//cout << a[i] << "," << b[j] << endl;							//显示读取查找表过程
		j++;
	}
	//////
	//while (getline(g_txt, g1))
	//{
	//	stringstream strs(g1);
	//	strs >> a[current_frame] >> b[current_frame] >> c[current_frame];
	//	current_frame++;

	//}

	int current_frame = 0;
	double fai_g, theta_g, fai_atan2, theta_atan2;//quantianzhoutuxiang zhushidian

	Mat frame;
	while (current_frame < 3000)
	{
		sprintf_s(write_str, "result_(%d).jpg", current_frame);
		sprintf_s(write_str2, "result_laseimg_(%d).jpg", current_frame);
		capture >> frame;
		//VideoWriter output_cap("toushi1.avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), 29, Size(3840, 1920), 1);// capture.get(CAP_PROP_FOURCC),capture.get(CAP_PROP_FPS)
		//if (!output_cap.isOpened()) { return -1; }
		//imshow("1",frame);
		


		//gx = a[current_frame];
		//gy = b[current_frame];
		//gz = c[current_frame];

		//current_frame++;

		//if (gx < 0 && gy <= 0) {												//透視画像点への原点からのベクトルp_vecより、球座標系におけるθとφを求める　直交座標系→球座標系
		//	fai_g = 180 + atan(gy / gx)*(180 / PI);
		//	theta_g = atan(sqrt(gx*gx + gy*gy) / gz)*(180 / PI);
		//}
		//else if (gx >= 0 && gy <= 0) {
		//	fai_g = 360 + atan(gy / gx)*(180 / PI);
		//	theta_g = atan(sqrt(gx*gx + gy*gy) / gz)*(180 / PI);
		//}
		//else if (gx >= 0 && gy >= 0) {
		//	fai_g = atan(gy / gx)*(180 / PI);
		//	theta_g = atan(sqrt(gx*gx + gy*gy) / gz)*(180 / PI);
		//}
		//else if ((gx <= 0 && gy >= 0)) {
		//	fai_g = 180 + atan(gy / gx)*(180 / PI);
		//	theta_g = atan(sqrt(gx*gx + gy*gy) / gz)*(180 / PI);
		//}

		//if (gx == 0 && gy == 0) {												//Z軸上の点に対するエラー処理
		//	fai_g = 0;
		//	theta_g = 0;
		//}
		//

		//if (theta_g < 0) { theta_g = theta_g + 180; }

		//

		//fai_atan2 = atan2(gy, gx)*(180 / PI);
		//theta_atan2 = atan2(sqrt(gx*gx + gy*gy) , gz)*(180 / PI);

		//cout << "atan:" << fai_g << theta_g << endl;
		//cout << "atan2" << fai_atan2 << theta_atan2 << endl;

		Mat OutPut = frame.clone();
		////个人误差修正
		g0fai[current_frame] = g0fai[current_frame] + error_x_0;
		g0theta[current_frame] = g0theta[current_frame] + error_y_0;
		g1fai[current_frame] = g1fai[current_frame] + error_x_1;
		g1theta[current_frame] = g1theta[current_frame] + error_y_1;
		g5fai[current_frame] = (g0fai[current_frame] + g1fai[current_frame]) / 2 + error_x;
		g5theta[current_frame] = (g0theta[current_frame] + g1theta[current_frame]) / 2 + error_y;
		////画注视点，0左眼，1右眼，5两眼正中
		int gxp0 = OutPut.cols * g0fai[current_frame] / 360;
		int gyp0 = OutPut.rows * g0theta[current_frame] / 180;
		int gxp1 = OutPut.cols * g1fai[current_frame] / 360;
		int gyp1 = OutPut.rows * g1theta[current_frame] / 180;
		//int gxp5 = OutPut.cols * g5fai[current_frame] / 360;
		//int gyp5 = OutPut.rows * g5theta[current_frame] / 180;
		//cout << "注視点=" << gxp0 << "," << gyp0 << endl;
		//cout << gxp0 << gyp0 << OutPut.cols << OutPut.rows << g0fai[current_frame] << g0theta[current_frame]<<endl;
		circle(OutPut, cv::Point(gxp0, gyp0), 20, cv::Scalar(255, 0, 0), -1);
		//circle(OutPut, cv::Point(gxp1, gyp1), 20, cv::Scalar(155, 0, 0), -1);
		//circle(OutPut, cv::Point(gxp5, gyp5), 20, cv::Scalar(0, 255, 0), -1);
		namedWindow("debug0", CV_WINDOW_NORMAL);
		imshow("debug0", OutPut);
		//output_cap << OutPut;									//写入视频
		waitKey(1);

		cv::Mat lastimg = PIMG(frame, cv::Point(gxp0, gyp0), 1000, cv::Size(500, 500), 1);
		cv::circle(lastimg, cv::Point(lastimg.cols / 2, lastimg.rows / 2), 3, cv::Scalar(0, 255, 255), -1);
		cv::circle(lastimg, cv::Point(lastimg.cols / 2, lastimg.rows / 2), 87, cv::Scalar(255, 0, 0), 1);
		cv::circle(lastimg, cv::Point(lastimg.cols / 2, lastimg.rows / 2), 52, cv::Scalar(255, 0, 0), 1);
		cv::namedWindow("debug4", CV_WINDOW_NORMAL);
		cv::imshow("debug4", lastimg);
		cv::imwrite(write_str2, lastimg);

		cv::waitKey(1);

		current_frame++;
	}

		return 0;
}