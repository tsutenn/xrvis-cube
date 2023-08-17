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

	this->camMatrix = (cv::Mat_<double>(3, 3) << 922.80181267, 0, 277.82133, 0, 915.382409, 197.2520247, 0, 0, 1);
	this->distCoeff = (cv::Mat_<double>(5, 1) << -0.4106936737, 0.1123164, -0.00748416, 0.00330122, 4.88862);
}

ca::~ca() {
	this->capture.release();
	this->frame.release();
	this->grayFrame.release();
	this->binaryFrame.release();
	this->edgeFrame.release();
}

cv::Mat* ca::GetFrame() {
	return &(this->frame);
}

cv::Mat* ca::GetBinaryFrame() {
	return &(this->binaryFrame);
}

cv::Mat* ca::GetEdgeFrame()
{
	return &(this->edgeFrame);
}

int ca::GetThreshG() {
	return this->threshG;
}

bool ca::LoopBlock() {
	return loopBlock;
}

int ca::GetMarkerSize() {
	return markerSize;
}

void ca::SetThreshG(int threshG) {
	this->threshG = threshG;
}

void ca::SetCubeInfo(int markerSize, double markerLength, double markerMargin, int cubeCount)
{
	this->markerSize = markerSize;
	this->markerLength = markerLength;
	this->markerMargin = markerMargin;
	this->cubeCount = cubeCount;
}

void ca::SetCubeArray(std::vector<Cube> cubes)
{
	this->cubes = cubes;
}

float ca::Perimeter(const std::vector<cv::Point>& a)
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

void ca::Fun() {
	loopBlock = true;

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
					float p1 = Perimeter(approxCurve);
					approxCurves[j] = p0 < p1 ? approxCurves[j] : approxCurve;
					perimeters[j] = p0 < p1 ? p0 : p1;
				}
				else {
					approxCurves.push_back(approxCurve);
					perimeters.push_back(Perimeter(approxCurve));
				}
			}
		}
	}

	/*
	 *	Extract the markers from frame and exclude the Incomplete codes
	 */
	
	// std::vector<cv::Mat> canonicalMats;
	std::vector<Marker> canonicalMarkers;

	for (int i = 0; i < approxCurves.size(); i++) {
		cv::Mat canonicalMat;

		std::vector<cv::Point2f> approxCurve2f;
		for (int j = 0; j < approxCurves[i].size(); j++) {
			approxCurve2f.push_back(cv::Point2f(approxCurves[i][j].x, approxCurves[i][j].y));
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

		if (integrity) {

			/*
			 *	convert the image to marker
			 */

			// int cellSize = canonicalMat.rows / (this->markerSize + 2);
			std::vector<std::vector<int>> m(markerSize, std::vector<int>(markerSize));

			for (int my = 0; my < markerSize; my++)
			{
				for (int mx = 0; mx < markerSize; mx++)
				{
					int cellX = (mx + 1) * cellSize;
					int cellY = (my + 1) * cellSize;
					cv::Mat cell = canonicalMat(cv::Rect(cellX, cellY, cellSize, cellSize));

					int nZ = cv::countNonZero(cell);
					m[my][mx] = (nZ > (cellSize * cellSize) / 2) ? 1 : 0;
				}
			}

			Marker canonicalMarker(markerSize, m);
			canonicalMarker.image = canonicalMat;

			canonicalMarkers.push_back(canonicalMarker);
			// outputMarkers.push_back(canonicalMarker);
		}
	}

	for (int i = 0; i < cubes.size(); i++) {
		// std::vector<Marker> markers_on_cube;

		for (int j = 0; j < canonicalMarkers.size(); j++) {
			int face_id = cubes[i].CheckFaceOnCube(canonicalMarkers[j]);
			if (face_id > -1) {
				// markers_on_cube.push_back(canonicalMarkers[j]);
				outputMarkers.push_back(canonicalMarkers[j]);
			}
		}
	}

	loopBlock = false;
	cv::waitKey(1);
}
