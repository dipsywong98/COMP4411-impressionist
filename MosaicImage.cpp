#include "MosaicImage.h"
#include "MosaicBuilder.h"
#include "impressionistDoc.h"

MosaicImage::MosaicImage(unsigned char* sourceImgPtr, const Dim& sourceDim):
	imgWrapper(ImageWrapper<unsigned char>::makeFromData(sourceImgPtr, sourceDim))
{}

void MosaicImage::segmentTiles()
{
	tiles.clear();
	substitutions.clear();

	const auto segmentColumnCount = imgWrapper.dim.width / MosaicBuilder::TILE_SIZE;
	const auto segmentRowCount = imgWrapper.dim.height / MosaicBuilder::TILE_SIZE;

	for (auto y = 0; y < segmentRowCount; y++)
	{
		std::vector< ImageWrapper<unsigned char> > row;
		std::vector< ImageWrapper<unsigned char>* > subRow;
		for (auto x = 0; x < segmentColumnCount; x++)
		{
			row.push_back(imgWrapper.subImage(
				x * MosaicBuilder::TILE_SIZE,
				y * MosaicBuilder::TILE_SIZE,
				MosaicBuilder::TILE_DIM
			));
			subRow.push_back(nullptr);
		}
		tiles.push_back(row);
		substitutions.push_back(subRow);
	}
}

void MosaicImage::applySubstitutions()
{
	for (auto y = 0uL; y < tiles.size(); y++)
	{
		auto& tilesRow = tiles[y];
		for (auto x = 0uL; x < tilesRow.size(); x++)
		{
			auto& sourceTile = tilesRow[x];
			ImageWrapper<unsigned char>* bestImagePtr = nullptr;
			auto bestScore = INFINITY;

			auto limit = static_cast<int>(MosaicBuilder::tileLimitSliderPtr->value());

			for (auto& [key, set] : MosaicBuilder::imagePool)
			{
				if (limit == 0) break;
				const auto score = MosaicBuilder::calculateScores(sourceTile, set);
				if (score < bestScore)
				{
					bestScore = score;
					bestImagePtr = set.original;
				}
				limit--;
			}

			substitutions[y][x] = bestImagePtr;
		}
	}
}

void MosaicImage::applyPainting(ImpressionistUI* uiPtr)
{
	auto* docPtr = uiPtr->getDocument();

	if (docPtr == nullptr) __debugbreak();

	memset(imgWrapper.dataPtr, 0, imgWrapper.dim.getLength());  //Remove edges

	for (auto y = 0uL; y < tiles.size(); y++)
	{
		auto& tilesRow = tiles[y];
		for (auto x = 0uL; x < tilesRow.size(); x++)
		{
			imgWrapper.pasteImage(x * MosaicBuilder::TILE_SIZE, y * MosaicBuilder::TILE_SIZE, *substitutions[y][x]);
		}
	}

	docPtr->recordHistory();
	std::copy(imgWrapper.dataPtr, imgWrapper.dataPtr + imgWrapper.dim.getLength() - 1, docPtr->m_ucPainting);
}

void MosaicImage::apply(ImpressionistUI* uiPtr)
{
	segmentTiles();
	applySubstitutions();
	applyPainting(uiPtr);
}