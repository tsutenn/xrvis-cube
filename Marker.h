#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <limits.h>

#include <opencv2/opencv.hpp>

class Marker
{

public:
	Marker(int size);
	Marker(int size, std::vector<int> marker);
	Marker(int size, std::vector<std::vector<int>> marker);
	~Marker();

	bool operator==(const Marker& other);
	int operator-(const Marker& other);
	Marker operator+(const int& other);
	Marker operator+=(const int& other);
	
	void Copy(const Marker& other);

	int At(int x, int y);

	const char* ToString();

	cv::Mat image;
	std::vector<cv::Point2f> points;

protected:
	std::vector<std::vector<int>> marker;
	int size;

	friend std::ostream& operator<<(std::ostream& strm, const Marker& m);
};
