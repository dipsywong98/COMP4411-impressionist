//
// PixelizeBrush.h
//
// The header file for Point Brush. 
//

#ifndef PixelizeBrush_H
#define PixelizeBrush_H

#include "ImpBrush.h"

class PixelizeBrush : public ImpBrush
{
public:
	PixelizeBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	Point startSource, startTarget;
};

#endif