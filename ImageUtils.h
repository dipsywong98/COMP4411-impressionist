#pragma once
#include <functional>

struct Dim
{
	long width;
	long height;
	long getLength() const;
};

class ImageUtils
{
public:
	static constexpr double SOBEL_X[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	static constexpr double SOBEL_Y[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
	static constexpr double LAPLACE_3_0[] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 };
	static constexpr double LAPLACE_3_1[] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
	static constexpr double LOW_PASS_3[] = { 0.333, 0.333, 0.333 , 0.333 , 0.333 , 0.333 , 0.333 , 0.333 , 0.333 };
	static constexpr double LOW_PASS_5[] = { 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04 };
	static constexpr double GAUSSIAN_7[] = { 0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291, 0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446, 0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363, 0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036 };

	/**
	 * Performs a convolution to an image data array.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @param filterArray a one-dimensional array containing the filter in rows.
	 * @param filterSize size of the filter.
	 */
	template <typename T>
	static void convolve(T* imgDataPtr, const Dim& dim, const double* filterArray, int filterSize);

	/**
	 * Performs a convolution on a gray level image. Effectively only calculating values on the R color channel.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @param filterArray a one-dimensional array containing the filter in rows.
	 * @param filterSize size of the filter.
	 */
	template <typename T>
	static void grayConvolve(T* imgDataPtr, const Dim& dim, const double* filterArray, int filterSize);


	/**
	 * Performs a for-loop on each pixel, (every 3 elements,) in the image data array.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @param eachCallback A callback function that is run for each pixel.
	 */
	template <typename T>
	static void eachPixel(T* imgDataPtr, const Dim& dim, const std::function<void(T* rgbArray, long x, long y)> &eachCallback);

	/**
	 * Retrieves the pointer to a pixel, (3 elements,) in the image data array.
	 * Pixels are limited to the boundary of the image.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @param x the x-location.
	 * @param y the y-location.
	 * @return A pointer to the pixel containing 3 elements.
	 */
	template <typename T>
	static T* getPixelPtr(T* imgDataPtr, const Dim& dim, long x, long y);

	/**
	 * Converts a color image to a gray level image.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 */
	template <typename T>
	static void toGray(T* imgDataPtr, const Dim& dim);

	template <typename T, typename U>
	static U* toNewType(T* fromImgDataPtr, const Dim& dim);

	template <typename T>
	static T* clone(T* imgDataPtr, const Dim& dim);

	template <typename T>
	static void mapColor(T* imgDataPtr, const Dim& dim, T fromMin, T fromMax);
};

//Template prototype functions
template void ImageUtils::convolve(unsigned char*, const Dim&, const double*, int);
template void ImageUtils::convolve(double*, const Dim&, const double*, int);
template void ImageUtils::grayConvolve(unsigned char*, const Dim&, const double*, int);
template void ImageUtils::grayConvolve(double*, const Dim&, const double*, int);
template void ImageUtils::eachPixel(unsigned char*, const Dim&, const std::function<void(unsigned char*, long, long)> &eachCallback);
template void ImageUtils::eachPixel(double*, const Dim&, const std::function<void(double*, long, long)> &eachCallback);
template unsigned char* ImageUtils::getPixelPtr(unsigned char*, const Dim&, long, long);
template double* ImageUtils::getPixelPtr(double*, const Dim&, long, long);
template unsigned char* ImageUtils::toNewType(double*, const Dim&);
template double* ImageUtils::toNewType(unsigned char*, const Dim&);
template unsigned char* ImageUtils::clone(unsigned char*, const Dim&);
template double* ImageUtils::clone(double*, const Dim&);
template void ImageUtils::toGray(unsigned char*, const Dim&);
template void ImageUtils::toGray(double*, const Dim&);
template void ImageUtils::mapColor(double* imgDataPtr, const Dim& dim, double fromMin, double fromMax);
template void ImageUtils::mapColor(unsigned char* imgDataPtr, const Dim& dim, unsigned char fromMin, unsigned char fromMax);

template <typename T>
struct ImageWrapper
{
	T* dataPtr;
	Dim dim;
	void convolve(const double* filterArray, const int filterSize)
	{
		ImageUtils::convolve<T>(dataPtr, dim, filterArray, filterSize);
	}
	void grayConvolve(const double* filterArray, const int filterSize)
	{
		ImageUtils::grayConvolve<T>(dataPtr, dim, filterArray, filterSize);
	}
	void eachPixel(const std::function<void(T* rgbArray, long x, long y)> &eachCallback)
	{
		ImageUtils::eachPixel<T>(dataPtr, dim, eachCallback);
	}
	T* getPixelPtr(const long x, const long y)
	{
		return ImageUtils::getPixelPtr<T>(dataPtr, dim, x, y);
	}
	void toGray()
	{
		ImageUtils::toGray(dataPtr, dim);
	}
	template <typename U>
	ImageWrapper<U> toNewType()
	{
		return {
			ImageUtils::toNewType<T, U>(dataPtr, dim),
			dim
		};
	}
	T* clone()
	{
		return ImageUtils::clone(dataPtr, dim);
	}
	void mapColor(T fromMin, T fromMax)
	{
		return ImageUtils::mapColor(dataPtr, dim, fromMin, fromMax);
	}
	ImageWrapper(T* imgDataPtr, Dim dim): dataPtr(imgDataPtr), dim(dim) {}
	ImageWrapper(const ImageWrapper& other)
	{
		dataPtr = ImageUtils::clone(other.dataPtr, other.dim);
		dim = other.dim;
	}
	ImageWrapper& operator=(const ImageWrapper& other)
	{
		delete[] dataPtr;

		dataPtr = ImageUtils::clone(other.dataPtr, dim);
		dim = other.dim;

		return *this;
	}
	~ImageWrapper()
	{
		delete[] dataPtr;
		dataPtr = nullptr;
	}
};