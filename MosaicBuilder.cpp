#include "MosaicBuilder.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <filesystem>
#include <FL/fl_ask.H>
#include "Bitmap.h"
#include "ImageUtils.h"
#include "Impressionist.h"
#include "MosaicImage.h"
#include "impressionistUI.h"
#include "impressionistDoc.h"

extern std::vector<std::vector<float>> getGaussianKernel(float sigma, int size);

const int MosaicBuilder::TILE_SIZE = 30;
const Dim MosaicBuilder::TILE_DIM = { TILE_SIZE, TILE_SIZE };
std::filesystem::path MosaicBuilder::tilesPath("");
std::unordered_map<std::string, ImageSet> MosaicBuilder::imagePool;
Fl_Window* MosaicBuilder::windowMosaicPtr = nullptr;
Fl_Value_Slider* MosaicBuilder::weightSliderPtr = nullptr;
Fl_Value_Slider* MosaicBuilder::tileLimitSliderPtr = nullptr;
Fl_Button* MosaicBuilder::btnPrepareAssets = nullptr;
Fl_Button* MosaicBuilder::btnApplyMosaic = nullptr;
ImpressionistUI* MosaicBuilder::uiPtr = nullptr;

void MosaicBuilder::openUi(Fl_Widget* o, void*)
{
	uiPtr = static_cast<ImpressionistUI*>(o->parent()->user_data());

	if (windowMosaicPtr == nullptr)
		buildUi();

	windowMosaicPtr->show();
}


void MosaicBuilder::cbPrepareTiles(Fl_Widget*, void*)
{
	prepareTiles();
}

void MosaicBuilder::cbApplyMosaic(Fl_Widget*, void*)
{
	auto* docPtr = uiPtr->getDocument();
	if (loadAllTiles())
	{
		MosaicImage mosaicImage(docPtr->m_ucPainting, { docPtr->m_nPaintWidth, docPtr->m_nPaintHeight });
		mosaicImage.apply(uiPtr);
		uiPtr->m_paintView->refresh();
		fl_alert("Completed mosaic!");
	}
}


void MosaicBuilder::buildUi()
{
	windowMosaicPtr = new Fl_Window(600, 100, "Mosaic");
	{
		weightSliderPtr = new Fl_Value_Slider(10, 10, 400, 25, "Edge Weight");
		weightSliderPtr->type(FL_HOR_NICE_SLIDER);
		weightSliderPtr->minimum(0);
		weightSliderPtr->maximum(100);
		weightSliderPtr->value(50);
		weightSliderPtr->step(1);
		weightSliderPtr->align(FL_ALIGN_BOTTOM);

		tileLimitSliderPtr = new Fl_Value_Slider(10, 55, 400, 25, "# Tiles Cmp.");
		tileLimitSliderPtr->type(FL_HOR_NICE_SLIDER);
		tileLimitSliderPtr->minimum(0);
		tileLimitSliderPtr->maximum(1200);
		tileLimitSliderPtr->value(200);
		tileLimitSliderPtr->step(10);
		tileLimitSliderPtr->align(FL_ALIGN_BOTTOM);

		btnPrepareAssets = new Fl_Button(440, 10, 150, 25, "Prepare Assets");
		btnPrepareAssets->callback(cbPrepareTiles);

		btnApplyMosaic = new Fl_Button(440, 55, 150, 25, "Apply Mosaic");
		btnApplyMosaic->callback(cbApplyMosaic);
	}
	windowMosaicPtr->end();
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

	// fl_alert("Will now generate required assets from tiles. This will take some time. Press CLOSE to continue!");

	tilesPath = std::filesystem::path(tilesPathStr);
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

			saveGradientImage(img, entry, resultGradientPath);
			saveColorImage(img, entry, resultColorPath);
		}
	}

	fl_alert("Assets generated!");
}

