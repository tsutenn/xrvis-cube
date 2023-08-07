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
}

cv::Mat* ca::getFrame() {
	return &(this->frame);
}

cv::Mat* ca::getAdptThr() {
	return &(this->adptThr);
}

int ca::getThreshG() {
	return this->threshG;
}

void ca::setThreshG(int threshG) {
	this->threshG = threshG;
}

void ca::fun() {
	cv::Mat gray;

	this->capture >> this->frame;
	cv::cvtColor(this->frame, gray, cv::COLOR_BGRA2GRAY);
	cv::threshold(gray, this->adptThr, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);

	cv::waitKey(1);
}

void ca::fun(int delay) {
	cv::Mat gray;

	this->capture >> this->frame;
	cv::cvtColor(this->frame, gray, cv::COLOR_BGRA2GRAY);
	cv::threshold(gray, this->adptThr, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);

	cv::waitKey(delay);
}