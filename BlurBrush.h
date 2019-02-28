#pragma once
#include "ImpBrush.h"
#include <vector>
#include <FL/gl.h>

class BlurBrush :
	public ImpBrush
{
public:
	BlurBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~BlurBrush();
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void ApplyBlur(Point source, Point target, GLubyte* ref);
	void DrawCircle(Point target, int size);
	char* BrushName(void);

	std::vector<std::vector<float>>kernel;
};

