#pragma once
#include "ImpBrush.h"
#include <vector>
#include "BlurBrush.h"

class SharpenBrush :
	public ImpBrush
{
public:
	SharpenBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~SharpenBrush();
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void ApplySharpen(Point source, Point target, GLubyte* ref);
	char* BrushName(void);

	std::vector<std::vector<float>>kernel;
};

