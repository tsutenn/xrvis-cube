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

	//this->camMatrix = (cv::Mat_<double>(3, 3) << 922.80181267, 0, 277.82133, 0, 915.382409, 197.2520247, 0, 0, 1);
	//this->distCoeff = (cv::Mat_<double>(5, 1) << -0.4106936737, 0.1123164, -0.00748416, 0.00330122, 4.88862);

	this->camMatrix = (cv::Mat_<double>(3, 3) << 896.8655716001124, 0, 337.0996608643468, 0, 895.8676984030446, 248.6402429023775, 0, 0, 1);
	this->distCoeff = (cv::Mat_<double>(5, 1) << -0.3160636131931209, -2.481283726165089, -0.003328328493339427, -0.0005555183747696143, 22.0213498782796);
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

Cube& ca::GetBaseCube() {
	return baseCube;
}

std::vector<Cube>& ca::GetCubeList() {
	return cubeList;
}

void ca::SetThreshG(int threshG) {
	this->threshG = threshG;
}

void ca::SetCubeInfo(int markerSize, double markerLength, double markerMargin)
{
	this->markerSize = markerSize;
	this->markerLength = markerLength;
	this->markerMargin = markerMargin;
}

void ca::SetCubeList(std::vector<Cube> cube_list, Cube base_cube)
{
	this->cubeList = cube_list;
	this->baseCube = base_cube;
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

void ca::GenerateFrames(cv::Mat& input, cv::Mat& gray_frame, cv::Mat& binary_frame, cv::Mat& edge_frame) {
	cv::Mat _binaryFrame;

	cv::cvtColor(input, gray_frame, cv::COLOR_BGRA2GRAY);
	cv::threshold(gray_frame, _binaryFrame, (double)(threshG), 255, cv::THRESH_BINARY_INV);
	morphologyEx(_binaryFrame, _binaryFrame, cv::MORPH_OPEN, cv::Mat());
	morphologyEx(_binaryFrame, binary_frame, cv::MORPH_CLOSE, cv::Mat());
	// cv::threshold(gray_frame, binary_frame, (double)(threshG), 255, cv::THRESH_BINARY_INV);
	cv::Canny(binary_frame, edge_frame, 100, 200);

	_binaryFrame.release();
}

void ca::GenerateFramesFromCapture(cv::Mat& raw_frame, cv::Mat& gray_frame, cv::Mat& binary_frame, cv::Mat& edge_frame)
{
	cv::Mat _binaryFrame;
	capture >> raw_frame;

	GenerateFrames(raw_frame, gray_frame, binary_frame, edge_frame);
}

std::vector<Marker> ca::ExtractMarkersFromFrame(cv::Mat& gray_frame, cv::Mat& binary_frame) {
	std::vector<Marker> canonicalMarkers;

	/*
	 *	Detected the contours of all markers
	 */

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary_frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

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

	for (int i = 0; i < approxCurves.size(); i++) {
		cv::Mat canonicalMat;

		std::vector<cv::Point2f> approxCurve2f;
		for (int j = 0; j < approxCurves[i].size(); j++) {
			approxCurve2f.push_back(cv::Point2f(approxCurves[i][j].x, approxCurves[i][j].y));
		}

		cv::Mat M = cv::getPerspectiveTransform(approxCurve2f, m_markerCorners2f);
		cv::warpPerspective(gray_frame, canonicalMat, M, canonicalSize);
		threshold(canonicalMat, canonicalMat, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		int cellSize = canonicalMat.rows / (markerSize + 2);
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
			canonicalMarker.points = approxCurve2f;

			canonicalMarkers.push_back(canonicalMarker);
		}
	}
	return canonicalMarkers;
}

std::vector<Marker> ca::GenerateBaseCube(Cube& base_cube, std::vector<Marker>& marker_list, int min_distance){
	std::vector<Marker> markers_on_cube;
	std::vector<int> marker_positions;
	std::vector<int> marker_rotations;

	for (int j = 0; j < marker_list.size(); j++) {
		int marker_rotation = 0;
		int face_id = base_cube.CheckFaceOnCube(marker_list[j], min_distance, marker_rotation);
		if (face_id >= 0) {
			markers_on_cube.push_back(marker_list[j]);
			marker_positions.push_back(face_id);
			marker_rotations.push_back(marker_rotation);
		}
	}

	if (markers_on_cube.size() > 0) {
		std::vector<Transform> transform_list;

		for (int j = 0; j < markers_on_cube.size(); j++) {
			Transform _transform;

			cv::Mat rvec, tvec;
			cv::solvePnP(base_cube.FacePoints(marker_positions[j], marker_rotations[j], markerLength, markerMargin),
				markers_on_cube[j].points, camMatrix, distCoeff, rvec, tvec);

			cv::Mat _rotationMatrix_mat;
			cv::Rodrigues(rvec, _rotationMatrix_mat);

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					_transform.rotationMatrix(i, j) = _rotationMatrix_mat.at<double>(i, j);
				}
				_transform.translation[i] = tvec.at<double>(i, 0);
			}

			transform_list.push_back(_transform);
		}

		base_cube.transform = Transform::Average(transform_list);
	}

	return markers_on_cube;
}

