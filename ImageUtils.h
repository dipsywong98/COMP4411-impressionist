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

	/**
	 * Converts a type of image array to a new type. New memory is allocated and the old one is not deleted. 
	 *
	 * @tparam T old type.
	 * @tparam U new type.
	 * @param fromImgDataPtr pointer to the old image data array.
	 * @param dim dimension of the image.
	 * @return pointer to new image type array.
	 */
	template <typename T, typename U>
	static U* toNewType(T* fromImgDataPtr, const Dim& dim);

	/**
	 * Clones the image array.
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @return pointer to new array.
	 */
	template <typename T>
	static T* clone(T* imgDataPtr, const Dim& dim);

	/**
	 * Maps color of an image to [0,255]
	 * 
	 * @tparam T type of image data array.
	 * @param imgDataPtr pointer to the image data array.
	 * @param dim dimension of the image.
	 * @param fromMin the minimum value of the image.
	 * @param fromMax the maximum value of the image.
	 */
	template <typename T>
	static void mapColor(T* imgDataPtr, const Dim& dim, T fromMin, T fromMax);

	/**
	 * Calculates the L2 distance of 2 images with the same dimension.
	 * 
	 * @tparam T type of image data array.
	 * @param imgData1Ptr pointer to the first image data array.
	 * @param imgData2Ptr pointer to the second image data array.
	 * @param dim dimension of the image.
	 * @return the L2 distance.
	 */
	template <typename T>
	static double l2(T* imgData1Ptr, T* imgData2Ptr, const Dim& dim);

	/**
	 *  
	 * @tparam T type of image data array.
	 * @param sourceImgDataPtr pointer to the source image data array.
	 * @param sourceDim dimension of the source image.
	 * @param startX x-location of the top left coordinate of the source of the sub image in the source image.
	 * @param startY y-location of the top left coordinate of the source of the sub image in the source image.
	 * @param targetDim dimension of the target image.
	 * @return pointer to new array.
	 */
	template <typename T>
	static T* subImage(T* sourceImgDataPtr, const Dim& sourceDim, long startX, long startY, const Dim& targetDim);

	template <typename T>
	static void pasteImage(T* sourceImgDataPtr, const Dim& sourceDim, long startX, long startY, T* pasteImgDataPtr, const Dim& pasteDim);
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
template void ImageUtils::mapColor(double*, const Dim&, double, double);
template void ImageUtils::mapColor(unsigned char*, const Dim&, unsigned char, unsigned char);
template double ImageUtils::l2(unsigned char*, unsigned char*, const Dim& dim);
template double ImageUtils::l2(double*, double*, const Dim& dim);
template unsigned char* ImageUtils::subImage(unsigned char*, const Dim&, long, long, const Dim&);
template double* ImageUtils::subImage(double*, const Dim&, long, long, const Dim&);
template void ImageUtils::pasteImage(unsigned char*, const Dim&, long, long, unsigned char*, const Dim&);
template void ImageUtils::pasteImage(double*, const Dim&, long, long, double*, const Dim&);

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
	void toGray() const
	{
		ImageUtils::toGray(dataPtr, dim);
	}
	template <typename U>
	ImageWrapper<U> toNewType() const
	{
		return {
			ImageUtils::toNewType<T, U>(dataPtr, dim),
			dim
		};
	}
	ImageWrapper clone() const
	{
		return {
			ImageUtils::clone<T>(dataPtr, dim),
			dim
		};
	}
	void mapColor(T fromMin, T fromMax)
	{
		return ImageUtils::mapColor(dataPtr, dim, fromMin, fromMax);
	}
	double l2(const ImageWrapper& other) const
	{
		return ImageUtils::l2(dataPtr, other.dataPtr, dim);
	}
	ImageWrapper subImage(long x, long y, const Dim& targetDim) const
	{
		return {
			ImageUtils::subImage(dataPtr, dim, x, y, targetDim),
			targetDim
		};
	}
	void pasteImage(long x, long y, const ImageWrapper& other)
	{
		ImageUtils::pasteImage(dataPtr, dim, x, y, other.dataPtr, other.dim);
	}
	ImageWrapper(T* imgDataPtr, const Dim& dim): dataPtr(imgDataPtr), dim(dim) {}
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
	static ImageWrapper<T> makeFromData(T* imgDataPtr, const Dim& dim)
	{
		return {
			ImageUtils::clone(imgDataPtr, dim),
			dim
		};
	}
};