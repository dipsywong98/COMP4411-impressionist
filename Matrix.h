#pragma once
#include <vector>

template<typename T>
class Matrix
{
public:
	Matrix(int w, int h);
	~Matrix();
	std::vector<std::vector<T>> mV;
};

