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

	/*
	 * return the marker id on the face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 */
	int CheckFaceOnCube(Marker marker);

	/*
	 * return the marker id on the face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 * min_distance: The max count of different value.
	 */
	int CheckFaceOnCube(Marker marker, int min_distance);

	/*
	 * return the point array of vertices of a face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 */
	std::vector<cv::Point3f> FacePoints(int face_id, float length);

	cv::Vec3f translation;
	cv::Matx33f rotation;

protected:
	int id;
	int size;
	std::vector<Marker> markers;
};
