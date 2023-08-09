#include "ca.h"

ca::ca(int camera_id) {
	this->capture.open(camera_id);
	this->capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	this->capture.set(cv::CAP_PROP_FPS, 30);
}

ca::~ca() {
	this->capture.release();
	this->frame.release();
	this->adptThr.release();
	this->edges.release();
}

cv::Mat* ca::getFrame() {
	return &(this->frame);
}

cv::Mat* ca::getAdptThr() {
	return &(this->adptThr);
}

cv::Mat* ca::getEdges()
{
	return &(this->edges);
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

void ca::fun() {
	loopBlock = true;

	cv::Mat gray;
	int cnt = 0;

	this->capture >> this->frame;
	cv::cvtColor(this->frame, gray, cv::COLOR_BGRA2GRAY);
	cv::threshold(gray, this->adptThr, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);
	cv::Canny(this->adptThr, this->edges, 100, 200);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(this->edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < contours.size(); ++i) {
		cv::Rect boundingRect = cv::boundingRect(contours[i]);
		cv::Mat roi = frame(boundingRect);
	}

	this->detected_count = contours.size();

	gray.release();
	loopBlock = false;
	cv::waitKey(1);
}

bool ca::LoopBlock() {
	return loopBlock;
}