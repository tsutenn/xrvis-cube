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

	this->capture >> this->frame;
	cv::cvtColor(this->frame, gray, cv::COLOR_BGRA2GRAY);
	cv::threshold(gray, this->adptThr, (double)(this->threshG), 255, cv::THRESH_BINARY_INV);
	cv::Canny(this->adptThr, this->edges, 100, 200);

	std::vector<std::vector<cv::Point>> allContours;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<Marker> detectedMarkers;
	cv::findContours(this->adptThr, allContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < allContours.size(); ++i) {
		if (allContours[i].size() > 4) {
			contours.push_back(allContours[i]);
		}
	}

	if (contours.size() > 0)
	{
		std::vector<cv::Point>  approxCurve;
		std::vector<Marker> possibleMarkers;
		
		for (size_t i = 0; i < contours.size(); ++i)
		{
			
			cv::approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);
			
			if (approxCurve.size() != 4)
				continue;
			
			if (!cv::isContourConvex(approxCurve))
				continue;

			
			float minDist = 1e10;
			for (int i = 0; i < 4; ++i)
			{
				cv::Point vec = approxCurve[i] - approxCurve[(i + 1) % 4];
				float squaredDistance = vec.dot(vec);
				minDist = std::min(minDist, squaredDistance);
			}

			
			if (minDist < 30)
				continue;

			    
			Marker m(5);

			detected_count = approxCurve.size();

			//for (int i = 0; i < 4; ++i)
			//{
			//	m.points.push_back(cv::Point2f(approxCurve[i].x, approxCurve[i].y));
			//}
			//
			//cv::Point v1 = m.points[1] - m.points[0];
			//cv::Point v2 = m.points[2] - m.points[0];
			//double o = (v1.x * v2.y) - (v1.y * v2.x);

			//if (o < 0.0)
			//{
			//	std::swap(m.points[1], m.points[3]);
			//}
			//possibleMarkers.push_back(m);

			// detectedMarkers.push_back(m);

		}
		
		//std::vector< std::pair<int, int> > tooNearCandidates;
		//for (size_t i = 0; i < possibleMarkers.size(); ++i)
		//{
		//	const Marker& m1 = possibleMarkers[i];
		//	
		//	for (size_t j = i + 1; j < possibleMarkers.size(); ++j)
		//	{
		//		const Marker& m2 = possibleMarkers[j];
		//		float distSquared = 0;
		//		for (int c = 0; c < 4; ++c)
		//		{
		//			cv::Point v = m1.points[c] - m2.points[c];
		//			distSquared += v.dot(v);
		//		}
		//		distSquared /= 4;

		//		if (distSquared < 5)
		//		{
		//			tooNearCandidates.push_back(std::pair<int, int>(i, j));
		//		}
		//	}
		//}
		
		//std::vector<bool> removalMask(possibleMarkers.size(), false);
		//for (size_t i = 0; i < tooNearCandidates.size(); ++i)
		//{
		//	float p1 = perimeter(possibleMarkers[tooNearCandidates[i].first].points);
		//	float p2 = perimeter(possibleMarkers[tooNearCandidates[i].second].points);
		//	size_t removalIndex;
		//	if (p1 > p2)
		//		removalIndex = tooNearCandidates[i].second;
		//	else
		//		removalIndex = tooNearCandidates[i].first;

		//	removalMask[removalIndex] = true;
		//}

		//detectedMarkers.clear();
		//for (size_t i = 0; i < possibleMarkers.size(); ++i)
		//{
		//	if (!removalMask[i])
		//		detectedMarkers.push_back(possibleMarkers[i]);
		//}
	}


	// this->detected_count = detectedMarkers.size();

	gray.release();
	loopBlock = false;
	cv::waitKey(1);
}

bool ca::LoopBlock() {
	return loopBlock;
}