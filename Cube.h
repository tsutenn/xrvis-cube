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

	int GetId();
	Marker Face(int index);

	void SetData(std::vector<std::vector<int>> markers);

	int checkFaceOnCube(Marker marker);

protected:
	int id;
	int size;
	std::vector<Marker> markers;
};
