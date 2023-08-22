#include "Marker.h"

Marker::Marker(int size) {
	std::vector<std::vector<int>> marker(size, std::vector<int>(size));

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			marker[i][j] = 0;
		}
	}

	this->size = size;
	this->marker = marker;
}

Marker::Marker(int size, std::vector<int>& marker_data) {
	std::vector<std::vector<int>> marker(size, std::vector<int>(size));

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			marker[i][j] = marker_data[i * size + j];
		}
	}

	this->size = size;
	this->marker = marker;
}

Marker::Marker(int size, std::vector<std::vector<int>>& marker_data) {
	std::vector<std::vector<int>> marker(size, std::vector<int>(size));

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			marker[i][j] = marker_data[i][j];
		}
	}

	this->size = size;
	this->marker = marker;
}

Marker::~Marker() {
	image.release();
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

int Marker::operator-(const Marker& other)
{
	if (this->size != other.size) {
		return INT_MAX;
	}

	int result = 0;
	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			if (this->marker[i][j] != other.marker[i][j]) {
				result += 1;
			}
		}
	}

	return result;
}

std::ostream& operator<<(std::ostream& strm, const Marker& m) {
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
			out.marker[i][j] = this->marker[j][size - i - 1];
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

Marker Marker::operator-(const int& other)
{
	Marker out(this->size, this->marker);
	int o = other % 4;

	if (o == 0) {
		return out;
	}

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			out.marker[i][j] = this->marker[size - j - 1][i];
			// out.marker[i][j] = this->marker[j][size - i - 1];
		}
	}

	return out - (o - 1);
}

Marker Marker::operator-=(const int& other)
{
	Marker out(this->size, this->marker);
	out = out - other;
	this->Copy(out);
	return out;
}

void Marker::Copy(const Marker& other) {
	std::vector<std::vector<int>> marker(other.size, std::vector<int>(other.size));

	for (int i = 0; i < other.size; i++) {
		for (int j = 0; j < other.size; j++) {
			marker[i][j] = other.marker[i][j];
		}
	}

	this->size = other.size;
	this->marker = marker;
}

int Marker::At(int x, int y) {
	return this->marker[y][x];
}

const char* Marker::ToString() {
	std::string result = "[";

	for (int i = 0; i < size; i++) {
		result += "\n\t[";

		for (int j = 0; j < size; j++) {
			result += std::to_string(marker[i][j]);
			if (j < size - 1) {
				result += "\t";
			}
		}

		result += "]";
	}

	result += "\n]";

	char* rnt = new char[result.size() + 1];
	std::strcpy(rnt, result.c_str());
	return rnt;
}





