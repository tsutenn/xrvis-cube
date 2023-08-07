#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

class ca
{

public:

	ca(int camera_id);
	~ca();

	cv::Mat* getFrame();
	cv::Mat* getAdptThr();
	int getThreshG();

	void setThreshG(int threshG);

	void fun();
	void fun(int delay);

private:

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat adptThr;

	int threshG = 120;
};

