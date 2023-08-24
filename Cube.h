#pragma once

#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "Marker.h"
#include "Transform.h"

#define WINDOW_FILTER_SIZE 3

class Cube
{
public:
	Cube();
	Cube(int id, int size);
	Cube(int id, int size, std::vector<std::vector<int>>& markers);

	~Cube();

	int GetId();
	Marker Face(int index);

	void SetData(std::vector<std::vector<int>>& markers);

	/*
	 * return the marker id on the face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 */
	int CheckFaceOnCube(Marker& marker);

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
	int CheckFaceOnCube(Marker& marker, int min_distance);

	/*
	 * return the marker id on the face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 * min_distance: The max count of different value.
	 * rotation: the times of 90 degree that similar marker rotated counterclockwise.
	 */
	int CheckFaceOnCube(Marker& marker, int min_distance, int& rotation);

	/*
	 * return the point array of vertices of a face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 */
	std::vector<cv::Point3f> FacePoints(int face_id, float length, float margin);

	/*
	 * return the point array of vertices of a face.
	 * 0:	location: y = +1	direction: +x
	 * 1:	location: y = -1	direction: -x
	 * 2:	location: x = -1	direction: +y
	 * 3:	location: x = +1	direction: -y
	 * 4:	location: z = -1	direction: +y
	 * 5:	location: z = +1	direction: -y
	 * rotation: the times of 90 degree that similar marker rotated counterclockwise.
	 */
	std::vector<cv::Point3f> FacePoints(int face_id, int rotation, float length, float margin);

	Transform transform;
	std::vector<Transform> buf;
	void GenerateTransformAuto(Transform transform);

protected:
	int id;
	int size;
	std::vector<Marker> markers;
};
