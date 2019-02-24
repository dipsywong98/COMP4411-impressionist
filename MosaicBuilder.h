#pragma once
#include <FL/Fl_Choice.H>
#include <filesystem>
#include "ImageUtils.h"

class MosaicBuilder
{
public:
	static const int TILE_SIZE;
	static const Dim TILE_DIM;

	static void prepareTiles();
	static Fl_Callback cbPrepareTiles;

private:
	static void generateGradientImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath, const std::filesystem::path& resultGradientPath);
	static void generateColorImage(ImageWrapper<unsigned char>& imgWrapper, const std::filesystem::path& imgPath, const std::filesystem::path& resultColorPath);
};
