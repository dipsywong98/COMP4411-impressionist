#pragma once
#include "ImpBrush.h"
class WarpBrush :
	public ImpBrush
{
public:
	~WarpBrush();
	WarpBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	std::vector<std::vector<float>> kernel;
	Point prev;
};

