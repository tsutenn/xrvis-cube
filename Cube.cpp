#include "Cube.h"

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;
}

Cube::Cube(int id, int size, std::vector<std::vector<int>> markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}

	this->id = id;
	this->size = size;
}

Cube::~Cube() {
}

int Cube::GetId() {
	return id;
}

Marker Cube::Face(int index) {
	return markers[index];
}

void Cube::SetData(std::vector<std::vector<int>> markers) {
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}

int Cube::checkFaceOnCube(Marker marker) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (markers[i] + j == marker)
				return i;
		}
	}
	return -1;
}
