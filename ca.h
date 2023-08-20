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

	/*
	 *	Generate gray_frame, binary_frame and edge_frame from input frame.
	 */
	void GenerateFrames(cv::Mat* input, cv::Mat* gray_frame, cv::Mat* binary_frame, cv::Mat* edge_frame);
	void GenerateFramesFromCapture(cv::Mat* raw_frame, cv::Mat* gray_frame, cv::Mat* binary_frame, cv::Mat* edge_frame);

	/*
	 *	Extract all possible markers frome input frame.
	 */
	std::vector<Marker> ExtractMarkersFromFrame(cv::Mat gray_frame, cv::Mat binary_frame);

	std::vector<int> ca::DetectedCubeId(std::vector<Marker> marker_list, int min_distance);

	/*
	 *	 Generate translation and rotation of cubes
	 */
	std::vector<Cube> GenerateCubes(std::vector<Marker> markers, int min_distance);

	void Fun();

	void Wait(int ms);

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
