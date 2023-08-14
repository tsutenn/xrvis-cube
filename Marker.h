#pragma once

#include <iostream>
#include <vector>
#include <cmath>

class Marker
{

public:
	Marker(int size);
	Marker(int size, int** marker);
	Marker(int size, int* marker);
	~Marker();

	bool operator==(const Marker& other);
	Marker operator+(const int& other);
	Marker operator+=(const int& other);

	void Copy(const Marker& other);

	int At(int x, int y);

protected:
	int** marker;
	int size;

	friend std::ostream& operator<<(std::ostream& strm, const Marker& m);
};
