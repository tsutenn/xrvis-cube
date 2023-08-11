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
	std::vector<Marker> detectedMarkers;
	cv::findContours(this->binaryFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	int cnt = 0;

	for (size_t i = 0; i < contours.size(); ++i) {
		if (contours[i].size() > 4) {
			std::vector<cv::Point>  approxCurve;
			cv::approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);

			if (approxCurve.size() == 4 && cv::isContourConvex(approxCurve)) {
				cnt++;
			}
		}
	}

	this->detected_count = cnt;

	grayFrame.release();
	loopBlock = false;
	cv::waitKey(1);
}

bool ca::LoopBlock() {
	return loopBlock;
}