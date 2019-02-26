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

		rgbArray[0] = static_cast<T>(sum);
		rgbArray[1] = static_cast<T>(sum);
		rgbArray[2] = static_cast<T>(sum);
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

template <typename T>
void ImageUtils::eachValue(T* imgDataPtr, const Dim& dim, const std::function<void(T& value)>& eachCallback)
{
	auto *pixelPtr = imgDataPtr;

	for (auto j = 0; j < dim.height; ++j)
	{
		for (auto i = 0; i < dim.width; ++i)
		{
			for (auto k = 0; k < 3; k++)
			{
				eachCallback(*pixelPtr);
				++pixelPtr;
			}
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
	auto range = double(fromMax) - fromMin;

	eachPixel<T>(imgDataPtr, dim, [&](T* rgbArray, long, long)
	{
		auto r = (double(rgbArray[0]) - fromMin) * 255 / range;
		auto g = (double(rgbArray[1]) - fromMin) * 255 / range;
		auto b = (double(rgbArray[2]) - fromMin) * 255 / range;

		if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
			__debugbreak();

		rgbArray[0] = static_cast<T>(r);
		rgbArray[1] = static_cast<T>(g);
		rgbArray[2] = static_cast<T>(b);
	});
}

template <typename T>
double ImageUtils::l2(T* imgData1Ptr, T* imgData2Ptr, const Dim& dim)
{
	double distance = 0;

	eachPixel<T>(imgData1Ptr, dim, [&](T* p1, long x, long y)
	{
		T* p2 = getPixelPtr(imgData2Ptr, dim, x, y);
		distance += sqrt(
			pow(static_cast<double>(p1[0]) - p2[0], 2) + 
			pow(static_cast<double>(p1[1]) - p2[1], 2) + 
			pow(static_cast<double>(p1[2]) - p2[2], 2)
		);
	});

	return distance;
}

template <typename T>
T* ImageUtils::subImage(T* sourceImgDataPtr, const Dim& sourceDim, const long startX, const long startY, const Dim& targetDim)
{
	T* t = new T[targetDim.getLength()];

	for (auto x = 0L; x < targetDim.width; x++)
	{
		for (auto y = 0L; y < targetDim.height; y++)
		{
			auto* tPixel = getPixelPtr(t, targetDim, x, y);
			auto* sPixel = getPixelPtr(sourceImgDataPtr, sourceDim, startX + x, startY + y);

			tPixel[0] = sPixel[0];
			tPixel[1] = sPixel[1];
			tPixel[2] = sPixel[2];
		}
	}

	return t;
}

template <typename T>
void ImageUtils::pasteImage(T* sourceImgDataPtr, const Dim& sourceDim, long startX, long startY, T* pasteImgDataPtr,
	const Dim& pasteDim)
{
	for (auto x = 0L; x < pasteDim.width; x++)
	{
		for (auto y = 0L; y < pasteDim.height; y++)
		{
			auto* pPixel = getPixelPtr(pasteImgDataPtr, pasteDim, x, y);
			auto* sPixel = getPixelPtr(sourceImgDataPtr, sourceDim, startX + x, startY + y);

			sPixel[0] = pPixel[0];
			sPixel[1] = pPixel[1];
			sPixel[2] = pPixel[2];
		}
	}
}

template <typename T>
std::pair<T*, Dim> ImageUtils::maxPool(T* sourceImgDataPtr, const Dim& sourceDim, const long poolSize)
{
	Dim targetDim = { sourceDim.width/poolSize, sourceDim.height/poolSize };
	Dim poolDim = { poolSize, poolSize };

	T* t = new T[targetDim.getLength()];

	for (auto y = 0; y < targetDim.height; y++)
	{
		for (auto x = 0; x < targetDim.width; x++)
		{
			T* clip = subImage(sourceImgDataPtr, sourceDim, x * poolSize, y * poolSize, poolDim);
			T* tPixel = getPixelPtr(t, targetDim, x, y);

			auto max = 0.0;

			eachPixel<T>(clip, poolDim, [&](T* rgbArray, long, long)
			{
				const double sum = rgbArray[0] + rgbArray[1] + rgbArray[2];
				if (sum > max)
				{
					max = sum;
					tPixel[0] = rgbArray[0];
					tPixel[1] = rgbArray[1];
					tPixel[2] = rgbArray[2];
				}
			});

			delete[] clip;
		}
	}

	return { t, targetDim };
}

template <typename T>
std::pair<T*, Dim> ImageUtils::maxMagnitudePool(T* sourceImgDataPtr, const Dim& sourceDim, long poolSize)
{
	Dim targetDim = { sourceDim.width / poolSize, sourceDim.height / poolSize };
	Dim poolDim = { poolSize, poolSize };

	T* t = new T[targetDim.getLength()];

	for (auto y = 0; y < targetDim.height; y++)
	{
		for (auto x = 0; x < targetDim.width; x++)
		{
			T* clip = subImage(sourceImgDataPtr, sourceDim, x * poolSize, y * poolSize, poolDim);
			T* tPixel = getPixelPtr(t, targetDim, x, y);

			auto maxMag = 0.0;

			eachPixel<T>(clip, poolDim, [&](T* rgbArray, long, long)
			{
				const double sum = rgbArray[0] + rgbArray[1] + rgbArray[2];
				if (abs(sum) > maxMag)
				{
					maxMag = abs(sum);
					tPixel[0] = rgbArray[0];
					tPixel[1] = rgbArray[1];
					tPixel[2] = rgbArray[2];
				}
			});

			delete[] clip;
		}
	}

	return { t, targetDim };
}

template <typename T>
std::pair<T*, Dim> ImageUtils::meanPool(T* sourceImgDataPtr, const Dim& sourceDim, const long poolSize)
{
	Dim targetDim = { sourceDim.width / poolSize, sourceDim.height / poolSize };
	Dim poolDim = { poolSize, poolSize };
	const double numPoolPixels = poolDim.width * poolDim.height;

	T* t = new T[targetDim.getLength()];

	for (auto y = 0; y < targetDim.height; y++)
	{
		for (auto x = 0; x < targetDim.width; x++)
		{
			T* clip = subImage(sourceImgDataPtr, sourceDim, x * poolSize, y * poolSize, poolDim);
			T* tPixel = getPixelPtr(t, targetDim, x, y);

			double rSum = 0;
			double gSum = 0;
			double bSum = 0;

			eachPixel<T>(clip, poolDim, [&](T* rgbArray, long, long)
			{
				rSum += rgbArray[0];
				gSum += rgbArray[1];
				bSum += rgbArray[2];
			});

			tPixel[0] = static_cast<T>(rSum / numPoolPixels);
			tPixel[1] = static_cast<T>(gSum / numPoolPixels);
			tPixel[2] = static_cast<T>(bSum / numPoolPixels);

			delete[] clip;
		}
	}

	return { t, targetDim };
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
