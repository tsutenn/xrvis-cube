#include "Cube.h"

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;
}

Cube::Cube(int id, int size, std::vector<std::vector<int>> markers) {
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

void Cube::SetData(std::vector<std::vector<int>> markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}

int Cube::CheckFaceOnCube(Marker marker) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j == marker)
				return i;
		}
	}
	return -1;
}

int Cube::CheckFaceOnCube(Marker marker, int distance) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j - marker <= distance)
				return i;
		}
	}
	return -1;
}

std::vector<cv::Point3f> Cube::FacePoints(int face_id, float length) {
	std::vector<cv::Point3f> result;
	float l = length / 2;

	switch (face_id) {
	case 0:
		result.push_back(cv::Point3f(l, l, -l));
		result.push_back(cv::Point3f(l, l, l));
		result.push_back(cv::Point3f(-l, l, l));
		result.push_back(cv::Point3f(-l, l, -l));
		break;

	case 1:
		result.push_back(cv::Point3f(-l, -l, -l));
		result.push_back(cv::Point3f(-l, -l, l));
		result.push_back(cv::Point3f(l, -l, l));
		result.push_back(cv::Point3f(l, -l, -l));
		break;

	case 2:
		result.push_back(cv::Point3f(-l, l, -l));
		result.push_back(cv::Point3f(-l, l, l));
		result.push_back(cv::Point3f(-l, -l, l));
		result.push_back(cv::Point3f(-l, -l, -l));
		break;

	case 3:
		result.push_back(cv::Point3f(l, -l, -l));
		result.push_back(cv::Point3f(l, -l, l));
		result.push_back(cv::Point3f(l, l, l));
		result.push_back(cv::Point3f(l, l, -l));
		break;

	case 4:
		result.push_back(cv::Point3f(l, l, -l));
		result.push_back(cv::Point3f(-l, l, -l));
		result.push_back(cv::Point3f(-l, -l, -l));
		result.push_back(cv::Point3f(l, -l, -l));
		break;
		
	case 5:
		result.push_back(cv::Point3f(l, -l, l));
		result.push_back(cv::Point3f(-l, -l, l));
		result.push_back(cv::Point3f(-l, l, l));
		result.push_back(cv::Point3f(l, l, l));
		break;
	}

	return result;
}
