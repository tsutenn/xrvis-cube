#include "ca.h"

ca::ca(int camera_id) {
	this->capture.open(camera_id);
	this->capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	this->capture.set(cv::CAP_PROP_FPS, 30);
}

ca::~ca() {
	this->capture.release();
	this->frame.release();
	this->binaryFrame.release();
	this->edgeFrame.release();
}

cv::Mat* ca::getFrame() {
	return &(this->frame);
}

cv::Mat* ca::getBinaryFrame() {
	return &(this->binaryFrame);
}

cv::Mat* ca::getEdgeFrame()
{
	return &(this->edgeFrame);
}

int ca::getThreshG() {
	return this->threshG;
}

int ca::getDetectedCount()
{
	return this->detected_count;
}

void ca::setThreshG(int threshG) {
	this->threshG = threshG;
}

void ca::setCubeInfo(int markerSize, double markerLength, double markerMargin, int cubeCount)
{
	this->markerSize = markerSize;
	this->markerLength = markerLength;
	this->markerMargin = markerMargin;
	this->cubeCount = cubeCount;
}

float ca::perimeter(const std::vector<cv::Point>& a)
{
	float sum = 0, dx, dy;

	for (size_t i = 0; i < a.size(); ++i)
	{
		size_t i2 = (i + 1) % a.size();

		dx = a[i].x - a[i2].x;
		dy = a[i].y - a[i2].y;

		sum += sqrt(dx * dx + dy * dy);
	}

	return sum;
}

void ca::fun() {
	loopBlock = true;

	cv::Mat grayFrame;

	this->capture >> this->frame;
	cv::cvtColor(this->frame, grayFrame, cv::COLOR_BGRA2GRAY);
	cv::threshold(grayFrame, grayFrame, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);
	morphologyEx(grayFrame, grayFrame, cv::MORPH_OPEN, cv::Mat());
	morphologyEx(grayFrame, this->binaryFrame, cv::MORPH_CLOSE, cv::Mat());
	cv::Canny(this->binaryFrame, this->edgeFrame, 100, 200);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(this->binaryFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> approxCurves;
	std::vector<float> perimeters;

	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 4) {
			std::vector<cv::Point>  approxCurve;
			cv::approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);

			if (approxCurve.size() == 4 && cv::isContourConvex(approxCurve)) {
				cv::Point v1 = approxCurve[1] - approxCurve[0];
				cv::Point v2 = approxCurve[2] - approxCurve[0];
				float o = (v1.x * v2.y) - (v1.y * v2.x);

				if (o < 0.0)
				{
					std::swap(approxCurve[1], approxCurve[3]);
				}
				
				int j = 0;
				for (j = 0; j < approxCurves.size(); j++) {
					float distSquared = 0;
					for (int k = 0; k < 4; k++)
					{
						cv::Point v = approxCurves[j][k] - approxCurve[k];
						distSquared += v.dot(v);
					}
					distSquared /= 4;

					if (distSquared < 5)
					{
						break;
					}
				}

				if (j < approxCurves.size()) {
					float p0 = perimeters[j];
					float p1 = perimeter(approxCurve);
					approxCurves[j] = p0 < p1 ? approxCurves[j] : approxCurve;
					perimeters[j] = p0 < p1 ? p0 : p1;
				}
				else {
					approxCurves.push_back(approxCurve);
					perimeters.push_back(perimeter(approxCurve));
				}
			}
		}
	}

	this->detected_count = approxCurves.size();

	grayFrame.release();
	loopBlock = false;
	cv::waitKey(1);
}

bool ca::LoopBlock() {
	return loopBlock;
}