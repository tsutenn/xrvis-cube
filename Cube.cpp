#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;

	this->translationVector = cv::Vec3f(0, 0, 0);
	this->rotationMatrix = cv::Matx33f(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

Cube::Cube(int id, int size, std::vector<std::vector<int>>& markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}

	this->id = id;
	this->size = size;

	this->translationVector = cv::Vec3f(0, 0, 0);
	this->rotationMatrix = cv::Matx33f(1, 0, 0, 0, 1, 0, 0, 0, 1);
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

void Cube::SetTranslationVector(cv::Vec3f& translation)
{
	this->translationVector[0] = translation[0];
	this->translationVector[1] = translation[1];
	this->translationVector[2] = translation[2];
}

void Cube::SetRotationMatrix(cv::Matx33f& rotationMatrix)
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->rotationMatrix(i, j) = rotationMatrix(i, j);
		}
	}
}

void Cube::GenerateTranslation(Cube base_cube)
{
	this->translation = base_cube.GetRotationMatrix().t() * (this->translationVector - base_cube.GetTranslationVector());
}

cv::Vec3f Cube::GetTranslationVector()
{
	return translationVector;
}

cv::Matx33f Cube::GetRotationMatrix()
{
	return rotationMatrix;
}

cv::Vec3f Cube::GetTranslation()
{
	return translation;
}

cv::Vec3f Cube::GetRotation()
{
	return rotation;
}

const char* Cube::GetTransformInString() {
	std::string translationStr = "Translation: [" +
		std::to_string(translation[0]) + ", " +
		std::to_string(translation[1]) + ", " +
		std::to_string(translation[2]) + "]";

	char* result = new char[translationStr.size() + 1];
	std::strcpy(result, translationStr.c_str());

	return result;
}

const char* Cube::GetRotationInString()
{
	std::string rotationStr = "Rotation: [" +
		std::to_string(rotation[0]) + ", " +
		std::to_string(rotation[1]) + ", " +
		std::to_string(rotation[2]) + "]";

	char* result = new char[rotationStr.size() + 1];
	std::strcpy(result, rotationStr.c_str());

	return result;
}

bool Cube::isRotationMatrix(cv::Mat& R)
{
	cv::Mat Rt;
	transpose(R, Rt);
	cv::Mat shouldBeIdentity = Rt * R;
	cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());

	return  norm(I, shouldBeIdentity) < 1e-6;
}

cv::Vec3f Cube::rotationMatrixToEulerAngles(cv::Mat& R)
{

	assert(isRotationMatrix(R));

	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));

	bool singular = sy < 1e-6; 

	float x, y, z;
	if (!singular)
	{
		x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
		y = atan2(-R.at<double>(2, 0), sy);
		z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
	}
	else
	{
		x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
		y = atan2(-R.at<double>(2, 0), sy);
		z = 0;
	}

	return cv::Vec3f(x, y, z) * 180 / CV_PI;
}
