#include "Cube.h"

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;
}

Cube::Cube(int id, int size, std::vector<std::vector<int>> markers)
{
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}

	this->id = id;
	this->size = size;
}

Cube::~Cube() {
}

void Cube::SetData(std::vector<std::vector<int>> markers)
{
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}
