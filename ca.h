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
	cv::Mat* getEdges();
	int getThreshG();
	int getDetectedCount();

	void setThreshG(int threshG);
	void setCubeInfo(int markerSize, double markerLength, double markerMargin, int cubeCount);

	void fun();

	bool LoopBlock();

protected:
	bool loopBlock = false;

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat adptThr;
	cv::Mat edges;

	int threshG = 120;

	int markerSize = 0;
	double markerMargin = 0;
	double markerLength = 0;
	int cubeCount = 0;

	int detected_count = 0;
};
