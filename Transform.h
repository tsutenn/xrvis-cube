#pragma once

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

class Transform
{
public:
	Transform();

	cv::Vec3f position;
	cv::Vec4f rotation;

	cv::Vec3f translation;
	cv::Matx33f rotationMatrix;

	static Transform zeros();

	cv::Vec3f ToCoordinates(cv::Vec3f point, Transform& target_transform);
	void TransformToCoordinates(Transform& target_transform);

	const char* GetPositionString();
	const char* GetRotationString();

	Transform& Transform::operator=(const Transform& other);

	static Transform Average(std::vector<Transform>);
};
