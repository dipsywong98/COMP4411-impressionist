#pragma once
#include <FL/Fl_Choice.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <filesystem>
#include "ImageUtils.h"
#include <FL/Fl_Value_Slider.H>
#include "impressionistUI.h"

struct ImageSet
{
	ImageWrapper<unsigned char>* original;
	ImageWrapper<unsigned char>* gradient;
	ImageWrapper<unsigned char>* color;
};

class MosaicBuilder
{
public:
	static const int TILE_SIZE;
	static const Dim TILE_DIM;

	static Fl_Callback openUi;
	static Fl_Callback cbPrepareTiles;
	static Fl_Callback cbApplyMosaic;
	static std::filesystem::path tilesPath;
	static std::unordered_map<std::string, ImageSet> imagePool;
	static Fl_Window* windowMosaicPtr;
	static Fl_Value_Slider* weightSliderPtr;
	static Fl_Value_Slider* tileLimitSliderPtr;
	static Fl_Button* btnPrepareAssets;
	static Fl_Button* btnApplyMosaic;
	static ImpressionistUI* uiPtr;

	static void buildUi();
	static void prepareTiles();
	static ImageWrapper<unsigned char> makeGradientImage(const ImageWrapper<unsigned char>& imgWrapper);
	static ImageWrapper<unsigned char> makeColorImage(const ImageWrapper<unsigned char>& imgWrapper);
	static bool loadAllTiles();
	static double getEdgeWeight();
	static double calculateScores(const ImageWrapper<unsigned char>& source, const ImageSet& target);

private:
	static void saveGradientImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath, const std::filesystem::path& resultGradientPath);
	static void saveColorImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath, const std::filesystem::path& resultColorPath);
};