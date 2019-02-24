#include "ImageUtils.h"
#include <algorithm>

template <typename T>
void ImageUtils::convolve(T* imgDataPtr, const Dim& dim, const double* filterArray, const int filterSize)
{
	auto* t = new T[dim.getLength()];
	std::copy(&imgDataPtr[0], &imgDataPtr[dim.getLength() - 1], t);
	auto offset = (filterSize - 1) / 2;

	eachPixel<T>(imgDataPtr, dim, [&](T* rgbArray, const long x, const long y)
	{
		double sum[] = {0.0, 0.0, 0.0};
		auto k = 0;

		for (auto j = y - offset; j <= y + offset; j++)
		{
			for (auto i = x - offset; i <= x + offset; i++)
			{
				auto* pixel = getPixelPtr(t, dim, i, j);

				sum[0] += pixel[0] * filterArray[k];
				sum[1] += pixel[1] * filterArray[k];
				sum[2] += pixel[2] * filterArray[k];
				k++;
			}
		}

		rgbArray[0] = static_cast<T>(sum[0]);
		rgbArray[1] = static_cast<T>(sum[1]);
		rgbArray[2] = static_cast<T>(sum[2]);
	});

	delete[] t;
}

long Dim::getLength() const
{
	return this->width * this->height * 3;
}

template <typename T>
void ImageUtils::grayConvolve(T* imgDataPtr, const Dim& dim, const double* filterArray, const int filterSize)
{
	auto* t = new T[dim.getLength()];
	std::copy(&imgDataPtr[0], &imgDataPtr[dim.getLength() - 1], t);
	auto offset = (filterSize - 1) / 2;

	eachPixel<T>(imgDataPtr, dim, [&](T* rgbArray, const long x, const long y)
	{
		auto sum = 0.0;
		auto k = 0;

		for (auto j = y - offset; j <= y + offset; j++)
		{
			for (auto i = x - offset; i <= x + offset; i++)
			{
				sum += getPixelPtr(t, dim, i, j)[0] * filterArray[k];
				k++;
			}
		}

		rgbArray[0] = static_cast<unsigned char>(sum);
		rgbArray[1] = static_cast<unsigned char>(sum);
		rgbArray[2] = static_cast<unsigned char>(sum);
	});

	delete[] t;
}

template <typename T>
void ImageUtils::eachPixel(T* imgDataPtr, const Dim& dim,
	const std::function<void(T* rgbArray, long x, long y)> &eachCallback)
{
	auto *pixelPtr = imgDataPtr;

	for (auto j = 0; j < dim.height; ++j)
	{
		for (auto i = 0; i < dim.width; ++i)
		{
			eachCallback(pixelPtr, i, j);

			pixelPtr += 3;
		}
	}
}

template<typename T>
T* ImageUtils::getPixelPtr(T* imgDataPtr, const Dim& dim, long x, long y)
{
	x = std::max(std::min(x, dim.width - 1), 0L);
	y = std::max(std::min(y, dim.height - 1), 0L);

	auto* pixel = imgDataPtr + y * dim.width * 3 + x * 3;

	if (pixel < imgDataPtr || pixel >= imgDataPtr + dim.getLength() * sizeof T) __debugbreak();

	return pixel;
}

template<typename T>
void ImageUtils::toGray(T* imgDataPtr, const Dim& dim)
{
	eachPixel<T>(imgDataPtr, dim, [](T* rgbArray, const long, const long)
	{
		const auto gray = (rgbArray[0] + rgbArray[1] + rgbArray[2]) / 3;
		rgbArray[0] = gray;
		rgbArray[1] = gray;
		rgbArray[2] = gray;
	});
}

template<typename T>
void ImageUtils::mapColor(T* imgDataPtr, const Dim& dim, T fromMin, T fromMax)
{
	auto range = fromMax - fromMin;

	eachPixel<T>(imgDataPtr, dim, [&](T* rgbArray, const long x, const long y)
	{
		rgbArray[0] = (rgbArray[0] - fromMin) * 255 / range;
		rgbArray[1] = (rgbArray[1] - fromMin) * 255 / range;
		rgbArray[2] = (rgbArray[2] - fromMin) * 255 / range;
	});
}

template <typename T, typename U>
U* ImageUtils::toNewType(T* fromImgDataPtr, const Dim& dim)
{
	auto* newImagePtr = new U[dim.getLength()];

	eachPixel<T>(fromImgDataPtr, dim, [&](const T* fromPixelArray, const long x, const long y)
	{
		auto* newPixelPtr = getPixelPtr<U>(newImagePtr, dim, x, y);
		newPixelPtr[0] = static_cast<U>(fromPixelArray[0]);
		newPixelPtr[1] = static_cast<U>(fromPixelArray[1]);
		newPixelPtr[2] = static_cast<U>(fromPixelArray[2]);
	});

	return newImagePtr;
}

template <typename T>
T* ImageUtils::clone(T* imgDataPtr, const Dim& dim)
{
	auto * t = new T[dim.getLength()];

	std::copy(&imgDataPtr[0], &imgDataPtr[dim.getLength() - 1], t);

	return t;
}
