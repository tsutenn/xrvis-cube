#include "Marker.h"

#include <qdebug.h>

Marker::Marker(int size) {
	this->marker = new int* [size];
	for (int i = 0; i < size; i++) {
		this->marker[i] = new int[size];

		for (int j = 0; j < size; j++) {
			this->marker[i][j] = 0;
		}
	}

	this->size = size;
}

Marker::Marker(int size, int** marker) {
	this->marker = new int* [size];
	for (int i = 0; i < size; i++) {
		this->marker[i] = new int[size];

		for (int j = 0; j < size; j++) {
			this->marker[i][j] = marker[i][j];
		}
	}

	this->size = size;
}

Marker::Marker(int size, int* marker) {
	this->marker = new int* [size];
	for (int i = 0; i < size; i++) {
		this->marker[i] = new int[size];

		for (int j = 0; j < size; j++) {
			this->marker[i][j] = marker[i * size + j];
		}
	}

	this->size = size;
}

Marker::~Marker() {
	// qDebug() << marker[0][0];
	/*for (int i = 0; i < size; i++) {
		delete[] marker[i];
	}*/
	// delete[] marker;
	// marker = nullptr;
}

bool Marker::operator==(const Marker& other) {
	if (this->size != other.size) {
		return false;
	}

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			if (this->marker[i][j] != other.marker[i][j]) {
				return false;
			}
		}
	}

	return true;
}

std::ostream& operator<<(std::ostream& strm, const Marker& m) {
	// TODO: insert return statement here
	strm << "[\n";
	for (int i = 0; i < m.size; i++) {
		strm << "\t[\t";
		for (int j = 0; j < m.size; j++) {
			strm << m.marker[i][j] << "\t";
		}
		strm << "]\n";
	}
	strm << "]";
	return strm;
}

Marker Marker::operator+(const int& other) {
	Marker out(this->size, this->marker);
	int o = other % 4;

	if (o == 0) {
		return out;
	}

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			out.marker[i][j] = this->marker[size - j - 1][i];
		}
	}

	return out + (o - 1);
}

Marker Marker::operator+=(const int& other) {
	Marker out(this->size, this->marker);
	out = out + other;
	this->Copy(out);
	return out;
}

void Marker::Copy(const Marker& other) {
	this->marker = new int* [other.size];
	for (int i = 0; i < other.size; i++) {
		this->marker[i] = new int[other.size];

		for (int j = 0; j < other.size; j++) {
			this->marker[i][j] = other.marker[i][j];
		}
	}

	this->size = other.size;
}

int Marker::At(int x, int y)
{
	return marker[y][x];
}
