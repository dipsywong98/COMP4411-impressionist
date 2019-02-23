#pragma once
#include "ImpBrush.h"
class CircleBrush :
	public ImpBrush
{
public:
	CircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void DrawCircle(Point target, int size);
	char* BrushName(void);
	CircleBrush();
	~CircleBrush() = default;
};
