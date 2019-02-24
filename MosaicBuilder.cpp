#include "MosaicBuilder.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <filesystem>
#include <FL/fl_ask.H>
#include "Bitmap.h"
#include "ImageUtils.h"
#include "Impressionist.h"

extern std::vector<std::vector<float>> getGaussianKernel(float sigma, int size);

const int MosaicBuilder::TILE_SIZE = 30;
const Dim MosaicBuilder::TILE_DIM = { TILE_SIZE, TILE_SIZE };

void MosaicBuilder::cbPrepareTiles(Fl_Widget*, void*)
{
	prepareTiles();
}

void MosaicBuilder::prepareTiles()
{
	//Load images
	Fl_Native_File_Chooser nativeFileChooser;
	nativeFileChooser.type(Fl_Native_File_Chooser::Type::BROWSE_DIRECTORY);
	nativeFileChooser.directory((std::filesystem::current_path() / "images").string().c_str());
	nativeFileChooser.title("Select path to tile images");
	nativeFileChooser.show();

	auto* tilesPathStr = nativeFileChooser.filename();

	if (strcmp(tilesPathStr, "") == 0)
	{
		fl_alert("Path not selected!");
		return;
	}

	fl_alert("Will now generate required assets from tiles. This will take some time. Press CLOSE to continue!");

	const auto tilesPath = std::filesystem::path(tilesPathStr);
	const auto gradientsPath = tilesPath / "gradients";
	const auto colorsPath = tilesPath / "colors";

	create_directory(gradientsPath);
	create_directory(colorsPath);

	for (auto& entry: std::filesystem::directory_iterator(tilesPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".bmp")
		{
			const auto resultGradientPath = gradientsPath / entry.path().filename();
			const auto resultColorPath = colorsPath / entry.path().filename();

			ImageWrapper<unsigned char> img{
				nullptr,
				TILE_DIM
			};

			generateGradientImage(img, entry, resultGradientPath);
			generateColorImage(img, entry, resultColorPath);
		}
	}

	fl_alert("Assets generated!");
}

void MosaicBuilder::generateGradientImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath,
	const std::filesystem::path& resultGradientPath)
{
	if (!is_regular_file(resultGradientPath))
	{
		// Generate gradient map R:X G:Y B:0
		// Max of sobel: 0*2 - 255*-2 = 510
		// Min of sobel: 0*-2 - 255*2 = -510
		// Map [-510,510] to [0, 255]. x4 of information loss.

		if (imgWrapper.dataPtr == nullptr)
		{
			auto width = 0;
			auto height = 0;
			imgWrapper.dataPtr = readBMP(imgPath.string().c_str(), width, height);
		}

		auto doubleImg = imgWrapper.toNewType<double>();
		doubleImg.toGray();

		auto doubleSobelX = doubleImg;
		doubleSobelX.convolve(ImageUtils::SOBEL_X, 3);
		doubleSobelX.mapColor(-510, 510);
		auto sobelX = doubleSobelX.toNewType<unsigned char>();

		auto doubleSobelY = doubleImg;
		doubleSobelY.convolve(ImageUtils::SOBEL_Y, 3);
		doubleSobelY.mapColor(-510, 510);
		auto sobelY = doubleSobelY.toNewType<unsigned char>();

		sobelY.eachPixel([&](unsigned char* rgbArray, long x, long y)
		{
			rgbArray[0] = sobelX.getPixelPtr(x, y)[0];
			rgbArray[2] = 255;
		});

		writeBMP(resultGradientPath.string().c_str(), TILE_DIM.width, TILE_DIM.height, sobelY.dataPtr);
	}
}

void MosaicBuilder::generateColorImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath,
	const std::filesystem::path& resultColorPath)
{
	static double* gaussianKernel = nullptr;
	static auto kernelSize = 0;

	if (gaussianKernel == nullptr)
	{
		auto vectorKernel = getGaussianKernel(10, 15);

		kernelSize = vectorKernel.size();

		std::vector<double> vectorKernel1D;

		for (auto& row : vectorKernel)
		{
			vectorKernel1D.insert(vectorKernel1D.end(), row.begin(), row.end());
		}

		gaussianKernel = new double[vectorKernel1D.size()];
		std::copy(vectorKernel1D.begin(), vectorKernel1D.end(), gaussianKernel);
	}

	if (!is_regular_file(resultColorPath))
	{
		// Generate color map
		// Low pass filter
		if (imgWrapper.dataPtr == nullptr)
		{
			auto width = 0;
			auto height = 0;
			imgWrapper.dataPtr = readBMP(imgPath.string().c_str(), width, height);
		}

		auto colorImg = imgWrapper;

		colorImg.convolve(gaussianKernel, kernelSize);

		writeBMP(resultColorPath.string().c_str(), TILE_DIM.width, TILE_DIM.height, colorImg.dataPtr);
	}
}
