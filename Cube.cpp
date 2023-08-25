#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;
}

Cube::Cube(int id, int size, std::vector<std::vector<int>>& markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}

	this->id = id;
	this->size = size;
}

Cube::~Cube() {
}

int Cube::GetId() {
	return id;
}

Marker Cube::Face(int index) {
	return markers[index];
}

void Cube::SetData(std::vector<std::vector<int>>& markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}

int Cube::CheckFaceOnCube(Marker& marker) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j == marker)
				return i;
		}
	}
	return -1;
}

int Cube::CheckFaceOnCube(Marker& marker, int min_distance) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j - marker <= min_distance)
				return i;
		}
	}
	return -1;
}

int Cube::CheckFaceOnCube(Marker& marker, int min_distance, int& rotation)
{
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j - marker <= min_distance){
				rotation = j;
				return i;
			}
		}
	}
	return -1;
}

std::vector<cv::Point3f> Cube::FacePoints(int face_id, float length, float margin) {
	std::vector<cv::Point3f> result;
	float l = length / 2;
	float h = l + margin;

	switch (face_id) {
	case 0:
		result.push_back(cv::Point3f(l, h, -l));
		result.push_back(cv::Point3f(l, h, l));
		result.push_back(cv::Point3f(-l, h, l));
		result.push_back(cv::Point3f(-l, h, -l));
		break;

	case 1:
		result.push_back(cv::Point3f(-l, -h, -l));
		result.push_back(cv::Point3f(-l, -h, l));
		result.push_back(cv::Point3f(l, -h, l));
		result.push_back(cv::Point3f(l, -h, -l));
		break;

	case 2:
		result.push_back(cv::Point3f(-h, l, -l));
		result.push_back(cv::Point3f(-h, l, l));
		result.push_back(cv::Point3f(-h, -l, l));
		result.push_back(cv::Point3f(-h, -l, -l)); 
		break;

	case 3:
		result.push_back(cv::Point3f(h, -l, -l));
		result.push_back(cv::Point3f(h, -l, l));
		result.push_back(cv::Point3f(h, l, l));
		result.push_back(cv::Point3f(h, l, -l));
		break;

	case 4:
		result.push_back(cv::Point3f(l, l, -h));
		result.push_back(cv::Point3f(-l, l, -h));
		result.push_back(cv::Point3f(-l, -l, -h));
		result.push_back(cv::Point3f(l, -l, -h));
		break;
		
	case 5:
		result.push_back(cv::Point3f(l, -l, h));
		result.push_back(cv::Point3f(-l, -l, h));
		result.push_back(cv::Point3f(-l, l, h));
		result.push_back(cv::Point3f(l, l, h));
		break;
	}

	return result;
}

std::vector<cv::Point3f> Cube::FacePoints(int face_id, int rotation, float length, float margin)
{
	std::vector<cv::Point3f> result;
	auto _result = FacePoints(face_id, length, margin);
	
	for (int i = 0; i < 4; i++) {
		result.push_back(_result[(i + rotation) % 4]);
	}

	return result;
}

void Cube::GenerateTransformAuto(Transform transform)
{
	if (buf.size() >= FILTER_WINDOW_SIZE) {
		buf.erase(buf.begin());
	}

	buf.push_back(transform);

	this->transform = Transform::Average(buf);
}


void Cube::SetFilterWindowSize(int size) {
	FILTER_WINDOW_SIZE = size;
}