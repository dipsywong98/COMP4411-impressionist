//
// LineBrush.h
//
// The header file for Line Brush. 
//

#ifndef LINEBRUSH_H
#define LINEBRUSH_H

#include "ImpBrush.h"
#include <functional>

class LineBrush : public ImpBrush
{
public:
	LineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void RightBegin(const Point source, const Point target) override;
	void RightMove(const Point source, const Point target) override;
	void RightEnd(const Point source, const Point target) override;
	void DrawLine(const Point source, const Point target, const double rad);
	char* BrushName(void);
	Point ClippedPoint(const Point origin, const int size, const double rad);
private:
	Point start;
	Point prev;
};

#endif