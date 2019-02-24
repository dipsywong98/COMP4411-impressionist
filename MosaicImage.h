#pragma once
#include "ImageUtils.h"
#include "impressionistUI.h"

class MosaicImage
{
public:
	MosaicImage(unsigned char* sourceImgPtr, const Dim& sourceDim);
	void apply(ImpressionistUI* uiPtr);

private:
	ImageWrapper<unsigned char> imgWrapper;
	std::vector< std::vector< ImageWrapper<unsigned char> > > tiles;
	std::vector< std::vector< ImageWrapper<unsigned char>* > > substitutions;

	void segmentTiles();
	void applySubstitutions();
	void applyPainting(ImpressionistUI* uiPtr);
};
