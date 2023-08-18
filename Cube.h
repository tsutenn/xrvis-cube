#pragma once

#include <iostream>
#include <algorithm>
#include<opencv2/opencv.hpp>

#include "Marker.h"

class Cube
{
public:
	Cube(int id, int size);
	Cube(int id, int size, std::vector<std::vector<int>> markers);

	~Cube();

	int GetId();
	Marker Face(int index);

	void SetData(std::vector<std::vector<int>> markers);

	int CheckFaceOnCube(Marker marker);
	int CheckFaceOnCube(Marker marker, int min_distance);
	std::vector<cv::Point3f> FacePoints(int face_id, float length);

protected:
	int id;
	int size;
	std::vector<Marker> markers;
};