ImageWrapper<unsigned char> MosaicBuilder::makeGradientImage(const ImageWrapper<unsigned char>& imgWrapper)
{
	const auto r = 3.0 / 1020;
	auto doubleImg = imgWrapper.toNewType<double>();
	doubleImg.toGray();

	auto doubleSobelX = doubleImg;
	doubleSobelX.grayConvolve(ImageUtils::SOBEL_X, 3);
	doubleSobelX.eachValue([&](auto& v){v = max( min(v, 510.0), -510.0); });
	doubleSobelX.mapColor(-510, 510);
	auto sobelX = doubleSobelX.toNewType<unsigned char>();

	auto doubleSobelY = doubleImg;
	doubleSobelY.grayConvolve(ImageUtils::SOBEL_Y, 3);
	doubleSobelY.eachValue([&](auto& v) {v = max( min(v, 510.0), -510.0); });
	doubleSobelY.mapColor(-510, 510);
	auto sobelY = doubleSobelY.toNewType<unsigned char>();

	sobelY.eachPixel([&](unsigned char* rgbArray, long x, long y)
	{
		rgbArray[0] = sobelX.getPixelPtr(x, y)[0];
		rgbArray[2] = 255;
	});

	auto gradient = sobelY.maxMagnitudePool(5);

	return gradient;
}

ImageWrapper<unsigned char> MosaicBuilder::makeColorImage(const ImageWrapper<unsigned char>& imgWrapper)
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

	auto colorImg = imgWrapper;

	colorImg.convolve(gaussianKernel, kernelSize);
	auto meanColor = colorImg.meanPool(6);

	return meanColor;
}

bool MosaicBuilder::loadAllTiles()
{
	// Load all gradients, colors, and images to pools. Expected ~15MB of memory use increase.
	if (tilesPath == "")
	{
		fl_alert("Please Prepare Assets first!");
		return false;
	}

	const auto gradientsPath = tilesPath / "gradients";
	const auto colorsPath = tilesPath / "colors";

	for (auto& entry : std::filesystem::directory_iterator(tilesPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".bmp")
		{
			const auto namePath = entry.path().filename();
			const auto name = namePath.string();
			const auto resultGradientPath = gradientsPath / namePath;
			const auto resultColorPath = colorsPath / namePath;

			if (imagePool.count(name) == 0)
			{
				//Load and add images to pool.
				auto width = 0;
				auto height = 0;

				auto a = readBMP(entry.path().string().c_str(), width, height);
				const Dim ad = { width, height };
				auto b = readBMP(resultGradientPath.string().c_str(), width, height);
				const Dim bd = { width, height };
				auto c = readBMP(resultColorPath.string().c_str(), width, height);
				const Dim cd = { width, height };

				imagePool[name] = ImageSet{
					new ImageWrapper<unsigned char>{
						a,
						ad
					},
					new ImageWrapper<unsigned char>{
						b,
						bd
					},
					new ImageWrapper<unsigned char>{
						c,
						cd
					}
				};
			}
		}
	}

	return true;
}

double MosaicBuilder::getEdgeWeight()
{
	if (!weightSliderPtr) __debugbreak();

	return weightSliderPtr->value() / 100.0;
}

double MosaicBuilder::calculateScores(const ImageWrapper<unsigned char>& source, const ImageSet& target)
{
	const auto gradient = makeGradientImage(source);
	const auto gradientDistance = gradient.l2(*target.gradient);

	const auto color = makeColorImage(source);
	const auto colorDistance = color.l2(*target.color);

	const auto eW = getEdgeWeight();

	return eW * gradientDistance + (1 - eW) * colorDistance;
}

void MosaicBuilder::saveGradientImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath,
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

		const auto gradient = makeGradientImage(imgWrapper);

		writeBMP(resultGradientPath.string().c_str(), gradient.dim.width, gradient.dim.height, gradient.dataPtr);
	}
}

void MosaicBuilder::saveColorImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath,
	const std::filesystem::path& resultColorPath)
{
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

		const auto color = makeColorImage(imgWrapper);

		writeBMP(resultColorPath.string().c_str(), color.dim.width, color.dim.height, color.dataPtr);
	}
}
