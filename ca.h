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
	void setCubeInfo(int cubeCount, double cubeSize, double markerMargin);

	void fun();
	void fun(int delay);

	bool LoopBlock();

protected:
	bool loopBlock = false;

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat adptThr;

	int threshG = 120;

	int cubeCount = 0;
	int cubeSize = 0;
	int markerMargin = 0;
};
