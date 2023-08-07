#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

class Marker
{

public:

	int** marker;
	int size;

	Marker(int size);
	Marker(int size, int** marker);
	Marker(int size, int* marker);
	Marker(cv::Mat mat);
	~Marker();

	bool operator==(const Marker& other);
	Marker operator+(const int& other);
	Marker operator+=(const int& other);
	Marker ToMarker(const cv::Mat& mat);
	void Copy(const Marker& other);

private:

	friend std::ostream& operator<<(std::ostream& strm, const Marker& m);
};
