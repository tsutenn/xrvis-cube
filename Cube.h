#pragma once

#include <iostream>
#include <algorithm>

#include "Marker.h"

class Cube
{
public:
	Cube(int id, int size);
	Cube(int id, int size, std::vector<std::vector<int>> markers);

	~Cube();

	int GetId() {
		return id;
	}

	Marker Face(int index) {
		return markers[index];
	}

	void SetData(std::vector<std::vector<int>> markers);

protected:
	int id;
	int size;
	std::vector<Marker> markers;
};

