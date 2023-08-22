#pragma once

#include <iostream>
#include <algorithm>
#include<opencv2/opencv.hpp>

#include "Marker.h"

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

	void SetTranslationVector(cv::Vec3f& translation);
	void SetRotationMatrix(cv::Matx33f& rotationMatrix);

	void GenerateTranslation(Cube base_cube);

	cv::Vec3f GetTranslationVector();
	cv::Matx33f GetRotationMatrix();

	cv::Vec3f GetTranslation();
	cv::Vec3f GetRotation();

	const char* GetTransformInString();
	const char* GetRotationInString();

	// Checks if a matrix is a valid rotation matrix.
	static bool isRotationMatrix(cv::Mat& R);

	// Calculates rotation matrix to euler angles
	// The result is the same as MATLAB except the order
	// of the euler angles ( x and z are swapped ).
	static cv::Vec3f rotationMatrixToEulerAngles(cv::Mat& R);

protected:
	int id;
	int size;
	std::vector<Marker> markers;

	cv::Vec3f translationVector;
	cv::Matx33f rotationMatrix;

	cv::Vec3f translation;
	cv::Vec3f rotation;
};
