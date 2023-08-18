#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "Marker.h"
#include "Cube.h"

class ca
{
public:
	ca(int camera_id);
	~ca();

	cv::Mat* GetFrame();
	cv::Mat* GetBinaryFrame();
	cv::Mat* GetEdgeFrame();
	int GetThreshG();
	bool LoopBlock();
	int GetMarkerSize();

	void SetThreshG(int threshG);
	void SetCubeInfo(int markerSize, double markerLength, double markerMargin, int cubeCount);
	void SetCubeList(std::vector<Cube> cubes);

	void Fun();

	// std::vector<cv::Mat> outputImages;
	std::vector<Marker> outputMarkers;

protected:
	bool loopBlock = false;

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat grayFrame;
	cv::Mat binaryFrame;
	cv::Mat edgeFrame;

	int threshG = 120;

	int markerSize = 0;
	double markerMargin = 0;
	double markerLength = 0;
	int cubeCount = 0;

	cv::Size canonicalSize;
	cv::Mat camMatrix;
	cv::Mat distCoeff;

	std::vector<Cube> cubeList;

	std::vector<cv::Point2f> m_markerCorners2f;

	float Perimeter(const std::vector<cv::Point>& a);
};
