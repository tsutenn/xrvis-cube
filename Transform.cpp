#include "Transform.h"

Transform::Transform()
{
	position = cv::Vec3f(0, 0, 0);
	rotation = cv::Vec4f(0, 0, 0, 1);

	translation = cv::Vec3f(0, 0, 0);
	rotationMatrix = cv::Matx33f::eye();
}

cv::Vec3f Transform::ToCoordinates(cv::Vec3f point, Transform& target_transform)
{
	cv::Vec3f point_c = rotationMatrix * point + translation;
	cv::Vec3f point_t = target_transform.rotationMatrix.t() * (point_c - target_transform.translation);
	return point_t;
}

Transform Transform::zeros()
{
	Transform zero;
	zero.rotation = cv::Vec4f::zeros();
	zero.position = cv::Vec3f::zeros();
	zero.rotationMatrix = cv::Matx33f::zeros();
	zero.translation = cv::Vec3f::zeros();
	
	return zero;
}

void Transform::TransformToCoordinates(Transform& target_transform)
{
	position = ToCoordinates(cv::Vec3f::zeros(), target_transform);
	
	cv::Vec3f point(1, 1, 1);
	cv::Vec3f op = point - cv::Vec3f::zeros();

	cv::Vec3f point_t = ToCoordinates(point, target_transform);
	cv::Vec3f op_t = point_t - position;

	cv::Vec3f axis = op.cross(op_t) / (cv::norm(op) * cv::norm(op_t));

	float cos_theta = op.dot(op_t) / (cv::norm(op) * cv::norm(op_t));
	float theta = acos(cos_theta);

	rotation = cv::Vec4f(axis[0] * sin(theta / 2), axis[1] * sin(theta / 2), axis[2] * sin(theta / 2), cos(theta / 2));
}

const char* Transform::GetPositionString()
{
	std::string positionStr = "Position: [" +
		std::to_string(position[0]) + ", " +
		std::to_string(position[1]) + ", " +
		std::to_string(position[2]) + "]";

	char* result = new char[positionStr.size() + 1];
	std::strcpy(result, positionStr.c_str());

	return result;
}

const char* Transform::GetRotationString()
{
	std::string rotationStr = "Rotation: [" +
		std::to_string(rotation[0]) + ", " +
		std::to_string(rotation[1]) + ", " +
		std::to_string(rotation[2]) + ", " +
		std::to_string(rotation[3]) + "]";

	char* result = new char[rotationStr.size() + 1];
	std::strcpy(result, rotationStr.c_str());

	return result;
}

Transform& Transform::operator=(const Transform& other)
{
	if (this == &other) {
		return *this;
	}
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			rotationMatrix(i, j) = other.rotationMatrix(i, j);
		}
		position[i] = other.position[i];
		translation[i] = other.translation[i];
	}

	for (int i = 0; i < 4; i++) {
		rotation[i] = other.rotation[i];
	}

	return *this;
}

Transform Transform::Average(std::vector<Transform> transform_list)
{
	Transform average_transform = zeros();
	for (int i = 0; i < transform_list.size(); i++) {
		average_transform.position += transform_list[i].position;
		average_transform.rotation += transform_list[i].rotation;
		average_transform.translation += transform_list[i].translation;
		average_transform.rotationMatrix += transform_list[i].rotationMatrix;
	}

	average_transform.position[0] /= transform_list.size();
	average_transform.position[1] /= transform_list.size();
	average_transform.position[2] /= transform_list.size();

	average_transform.rotation[0] /= transform_list.size();
	average_transform.rotation[1] /= transform_list.size();
	average_transform.rotation[2] /= transform_list.size();
	average_transform.rotation[3] /= transform_list.size();
	average_transform.rotation = average_transform.rotation / cv::norm(average_transform.rotation);

	average_transform.translation[0] /= transform_list.size();
	average_transform.translation[1] /= transform_list.size();
	average_transform.translation[2] /= transform_list.size();

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			average_transform.rotationMatrix(i, j) /= transform_list.size();
		}
	}
	// average_transform.rotationMatrix = average_transform.rotationMatrix / cv::norm(average_transform.rotationMatrix);

	return average_transform;
}
