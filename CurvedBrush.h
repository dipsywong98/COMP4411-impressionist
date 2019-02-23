#pragma once
#include "CircleBrush.h"
#include "impressionistDoc.h"
#include <functional>
#include <vector>
#include "impressionistUI.h"
#include <vector>

class CurvedBrush :
	public CircleBrush
{
public:
	CurvedBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~CurvedBrush();

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushBegin(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	std::vector<Point> makeCurvedBrush(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas);
	void SetColor(GLubyte* color3);
};

