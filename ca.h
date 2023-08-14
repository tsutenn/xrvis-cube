#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "Marker.h"

class ca
{
public:
	ca(int camera_id);
	~ca();

	cv::Mat* getFrame();
	cv::Mat* getBinaryFrame();
	cv::Mat* getEdgeFrame();
	int getThreshG();

	void setThreshG(int threshG);
	void setCubeInfo(int markerSize, double markerLength, double markerMargin, int cubeCount);

	float perimeter(const std::vector<cv::Point>& a);

	void fun();
	bool LoopBlock();

	int getMarkerSize() {
		return markerSize;
	}

	std::vector<cv::Mat> outputImages;
	std::vector<Marker> outputMarkers;

protected:
	bool loopBlock = false;

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat grayFrame;
	cv::Mat binaryFrame;
	cv::Mat edgeFrame;

	cv::Size canonicalSize;

	int threshG = 120;

	int markerSize = 0;
	double markerMargin = 0;
	double markerLength = 0;
	int cubeCount = 0;

	std::vector<cv::Point2f> m_markerCorners2f;
};
