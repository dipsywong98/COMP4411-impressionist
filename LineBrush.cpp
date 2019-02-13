//
// LineBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "LineBrush.h"
#include <ostream>
#include <iostream>
#include <functional>

extern float frand();
extern double degToRad(double);
extern double radToDeg(double);

LineBrush::LineBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void LineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int width = pDoc->getLineWidth();
	glLineWidth((float)width);
	prev = source;
	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL)
	{
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	SetColor(source);

	switch (ImpBrush::m_DirectionType)
	{
	case SLIDER_RIGHT_CLICK:
		DrawLine(source, target, degToRad(pDoc->getLineAngle()));
		break;
	case GRADIENT:
		{
			Point grad = CalGradient(source, target, [&](int x, int y) {return pDoc->GetOriginalPixel(x,y); });
			const double rad_ = atan2(grad.y, grad.x);
			DrawLine(source, target, rad_);
		}
		break;
	case GRADIENT_ANOTHER:
		{
			Point grad = CalGradient(source, target, [&](int x, int y) {return pDoc->GetAnotherPixel(x,y); });
			const double rad_ = atan2(grad.y, grad.x);
			DrawLine(source, target, rad_);
		}
		break;
	case BRUSH_DIRECTION:
		const double rad = atan2(prev.y - source.y, prev.x - source.x);
		DrawLine(source, target, rad);
		prev = source;
		break;
	}
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
}

void LineBrush::RightBegin(const Point source, const Point target)
{
	start = source;
}

void LineBrush::RightMove(const Point source, const Point target)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(start.x, start.y);
	glVertex2d(source.x, source.y);
	glEnd();
}

void LineBrush::RightEnd(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int angle = static_cast<int>(radToDeg(atan2(source.y - start.y, source.x - start.x)));
	dlg->setLineAngle(angle);
}

void LineBrush::DrawLine(const Point source, const Point target, const double rad)
{
	ImpressionistDoc* pDoc = GetDocument();
	int size = pDoc->getSize();

	const int w = pDoc->m_nPaintWidth;
	const int h = pDoc->m_nPaintHeight;

	const double dsin = size * sin(rad) / 2.0f;
	const double dcos = size * cos(rad) / 2.0f;
	// double x1 = ;
	// double x2 = ;
	// double y1 = ;
	// double y2 = ;
	//
	// if(x1 > w || x2 > w)
	// {
	// 	const double m = (y1 - y2) / (x1 - x2);
	// 	if(x1 > w)
	// 	{
	// 		y1 -= m*(x1 - w);
	// 		x1 = w;
	// 	} else
	// 	{
	// 		y2 -= m*(x2 - w);
	// 		x2 = w;
	// 	}
	// }
	// if(y1 > h || y2 > h)
	// {
	// 	const double m = (x1 - x2) / (y1 - y2);
	// 	if(y1>h)
	// 	{
	// 		x1 -= m*(y1 - h);
	// 		y1 = h;
	// 	} else
	// 	{
	// 		x2 -= m*(y2 - h);
	// 		y2 = h;
	// 	}
	// }

	glBegin(GL_LINES);
	glVertex2d(target.x + dcos, target.y + dsin);
	glVertex2d(target.x - dcos, target.y - dsin);
	glEnd();
}

Point LineBrush::CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel)
{
	ImpressionistDoc* pDoc = GetDocument();
	const int sobel[][3] = {
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};
	int gx = 0, gy = 0;
	// const int img[3][3] = {};
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			const int x = source.x - 1 + i;
			const int y = source.y - 1 + j;
			const int pix = *getPixel(x, y);
			gx += sobel[j][i] * pix;
			gy += sobel[i][j] * pix;
		}
	}
	return Point(gx, gy);
}
