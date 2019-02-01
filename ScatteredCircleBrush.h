#pragma once
#include "CircleBrush.h"
class ScatteredCircleBrush :
	public CircleBrush
{
public:
	ScatteredCircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
	~ScatteredCircleBrush();
};

