#include "ca.h"

ca::ca(int camera_id) {
	this->capture.open(camera_id);
	this->capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	this->capture.set(cv::CAP_PROP_FPS, 30);

	this->canonicalSize.width = 128;
	this->canonicalSize.height = 128;

	this->m_markerCorners2f.push_back(cv::Point2f(0, 0));
	this->m_markerCorners2f.push_back(cv::Point2f(canonicalSize.width - 1, 0));
	this->m_markerCorners2f.push_back(cv::Point2f(canonicalSize.width - 1, canonicalSize.height - 1));
	this->m_markerCorners2f.push_back(cv::Point2f(0, canonicalSize.height - 1));
}

ca::~ca() {
	this->capture.release();
	this->frame.release();
	this->grayFrame.release();
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

	outputImages.clear();
	outputMarkers.clear();

	/*
	 *	generate frames
	 */

	cv::Mat _binaryFrame;

	this->capture >> this->frame;
	cv::cvtColor(this->frame, this->grayFrame, cv::COLOR_BGRA2GRAY);
	cv::threshold(this->grayFrame, _binaryFrame, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);
	morphologyEx(_binaryFrame, _binaryFrame, cv::MORPH_OPEN, cv::Mat());
	morphologyEx(_binaryFrame, this->binaryFrame, cv::MORPH_CLOSE, cv::Mat());
	cv::Canny(this->binaryFrame, this->edgeFrame, 100, 200);

	/*
	 *	Detected the contours of all markers
	 */

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(this->binaryFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	/*
	 *	filter out the appropriate contours
	 */

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

	/*
	 *	Extract the markers from frame and exclude the Incomplete codes
	 */
	
	std::vector<cv::Mat> canonicalMats;

	for (int i = 0; i < approxCurves.size(); i++) {
		cv::Mat canonicalMat;

		std::vector<cv::Point2f> approxCurve2f;
		for (int j = 0; j < approxCurves[i].size(); j++) {
			cv::Point2f p(approxCurves[i][j].x, approxCurves[i][j].y);
			approxCurve2f.push_back(p);
		}

		cv::Mat M = cv::getPerspectiveTransform(approxCurve2f, this->m_markerCorners2f);
		cv::warpPerspective(this->grayFrame, canonicalMat, M, this->canonicalSize);
		threshold(canonicalMat, canonicalMat, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		int cellSize = canonicalMat.rows / (this->markerSize + 2);
		bool integrity = true;

		for (int y = 0; y < markerSize + 2 && integrity; y++) {
			int inc = (y == 0 || y == (markerSize + 1)) ? 1 : (markerSize + 1);
			for (int x = 0; x < markerSize + 2 && integrity; x += inc) {
				int cellX = x * cellSize;
				int cellY = y * cellSize;

				cv::Mat cell = canonicalMat(cv::Rect(cellX, cellY, cellSize, cellSize));
				int nZ = cv::countNonZero(cell);

				integrity = nZ < (cellSize * cellSize) / 2;
			}
		}

		if(integrity)
			canonicalMats.push_back(canonicalMat);
	}

	/*
	 *	convert the image to marker
	 */

	std::vector<Marker> canonicalMarkers;

	for (int i = 0; i < canonicalMats.size(); i++) {
		int cellSize = canonicalMats[i].rows / (this->markerSize + 2);

		std::vector<std::vector<int>> m(markerSize, std::vector<int>(markerSize));

		for (int y = 0; y < markerSize; y++)
		{
			for (int x = 0; x < markerSize; x++)
			{
				int cellX = (x + 1) * cellSize;
				int cellY = (y + 1) * cellSize;
				cv::Mat cell = canonicalMats[i](cv::Rect(cellX, cellY, cellSize, cellSize));

				int nZ = cv::countNonZero(cell);
				m[y][x] = (nZ >(cellSize * cellSize) / 2) ? 1 : 0;
			}
		}

		Marker canonicalMarker(markerSize, m);
		canonicalMarkers.push_back(canonicalMarker);

		outputImages.push_back(canonicalMats[i]);
		outputMarkers.push_back(canonicalMarker);
	}

	/*
	 *	compare markers with that in dataset
	 */

	for (int i = 0; i < canonicalMarkers.size(); i++) {

	}

	loopBlock = false;
	cv::waitKey(1);
}

bool ca::LoopBlock() {
	return loopBlock;
}

int ca::getMarkerSize() {
	return markerSize;
}
