#include "Cube.h"

Cube::Cube(int id, int size) {
	this->id = id;
	this->size = size;
}

Cube::Cube(int id, int size, int** markers)
{
	this->id = id;
	this->size = size;
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}

Cube::~Cube() {
}

void Cube::SetData(int** markers)
{
	for (int i = 0; i < 6; i++) {
		Marker marker(size, markers[i]);
		this->markers.push_back(marker);
	}
}
