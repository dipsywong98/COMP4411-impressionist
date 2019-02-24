#include "Matrix.h"


template <typename T>
Matrix<T>::Matrix(int w, int h):w(w), h(h) {
	mV = std::vector<std::vector<T>>(h);
	for (int i = 0; i<h; i++)
	{
		mV[i] = std::vector<T>(w);
	}
}

template <typename T>
Matrix<T>::~Matrix()
{
}