std::vector<Cube> ca::GenerateCubes(std::vector<Cube>& cube_list, Cube& base_cube, std::vector<Marker>& marker_list, int min_distance) {
	std::vector<Cube> detected_cubes;

	for (int i = 0; i < cube_list.size(); i++) {
		std::vector<Marker> markers_on_cube;
		std::vector<int> marker_positions;
		std::vector<int> marker_rotations;

		for (int j = 0; j < marker_list.size(); j++) {
			int marker_rotation = 0;
			int face_id = cube_list[i].CheckFaceOnCube(marker_list[j], min_distance, marker_rotation);
			if (face_id >= 0) {
				markers_on_cube.push_back(marker_list[j]);
				marker_positions.push_back(face_id);
				marker_rotations.push_back(marker_rotation);
			}
		}

		if (markers_on_cube.size() > 0) {
			std::vector<Transform> transform_list;

			for (int j = 0; j < markers_on_cube.size(); j++) {
				Transform _transform;

				cv::Mat rvec, tvec;
				cv::solvePnP(cube_list[i].FacePoints(marker_positions[j], marker_rotations[j], markerLength, markerMargin),
					markers_on_cube[j].points, camMatrix, distCoeff, rvec, tvec);

				cv::Mat _rotationMatrix_mat;
				cv::Rodrigues(rvec, _rotationMatrix_mat);

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						_transform.rotationMatrix(i, j) = _rotationMatrix_mat.at<double>(i, j);
					}
					_transform.translation[i] = tvec.at<double>(i, 0);
				}

				transform_list.push_back(_transform);
			}
		
			Transform transform = Transform::Average(transform_list);
			transform.TransformToCoordinates(base_cube.transform);
			cube_list[i].GenerateTransformAuto(transform);
			//cube_list[i].transform = transform;
			detected_cubes.push_back(cube_list[i]);
		}
	}

	return detected_cubes;
}

void ca::Fun() {
	loopBlock = true;
	outputMarkers.clear();

	capture >> frame;
	GenerateFrames(frame, grayFrame, binaryFrame, edgeFrame);
	auto canonicalMarkers = ExtractMarkersFromFrame(grayFrame, binaryFrame);

	GenerateBaseCube(baseCube, canonicalMarkers, 1);
	auto detectedCubes = GenerateCubes(cubeList, baseCube, canonicalMarkers, 1);

	for (int i = 0; i < canonicalMarkers.size(); i++) {
		outputMarkers.push_back(canonicalMarkers[i]);
	}

	loopBlock = false;
	cv::waitKey(10);
}

void ca::Wait(int ms) {
	cv::waitKey(ms);
}
