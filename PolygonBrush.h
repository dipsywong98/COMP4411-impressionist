#pragma once
#include "ImpBrush.h"
class PolygonBrush :
	public ImpBrush
{
public:
	PolygonBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL, bool scattered = false);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void DrawPolygon(Point target, int size, int nSides);
	char* BrushName(void);
	PolygonBrush();
	~PolygonBrush() = default;
	bool scattered;
	int nSides = 0;
};
