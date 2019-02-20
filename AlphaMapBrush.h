#pragma once
#include "ImpBrush.h"

class AlphaMapBrush: public ImpBrush
{
public:
	AlphaMapBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~AlphaMapBrush();
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

