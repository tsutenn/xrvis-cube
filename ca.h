#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "Marker.h"
#include "Cube.h"

class ca
{
public:
	ca(int camera_id);
	ca(int camera_id, std::vector<double> cam_matrix, std::vector<double> dist_coeff);
	~ca();

	cv::Mat* GetFrame();
	cv::Mat* GetBinaryFrame();
	int GetThreshG();
	bool LoopBlock();
	int GetMarkerSize();

	Cube& GetBaseCube();
	std::vector<Cube>& GetCubeList();

	void SetThreshG(int threshG);
	void SetCubeInfo(int markerSize, double markerLength, double markerMargin);
	void SetCubeList(std::vector<Cube> cube_list, Cube base_cube);

	/*
	 *	Generate gray_frame, binary_frame and edge_frame from input frame.
	 */
	void GenerateFrames(cv::Mat& input, cv::Mat& gray_frame, cv::Mat& binary_frame);
	void GenerateFramesFromCapture(cv::Mat& raw_frame, cv::Mat& gray_frame, cv::Mat& binary_frame);

	/*
	 *	Extract all possible markers frome input frame.
	 */
	std::vector<Marker> ExtractMarkersFromFrame(cv::Mat& gray_frame, cv::Mat& binary_frame);

	std::vector<Marker> GenerateBaseCube(Cube& base_cube, std::vector<Marker>& marker_list, int min_distance);

	/*
	 *	 Generate translation and rotation of cubes
	 */
	std::vector<Cube> GenerateCubes(std::vector<Cube>& cube_list, Cube& base_cube, std::vector<Marker>& markers, int min_distance);

	void Fun();

	void Wait(int ms);

	std::vector<Marker> outputMarkers;

protected:
	bool loopBlock = false;

	cv::VideoCapture capture;
	cv::Mat frame;
	cv::Mat grayFrame;
	cv::Mat binaryFrame;

	int threshG = 120;

	int markerSize = 0;
	double markerMargin = 0;
	double markerLength = 0;

	cv::Size canonicalSize;
	cv::Mat camMatrix;
	cv::Mat distCoeff;

	Cube baseCube;

	std::vector<Cube> cubeList;

	std::vector<cv::Point2f> m_markerCorners2f;

	float Perimeter(const std::vector<cv::Point>& a);
};
